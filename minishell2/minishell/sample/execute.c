/******************************************************************************
 * mini_shell.c
 *
 * 概要:
 *  - Lexer(字句解析):   入力文字列 -> トークン列
 *  - Parser(構文解析): トークン列 -> AST(抽象構文木)
 *  - Executor(実行):   AST -> 実際にfork/exec/builtin/パイプ/リダイレクト/サブシェル実行
 *  - 変数展開:         `$VAR`, `$?` を簡易的に置換
 *  - ヒアドキュメント: "<< word" を検出、子プロセス実行前にパイプ経由で入力を与える
 *  - ビルトイン:       echo, cd, exit (簡易実装)
 *
 * (bashレベルにはほど遠い簡易サンプルであり、完全版ではありません)
 ******************************************************************************/
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/******************************************************************************
 * ユーティリティ (簡易版)
 ******************************************************************************/
static size_t	my_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
		i++;
	return (i);
}
static char	*my_strdup(const char *s)
{
	size_t	len;
	char	*p;

	if (!s)
		return (NULL);
	len = my_strlen(s);
	p = malloc(len + 1);
	if (!p)
		return (NULL);
	memcpy(p, s, len);
	p[len] = '\0';
	return (p);
}
static int	my_strcmp(const char *a, const char *b)
{
	if (!a || !b)
		return (1);
	while (*a && *a == *b)
	{
		a++;
		b++;
	}
	return ((unsigned char)*a - (unsigned char)*b);
}
static bool	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v'
		|| c == '\f');
}

/******************************************************************************
 * 環境変数リスト (超簡易版)
 *   - "key=value" スタイルをシンプルに保持する
 ******************************************************************************/
typedef struct s_env
{
	char *key;   // 例: "HOME"
	char *value; // 例: "/home/user"
	struct s_env	*next;
}					t_env;

/* 環境変数を検索してvalueを返す (なければNULL) */
static const char	*env_get(t_env *env, const char *key)
{
	while (env)
	{
		if (!my_strcmp(env->key, key))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/* 環境変数をセット/更新 */
static void	env_set(t_env **penv, const char *key, const char *val)
{
	t_env	*cur;
	t_env	*n;

	cur = *penv;
	while (cur)
	{
		if (!my_strcmp(cur->key, key))
		{
			free(cur->value);
			cur->value = my_strdup(val ? val : "");
			return ;
		}
		cur = cur->next;
	}
	// 新規
	n = malloc(sizeof(t_env));
	n->key = my_strdup(key);
	n->value = my_strdup(val ? val : "");
	n->next = *penv;
	*penv = n;
}

/* 環境リストを全削除 */
static void	env_freeall(t_env *env)
{
	t_env	*nx;

	while (env)
	{
		nx = env->next;
		free(env->key);
		free(env->value);
		free(env);
		env = nx;
	}
}

/******************************************************************************
 * (1) Lexer
 ******************************************************************************/
typedef enum e_toktype
{
	TOK_WORD,
	TOK_OP,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_EOF
}					t_toktype;

typedef struct s_token
{
	t_toktype		type;
	char			*value;
}					t_token;

/* トークン配列を作るヘルパ */
static bool	add_token(t_token ***ptoks, int *count, t_toktype t,
		const char *val)
{
	t_token	**toks;
	t_token	*newt;
	t_token	**tmp;

	toks = *ptoks;
	newt = malloc(sizeof(t_token));
	if (!newt)
		return (false);
	newt->type = t;
	newt->value = (val ? my_strdup(val) : NULL);
	tmp = realloc(toks, sizeof(t_token *) * (*count + 2));
	if (!tmp)
	{
		free(newt->value);
		free(newt);
		return (false);
	}
	toks = tmp;
	toks[*count] = newt;
	(*count)++;
	toks[*count] = NULL;
	*ptoks = toks;
	return (true);
}

static void	free_tokens(t_token **toks)
{
	if (!toks)
		return ;
	for (int i = 0; toks[i]; i++)
	{
		free(toks[i]->value);
		free(toks[i]);
	}
	free(toks);
}

static bool	is_metachar(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '&' || c == '('
		|| c == ')');
}
static bool	read_operator(const char *line, int *pos, t_token ***ptoks,
		int *count)
{
	char	c;
		char op[2];

	c = line[*pos];
	char d = line[*pos + 1]; // 範囲外アクセス注意
	// 1) '('
	if (c == '(')
	{
		(*pos)++;
		return (add_token(ptoks, count, TOK_LPAREN, "("));
	}
	// 2) ')'
	else if (c == ')')
	{
		(*pos)++;
		return (add_token(ptoks, count, TOK_RPAREN, ")"));
	}
	// 3) 2文字演算子 (<<, >>, &&, ||)
	else if (c == '<' && d == '<')
	{
		(*pos) += 2;
		return (add_token(ptoks, count, TOK_OP, "<<"));
	}
	else if (c == '>' && d == '>')
	{
		(*pos) += 2;
		return (add_token(ptoks, count, TOK_OP, ">>"));
	}
	else if (c == '&' && d == '&')
	{
		(*pos) += 2;
		return (add_token(ptoks, count, TOK_OP, "&&"));
	}
	else if (c == '|' && d == '|')
	{
		(*pos) += 2;
		return (add_token(ptoks, count, TOK_OP, "||"));
	}
	// 4) 1文字演算子 (|, <, >)
	else if (c == '|' || c == '<' || c == '>')
	{
		// cだけを単文字にして add_token
		op[0] = c;
		op[1] = '\0';
		(*pos)++;
		return (add_token(ptoks, count, TOK_OP, op));
	}
	// 5) 上記いずれにも該当しない => 未知
	else
	{
		return (false);
	}
}

/* クォートを除去しながらワード読み */
static char	*read_word(const char *line, int *pos)
{
	int		capacity;
	int		len;
	char	*buf;
	char	*tmp;

	capacity = 128;
	len = 0;
	buf = malloc(capacity);
	if (!buf)
		return (NULL);
	buf[0] = '\0';
	while (line[*pos] && !is_whitespace(line[*pos]) && !is_metachar(line[*pos]))
	{
		if (line[*pos] == '\'')
		{
			(*pos)++;
			while (line[*pos] && line[*pos] != '\'')
			{
				if (len + 2 >= capacity)
				{
					capacity *= 2;
					tmp = realloc(buf, capacity);
					if (!tmp)
					{
						free(buf);
						return (NULL);
					}
					buf = tmp;
				}
				buf[len++] = line[*pos];
				buf[len] = '\0';
				(*pos)++;
			}
			if (line[*pos] == '\'')
				(*pos)++;
			else
			{
				free(buf);
				return (NULL);
			} //不一致
		}
		else if (line[*pos] == '"')
		{
			(*pos)++;
			while (line[*pos] && line[*pos] != '"')
			{
				if (len + 2 >= capacity)
				{
					capacity *= 2;
					tmp = realloc(buf, capacity);
					if (!tmp)
					{
						free(buf);
						return (NULL);
					}
					buf = tmp;
				}
				buf[len++] = line[*pos];
				buf[len] = '\0';
				(*pos)++;
			}
			if (line[*pos] == '"')
				(*pos)++;
			else
			{
				free(buf);
				return (NULL);
			}
		}
		else
		{
			//通常文字
			if (len + 2 >= capacity)
			{
				capacity *= 2;
				tmp = realloc(buf, capacity);
				if (!tmp)
				{
					free(buf);
					return (NULL);
				}
				buf = tmp;
			}
			buf[len++] = line[*pos];
			buf[len] = '\0';
			(*pos)++;
		}
	}
	return (buf);
}

/* lexer本体 */
static t_token	**lexer(const char *line, int *err)
{
	t_token	**toks;
	int		tcount;
	int		pos;
	int		length;
	char	*w;

	*err = 0;
	toks = malloc(sizeof(t_token *));
	if (!toks)
		return (NULL);
	toks[0] = NULL;
	tcount = 0;
	pos = 0;
	length = my_strlen(line);
	while (pos < length)
	{
		if (is_whitespace(line[pos]))
		{
			pos++;
			continue ;
		}
		if (is_metachar(line[pos]))
		{
			if (!read_operator(line, &pos, &toks, &tcount))
			{
				fprintf(stderr, "[Lexer] unknown operator near '%c'\n",
					line[pos]);
				*err = 2; // syntax error
				free_tokens(toks);
				return (NULL);
			}
		}
		else
		{
			w = read_word(line, &pos);
			if (!w)
			{
				fprintf(stderr, "[Lexer] unclosed quote?\n");
				*err = 2;
				free_tokens(toks);
				return (NULL);
			}
			if (!add_token(&toks, &tcount, TOK_WORD, w))
			{
				free(w);
				free_tokens(toks);
				*err = 2;
				return (NULL);
			}
			free(w);
		}
	}
	add_token(&toks, &tcount, TOK_EOF, "");
	return (toks);
}

/******************************************************************************
 * (2) Parser: AST構築 (再帰下降)
 ******************************************************************************/
typedef enum e_asttype
{
	AST_CMD,
	AST_PIPE,
	AST_AND,
	AST_OR,
	AST_SUBSHELL
}					t_asttype;

typedef enum e_redirtype
{
	REDIR_IN,
	REDIR_HEREDOC,
	REDIR_OUT,
	REDIR_APPEND
}					t_redirtype;

/* リダイレクトリスト */
typedef struct s_redir
{
	t_redirtype		rtype;
	char *filename; // or heredoc delimiter
	struct s_redir	*next;
}					t_redir;

typedef struct s_ast
{
	t_asttype		type;
	struct s_ast	*left;
	struct s_ast	*right;
	struct s_ast *child; // for subshell

	char **argv;    // AST_CMDの場合
	t_redir *redir; // リダイレクト一覧( CMD or SUBSHELL に付与可能 )
}					t_ast;

static t_ast	*new_ast(t_asttype t)
{
	t_ast	*n;

	n = malloc(sizeof(t_ast));
	if (!n)
		return (NULL);
	n->type = t;
	n->left = NULL;
	n->right = NULL;
	n->child = NULL;
	n->argv = NULL;
	n->redir = NULL;
	return (n);
}
static void	add_argv(t_ast *node, const char *word)
{
	int		count;
	char	**tmp;

	if (!node || node->type != AST_CMD)
		return ;
	count = 0;
	if (node->argv)
	{
		while (node->argv[count])
			count++;
	}
	tmp = realloc(node->argv, sizeof(char *) * (count + 2));
	if (!tmp)
		return ;
	node->argv = tmp;
	node->argv[count] = my_strdup(word);
	node->argv[count + 1] = NULL;
}
static void	add_redir(t_ast *node, t_redirtype rt, const char *fname)
{
	t_redir	*r;
	t_redir	*p;

	r = malloc(sizeof(t_redir));
	r->rtype = rt;
	r->filename = my_strdup(fname);
	r->next = NULL;
	if (!node->redir)
		node->redir = r;
	else
	{
		p = node->redir;
		while (p->next)
			p = p->next;
		p->next = r;
	}
}

/* tokenリストを読みながらパースするための構造 */
typedef struct s_parser
{
	t_token			**toks;
	int				pos;
}					t_parser;

static t_token	*curr_token(t_parser *p)
{
	return (p->toks[p->pos]);
}
static t_token	*next_token(t_parser *p)
{
	return (p->toks[++(p->pos)]);
}
static bool	match_op(t_parser *p, const char *op)
{
	t_token	*tk;

	tk = curr_token(p);
	return (tk->type == TOK_OP && !my_strcmp(tk->value, op));
}
static bool	match_type(t_parser *p, t_toktype t)
{
	return (curr_token(p)->type == t);
}

static void	parse_redirs(t_parser *p, t_ast *node)
{
	char	*op;

	while (true)
	{
		// トークンが演算子(OP)でなければ終了
		if (!match_type(p, TOK_OP))
			break ;
		op = curr_token(p)->value;
		// 毎回ループ先頭で最新の op を取得
		if (!my_strcmp(op, "<"))
		{
			next_token(p);
			if (match_type(p, TOK_WORD))
			{
				add_redir(node, REDIR_IN, curr_token(p)->value);
				next_token(p);
			}
			else
			{
				fprintf(stderr, "syntax error after <\n");
				break ;
			}
		}
		else if (!my_strcmp(op, "<<"))
		{
			next_token(p);
			if (match_type(p, TOK_WORD))
			{
				add_redir(node, REDIR_HEREDOC, curr_token(p)->value);
				next_token(p);
			}
			else
			{
				fprintf(stderr, "syntax error after <<\n");
				break ;
			}
		}
		else if (!my_strcmp(op, ">"))
		{
			next_token(p);
			if (match_type(p, TOK_WORD))
			{
				add_redir(node, REDIR_OUT, curr_token(p)->value);
				next_token(p);
			}
			else
			{
				fprintf(stderr, "syntax error after >\n");
				break ;
			}
		}
		else if (!my_strcmp(op, ">>"))
		{
			next_token(p);
			if (match_type(p, TOK_WORD))
			{
				add_redir(node, REDIR_APPEND, curr_token(p)->value);
				next_token(p);
			}
			else
			{
				fprintf(stderr, "syntax error after >>\n");
				break ;
			}
		}
		else
		{
			// リダイレクト演算子以外 => ループ終了
			break ;
		}
	}
}

/* command := ( word... [redirs...] ) | ( '(' cmdline ')' [redirs...] ) */
static t_ast		*parse_command(t_parser *p);

/* pipeline := command { '|' command } */
static t_ast		*parse_pipeline(t_parser *p);

/* logical_and := pipeline { '&&' pipeline } */
static t_ast		*parse_logical_and(t_parser *p);

/* logical_or := logical_and { '||' logical_and } */
static t_ast		*parse_logical_or(t_parser *p);

/* cmdline := logical_or */
static t_ast	*parse_cmdline(t_parser *p)
{
	return (parse_logical_or(p));
}

static t_ast	*parse_command(t_parser *p)
{
	t_ast	*sub;
	t_ast	*node;

	// '(' cmdline ')' => subshell
	if (match_type(p, TOK_LPAREN))
	{
		next_token(p);
		sub = parse_cmdline(p);
		if (!match_type(p, TOK_RPAREN))
		{
			fprintf(stderr, "missing )\n");
		}
		else
		{
			next_token(p);
		}
		node = new_ast(AST_SUBSHELL);
		node->child = sub;
		parse_redirs(p, node);
		return (node);
	}
	// 普通のCMD
	node = new_ast(AST_CMD);
	while (match_type(p, TOK_WORD))
	{
		add_argv(node, curr_token(p)->value);
		next_token(p);
	}
	parse_redirs(p, node);
	return (node);
}

static t_ast	*parse_pipeline(t_parser *p)
{
	t_ast	*left;
	t_ast	*right;
	t_ast	*pipe;

	left = parse_command(p);
	while (match_op(p, "|"))
	{
		next_token(p); // consume |
		right = parse_command(p);
		pipe = new_ast(AST_PIPE);
		pipe->left = left;
		pipe->right = right;
		left = pipe;
	}
	return (left);
}

static t_ast	*parse_logical_and(t_parser *p)
{
	t_ast	*left;
	t_ast	*right;
	t_ast	*node;

	left = parse_pipeline(p);
	while (match_op(p, "&&"))
	{
		next_token(p);
		right = parse_pipeline(p);
		node = new_ast(AST_AND);
		node->left = left;
		node->right = right;
		left = node;
	}
	return (left);
}

static t_ast	*parse_logical_or(t_parser *p)
{
	t_ast	*left;
	t_ast	*right;
	t_ast	*node;

	left = parse_logical_and(p);
	while (match_op(p, "||"))
	{
		next_token(p);
		right = parse_logical_and(p);
		node = new_ast(AST_OR);
		node->left = left;
		node->right = right;
		left = node;
	}
	return (left);
}

/* トップレベルパース */
static t_ast	*parse_tokens(t_token **toks)
{
	t_parser	p;
	t_ast		*root;

	p.toks = toks;
	p.pos = 0;
	root = parse_cmdline(&p);
	if (toks[p.pos] && toks[p.pos]->type != TOK_EOF)
	{
		fprintf(stderr, "warning: trailing tokens?\n");
	}
	return (root);
}

/******************************************************************************
 * AST解放
 ******************************************************************************/
static void	free_redirs(t_redir *r)
{
	t_redir	*nx;

	while (r)
	{
		nx = r->next;
		free(r->filename);
		free(r);
		r = nx;
	}
}
static void	free_ast(t_ast *n)
{
	if (!n)
		return ;
	free_ast(n->left);
	free_ast(n->right);
	free_ast(n->child);
	if (n->argv)
	{
		for (int i = 0; n->argv[i]; i++)
			free(n->argv[i]);
		free(n->argv);
	}
	free_redirs(n->redir);
	free(n);
}

/******************************************************************************
 * 変数展開 (簡易版)
 *   - $?: 直前の終了ステータスを文字列化
 *   - $VAR: envリストで検索
 *   - その他: $だけor不明変数は空文字にする(本サンプルでは)
 *
 * 実際には文字列中途の $VAR や ${VAR} 構文など、複雑な処理が必要だが省略。
 ******************************************************************************/
static bool	is_var_char(char c)
{
	return (isalnum((unsigned char)c) || (c == '_'));
}

static char	*expand_var(const char *src, int last_status, t_env *env)
{
	size_t		len;
	int			rpos;
	char		tmp[32];
	int			start;
	int			vlen;
	char		*varname;
	const char	*val = env_get(env, varname);

	if (!src)
		return (NULL);
	len = my_strlen(src);
	char *res = malloc(len * 2 + 1); // 適当に2倍確保(甘い)
	if (!res)
		return (NULL);
	rpos = 0;
	for (int i = 0; i < (int)len; i++)
	{
		if (src[i] == '$')
		{
			// $? 判定
			if (src[i + 1] == '?')
			{
				snprintf(tmp, sizeof(tmp), "%d", last_status);
				for (int k = 0; tmp[k]; k++)
					res[rpos++] = tmp[k];
				i++;
			}
			else
			{
				// $VAR
				start = i + 1;
				vlen = 0;
				while (is_var_char(src[start + vlen]))
					vlen++;
				if (vlen > 0)
				{
					varname = malloc(vlen + 1);
					memcpy(varname, &src[start], vlen);
					varname[vlen] = '\0';
					if (val)
					{
						for (int k = 0; val[k]; k++)
							res[rpos++] = val[k];
					}
					free(varname);
					i += vlen;
				}
				else
				{
					// $だけ
					// (本サンプルでは空にする)
				}
			}
		}
		else
		{
			res[rpos++] = src[i];
		}
	}
	res[rpos] = '\0';
	return (res);
}

/******************************************************************************
 * (3) Executor
 *   - ASTを走査し、再帰的にコマンド実行やパイプ/論理演算/サブシェルを実行
 *   - リダイレクト(open/dup2), ヒアドキュメント, 変数展開を行う
 ******************************************************************************/
typedef struct s_shell
{
	t_env			*env;
	int				last_status;
}					t_shell;

/* ビルトイン簡易実装 */
static bool	is_builtin(const char *cmd)
{
	if (!cmd)
		return (false);
	if (!my_strcmp(cmd, "echo"))
		return (true);
	if (!my_strcmp(cmd, "cd"))
		return (true);
	if (!my_strcmp(cmd, "exit"))
		return (true);
	return (false);
}

static int	run_builtin(char **argv, t_shell *sh)
{
	int			i;
	const char	*home = env_get(sh->env, "HOME");
	int			code;

	if (!my_strcmp(argv[0], "echo"))
	{
		// echoは単純に argv[1..] をスペース区切りで出力
		// "-n" 等省略
		i = 1;
		while (argv[i])
		{
			printf("%s", argv[i]);
			if (argv[i + 1])
				printf(" ");
			i++;
		}
		printf("\n");
		return (0);
	}
	else if (!my_strcmp(argv[0], "cd"))
	{
		if (!argv[1])
		{
			// cd ~
			if (!home)
			{
				fprintf(stderr, "cd: HOME not set\n");
				return (1);
			}
			if (chdir(home) < 0)
			{
				perror("cd");
				return (1);
			}
			return (0);
		}
		else
		{
			if (chdir(argv[1]) < 0)
			{
				perror("cd");
				return (1);
			}
			return (0);
		}
	}
	else if (!my_strcmp(argv[0], "exit"))
	{
		// exit [n]
		code = 0;
		if (argv[1])
			code = atoi(argv[1]); //簡易
		// bash だと "exit 999999999999999999999" 等で2返す等があるが省略
		_exit(code);
	}
	return (0); //ここには来ない
}

/* リダイレクト適用: <, >, >>, << */
static int	apply_redirects(t_redir *r, t_shell *sh)
{
	int		fd;
	int		pipefd[2];
	char	*line;
	size_t	cap;
	ssize_t	n;

	while (r)
	{
		if (r->rtype == REDIR_IN)
		{
			fd = open(r->filename, O_RDONLY);
			if (fd < 0)
			{
				perror(r->filename);
				return (1);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (r->rtype == REDIR_OUT)
		{
			fd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
			{
				perror(r->filename);
				return (1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (r->rtype == REDIR_APPEND)
		{
			fd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd < 0)
			{
				perror(r->filename);
				return (1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (r->rtype == REDIR_HEREDOC)
		{
			// ヒアドキュメント実装: delimiter=r->filename
			// 簡単に、行入力を読み込んで delimiter と一致したら終了
			// ここではfork前に実行しているので、子プロセス/パイプでやるのが本来は多い
			// 簡易のため親側でやり、結果を一時FDに書いてdup2
			if (pipe(pipefd) < 0)
			{
				perror("pipe");
				return (1);
			}
			// ヒアドキュメント(対話)
			while (true)
			{
				line = NULL;
				cap = 0;
				printf("> ");
				fflush(stdout);
				n = getline(&line, &cap, stdin);
				if (n <= 0)
				{
					free(line);
					break ;
				} // ctrl+D
				// 改行含むので除去
				if (line[n - 1] == '\n')
					line[n - 1] = '\0';
				if (!my_strcmp(line, r->filename))
				{
					free(line);
					break ; // delimiter -> end
				}
				// 書き込み
				write(pipefd[1], line, strlen(line));
				write(pipefd[1], "\n", 1);
				free(line);
			}
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
		}
		r = r->next;
	}
	return (0);
}

/* コマンド実行(AST_CMD) */
static int	exec_cmd(t_ast *cmdnode, t_shell *sh)
{
	char	*expanded;
	int		backup_stdin;
	int		backup_stdout;
	int		status;
	pid_t	pid;
	int		wstatus;

	// argvが空なら何もしない
	if (!cmdnode->argv || !cmdnode->argv[0])
	{
		return (0);
	}
	// 変数展開
	for (int i = 0; cmdnode->argv[i]; i++)
	{
		expanded = expand_var(cmdnode->argv[i], sh->last_status, sh->env);
		free(cmdnode->argv[i]);
		cmdnode->argv[i] = expanded;
	}
	// ビルトイン判定
	if (is_builtin(cmdnode->argv[0]))
	{
		// リダイレクトは子を作らずに適用(本サンプルでは注意)
		backup_stdin = dup(STDIN_FILENO);
		backup_stdout = dup(STDOUT_FILENO);
		if (apply_redirects(cmdnode->redir, sh))
		{
			dup2(backup_stdin, STDIN_FILENO);
			dup2(backup_stdout, STDOUT_FILENO);
			close(backup_stdin);
			close(backup_stdout);
			return (1);
		}
		status = run_builtin(cmdnode->argv, sh);
		dup2(backup_stdin, STDIN_FILENO);
		dup2(backup_stdout, STDOUT_FILENO);
		close(backup_stdin);
		close(backup_stdout);
		return (status);
	}
	// 外部コマンド
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		// child
		if (apply_redirects(cmdnode->redir, sh))
		{
			_exit(1);
		}
		execvp(cmdnode->argv[0], cmdnode->argv);
		perror(cmdnode->argv[0]);
		_exit(127);
	}
	else
	{
		wstatus = 0;
		waitpid(pid, &wstatus, 0);
		if (WIFEXITED(wstatus))
		{
			return (WEXITSTATUS(wstatus));
		}
		if (WIFSIGNALED(wstatus))
		{
			return (128 + WTERMSIG(wstatus));
		}
		return (1);
	}
}

/* executor本体 */
static int			executor(t_ast *node, t_shell *sh);

/* パイプ: left|right */
static int	exec_pipe(t_ast *node, t_shell *sh)
{
	int		pipefd[2];
	pid_t	pid;
	int		st;
	pid_t	pid2;
	int		st1;
	int		st2;

	st1 = 0, st2;
	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return (1);
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		// child for left
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
		st = executor(node->left, sh);
		_exit(st);
	}
	else
	{
		// parent, run right in subproc
		pid2 = fork();
		if (pid2 < 0)
		{
			perror("fork2");
			return (1);
		}
		if (pid2 == 0)
		{
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[1]);
			close(pipefd[0]);
			st = executor(node->right, sh);
			_exit(st);
		}
		else
		{
			close(pipefd[0]);
			close(pipefd[1]);
			st1 = 0, st2 = 0;
			waitpid(pid, &st1, 0);
			waitpid(pid2, &st2, 0);
			if (WIFEXITED(st2))
				return (WEXITSTATUS(st2));
			if (WIFSIGNALED(st2))
				return (128 + WTERMSIG(st2));
			return (1); // fallback
		}
	}
}

/* サブシェル: (childAST) + 自分のリダイレクト */
static int	exec_subshell(t_ast *node, t_shell *sh)
{
	pid_t	pid;
	int		st;
	int		wstatus;

	// forkして子で実行
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		// child
		if (apply_redirects(node->redir, sh))
		{
			_exit(1);
		}
		st = executor(node->child, sh);
		_exit(st);
	}
	else
	{
		wstatus = 0;
		waitpid(pid, &wstatus, 0);
		if (WIFEXITED(wstatus))
		{
			return (WEXITSTATUS(wstatus));
		}
		if (WIFSIGNALED(wstatus))
		{
			return (128 + WTERMSIG(wstatus));
		}
		return (1);
	}
}

/* AND/OR: 短絡評価 */
static int	exec_and_or(t_ast *node, t_shell *sh)
{
	int	left_st;
	int	right_st;

	left_st = executor(node->left, sh);
	if (node->type == AST_AND)
	{
		// leftが成功(0)ならright実行
		if (left_st == 0)
		{
			right_st = executor(node->right, sh);
			return (right_st);
		}
		else
		{
			return (left_st);
		}
	}
	else
	{ // AST_OR
		// leftが失敗(!=0)ならright実行
		if (left_st != 0)
		{
			right_st = executor(node->right, sh);
			return (right_st);
		}
		else
		{
			return (left_st);
		}
	}
}

static int	executor(t_ast *node, t_shell *sh)
{
	if (!node)
		return (0);
	switch (node->type)
	{
	case AST_CMD:
		return (exec_cmd(node, sh));
	case AST_PIPE:
		return (exec_pipe(node, sh));
	case AST_AND:
	case AST_OR:
		return (exec_and_or(node, sh));
	case AST_SUBSHELL:
		return (exec_subshell(node, sh));
	}
	return (0);
}

/******************************************************************************
 * main: テスト用
 ******************************************************************************/
int	main(int argc, char **argv)
{
	const char	*input = argv[1];
	t_env		*env;
	int			lexerr;
	t_token		**tokens;
	const char	*tn = "???";
	t_ast		*root;
	t_shell		shell;
	int			status;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s \"command line\"\n", argv[0]);
		return (1);
	}
	printf("[Input] %s\n", input);
	/* 環境変数リスト(簡易構築) */
	env = NULL;
	env_set(&env, "HOME", "/home/testuser");
	env_set(&env, "PATH", "/bin:/usr/bin");
	env_set(&env, "PWD", "/tmp");
	/* Lexer */
	lexerr = 0;
	tokens = lexer(input, &lexerr);
	if (!tokens || lexerr != 0)
	{
		fprintf(stderr, "Lexer error.\n");
		if (tokens)
			free_tokens(tokens);
		env_freeall(env);
		return (lexerr ? lexerr : 1);
	}
	printf("--- Tokens ---\n");
	for (int i = 0; tokens[i]; i++)
	{
		switch (tokens[i]->type)
		{
		case TOK_WORD:
			tn = "WORD";
			break ;
		case TOK_OP:
			tn = "OP";
			break ;
		case TOK_LPAREN:
			tn = "LPAREN";
			break ;
		case TOK_RPAREN:
			tn = "RPAREN";
			break ;
		case TOK_EOF:
			tn = "EOF";
			break ;
		}
		printf("[%d] %s('%s')\n", i, tn, tokens[i]->value);
		if (tokens[i]->type == TOK_EOF)
			break ;
	}
	/* Parser -> AST */
	root = parse_tokens(tokens);
	if (!root)
	{
		free_tokens(tokens);
		env_freeall(env);
		return (2);
	}
	/* Executor */
	shell.env = env;
	shell.last_status = 0;
	status = executor(root, &shell);
	printf("Command finished with status=%d\n", status);
	/* 後始末 */
	free_ast(root);
	free_tokens(tokens);
	env_freeall(env);
	return (status);
}
