
/******************************************************************************
 * mini_shell.c
 *
 * ポイント:
 *  1) ビルトインを含む全コマンドを子プロセスで実行 → パイプライン中の `exit` でも親は終了しない
 *  2) シングルクォート/ダブルクォートでの `$` 展開挙動を bash に近づける
 *  3) `$USERabc`, `$?abc` などをある程度再現:
 *     - `$?abc` => "<exitstatus>abc"
 *     - `$USERabc` => getenv("USERabc")
 *  4) Lexer->Parser->AST->Executor(再帰下降) + Readline
 *
 * 注意:
 *  - バックスラッシュ(\)や${VAR}などは未対応
 *  - Bashほど厳密ではない
 ******************************************************************************/
#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* for readline */
#include <readline/history.h>
#include <readline/readline.h>

/******************************************************************************
 * ユーティリティ
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
	while (*a && *b && *a == *b)
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
 * (1) Lexer
 ******************************************************************************/
typedef enum e_toktype
{
	TOK_WORD,
	TOK_OP,     // |, ||, &&, <, <<, >, >> など
	TOK_LPAREN, // (
	TOK_RPAREN, // )
	TOK_EOF
}					t_toktype;

typedef struct s_token
{
	t_toktype		type;
	char *value; // raw string
}					t_token;

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
	newt->value = val ? my_strdup(val) : NULL;
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
	return (c == '(' || c == ')' || c == '&' || c == '|' || c == '<'
		|| c == '>');
}

/* 2文字演算子 + 1文字演算子 + ( ) */
static bool	read_operator(const char *line, int *pos, t_token ***ptoks,
		int *count)
{
	int		len;
	char	c;
	char	d;
	char	op[2];

	len = strlen(line);
	c = line[*pos];
	d = '\0';
	if (*pos + 1 < len)
		d = line[*pos + 1];
	/* "(" */
	if (c == '(')
	{
		(*pos)++;
		return (add_token(ptoks, count, TOK_LPAREN, "("));
	}
	/* ")" */
	else if (c == ')')
	{
		(*pos)++;
		return (add_token(ptoks, count, TOK_RPAREN, ")"));
	}
	/* 2文字 */
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
	/* 1文字 */
	else if (c == '|' || c == '<' || c == '>')
	{
		op[0] = c;
		op[1] = '\0';
		(*pos)++;
		return (add_token(ptoks, count, TOK_OP, op));
	}
	return (false); // 未知
}

/* クォート付きWORDを読み取る */
static char	*read_word(const char *line, int *pos)
{
	int		cap;
	char	*buf;
	int		len;
	int		L;
	char	*tmp;

	cap = 128;
	buf = malloc(cap);
	if (!buf)
		return (NULL);
	len = 0;
	buf[0] = '\0';
	L = strlen(line);
	while (*pos < L && !is_whitespace(line[*pos]) && !is_metachar(line[*pos]))
	{
		/* シングルクォート */
		if (line[*pos] == '\'')
		{
			(*pos)++;
			while (*pos < L && line[*pos] != '\'')
			{
				if (len + 2 >= cap)
				{
					cap *= 2;
					tmp = realloc(buf, cap);
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
			if (*pos < L && line[*pos] == '\'')
				(*pos)++;
			else
				fprintf(stderr, "Unclosed single quote?\n");
		}
		/* ダブルクォート */
		else if (line[*pos] == '"')
		{
			(*pos)++;
			while (*pos < L && line[*pos] != '"')
			{
				if (len + 2 >= cap)
				{
					cap *= 2;
					tmp = realloc(buf, cap);
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
			if (*pos < L && line[*pos] == '"')
				(*pos)++;
			else
				fprintf(stderr, "Unclosed double quote?\n");
		}
		else
		{
			// 通常文字
			if (len + 2 >= cap)
			{
				cap *= 2;
				tmp = realloc(buf, cap);
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
	int		L;
	char	*w;

	toks = malloc(sizeof(t_token *));
	if (!toks)
		return (NULL);
	toks[0] = NULL;
	tcount = 0;
	*err = 0;
	pos = 0;
	L = strlen(line);
	while (pos < L)
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
				*err = 2;
				free_tokens(toks);
				return (NULL);
			}
		}
		else
		{
			w = read_word(line, &pos);
			if (!w)
			{
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
	// EOF
	add_token(&toks, &tcount, TOK_EOF, "");
	return (toks);
}

/******************************************************************************
 * (2) 変数展開など
 ******************************************************************************/

/* 大文字小文字区別なく [a-zA-Z0-9_] を変数文字とする */
static bool	is_var_char2(char c)
{
	return (isalnum((unsigned char)c) || c == '_');
}

static char	*itoa_status(int st)
{
	char	tmp[32];

	snprintf(tmp, sizeof(tmp), "%d", st);
	return (my_strdup(tmp));
}

/* expand_token():
 *   シングルクォート内 => $無効
 *   ダブルクォート内 => $有効
 *   クォート外 => $有効
 *   $?abc => "<status>abc"
 *   $HOME123 => getenv("HOME123")
 */
static char	*expand_token(const char *word, int last_st)
{
	int		L;
	char	*res;
	int		rpos;
	bool	in_squote;
	bool	in_dquote;
	int		i;
	char	*st_str;
	int		start;
	int		vlen;
	char	*varname;
	char	*val;

	if (!word)
		return (NULL);
	L = strlen(word);
	res = malloc(L * 2 + 1);
	if (!res)
		return (NULL);
	rpos = 0;
	in_squote = false;
	in_dquote = false;
	i = 0;
	while (i < L)
	{
		if (in_squote)
		{
			/* シングルクォート内 */
			if (word[i] == '\'')
			{
				in_squote = false;
				res[rpos++] = word[i];
				res[rpos] = '\0';
				i++;
			}
			else
			{
				// リテラル
				res[rpos++] = word[i];
				res[rpos] = '\0';
				i++;
			}
		}
		else if (in_dquote)
		{
			/* ダブルクォート内 */
			if (word[i] == '"')
			{
				in_dquote = false;
				res[rpos++] = word[i];
				res[rpos] = '\0';
				i++;
			}
			else if (word[i] == '$')
			{
				// $?
				if (word[i + 1] == '?')
				{
					st_str = itoa_status(last_st);
					for (int k = 0; st_str[k]; k++)
						res[rpos++] = st_str[k];
					free(st_str);
					i += 2;
				}
				else
				{
					// $VAR
					start = i + 1;
					vlen = 0;
					while (word[start + vlen] && is_var_char2(word[start
							+ vlen]))
						vlen++;
					if (vlen > 0)
					{
						varname = malloc(vlen + 1);
						memcpy(varname, &word[start], vlen);
						varname[vlen] = '\0';
						/* ここで初めて getenv() を呼ぶ */
						if (val)
						{
							for (int k = 0; val[k]; k++)
								res[rpos++] = val[k];
						}
						free(varname);
						i += (1 + vlen);
					}
					else
					{
						// $だけ
						i++;
					}
				}
			}
			else
			{
				res[rpos++] = word[i];
				res[rpos] = '\0';
				i++;
			}
		}
		else
		{
			/* クォート外 */
			if (word[i] == '\'')
			{
				in_squote = true;
				res[rpos++] = word[i];
				res[rpos] = '\0';
				i++;
			}
			else if (word[i] == '"')
			{
				in_dquote = true;
				res[rpos++] = word[i];
				res[rpos] = '\0';
				i++;
			}
			else if (word[i] == '$')
			{
				// $?
				if (word[i + 1] == '?')
				{
					st_str = itoa_status(last_st);
					for (int k = 0; st_str[k]; k++)
						res[rpos++] = st_str[k];
					free(st_str);
					i += 2;
				}
				else
				{
					// $VAR
					start = i + 1;
					vlen = 0;
					while (word[start + vlen] && is_var_char2(word[start
							+ vlen]))
						vlen++;
					if (vlen > 0)
					{
						varname = malloc(vlen + 1);
						memcpy(varname, &word[start], vlen);
						varname[vlen] = '\0';
						// ★ここで val に getenv(varname) を呼ぶ
						val = getenv(varname);
						if (val)
						{
							// 環境変数があれば 出力バッファにコピー
							for (int k = 0; val[k]; k++)
								res[rpos++] = val[k];
						}
						free(varname);
						i += (1 + vlen);
					}
					else
					{
						// $だけ
						i++;
					}
				}
			}
			else
			{
				res[rpos++] = word[i];
				res[rpos] = '\0';
				i++;
			}
		}
	}
	res[rpos] = '\0';
	return (res);
}

/******************************************************************************
 * (3) Parser + Executor (同じまま)
 * ここは概略; 省略せず貼り付ける必要があれば適宜残す
 ******************************************************************************/

// 以下はあなたの Parser, AST, Executor コードをそのまま or 必要箇所だけ修正したものを貼り付け
//   (省略)

/******************************************************************************
 * (3) Parser (AND/OR/PIPE/SUBSHELL/REDIR) & Executor
 ******************************************************************************/

typedef enum e_asttype
{
	AST_CMD,
	AST_PIPE,
	AST_AND,
	AST_OR,
	AST_SUBSHELL
}					t_asttype2;

typedef enum e_redirtype
{
	REDIR_IN,
	REDIR_HEREDOC,
	REDIR_OUT,
	REDIR_APPEND
}					t_redirtype2;

typedef struct s_redir
{
	t_redirtype2	rtype;
	char			*filename;
	struct s_redir	*next;
}					t_redir2;

typedef struct s_ast
{
	t_asttype2		type;
	struct s_ast	*left;
	struct s_ast	*right;
	struct s_ast *child; // for subshell
	char			**argv;
	t_redir2		*redir;
}					t_ast2;

/* ここでは簡単のため、AST構築時点では raw token を持ちつつ、
 * Executor直前に expand_token() を呼び出す形にすることにします。 */

/* === Parser === */
typedef struct s_parser
{
	t_token			**toks;
	int				pos;
}					t_parser2;

static t_token	*curr_token2(t_parser2 *p)
{
	return (p->toks[p->pos]);
}
static t_token	*next_token2(t_parser2 *p)
{
	return (p->toks[++(p->pos)]);
}
static bool	match_op2(t_parser2 *p, const char *op)
{
	t_token	*tk;

	tk = curr_token2(p);
	return (tk->type == TOK_OP && !my_strcmp(tk->value, op));
}
static bool	match_type2(t_parser2 *p, t_toktype t)
{
	return (curr_token2(p)->type == t);
}

static t_ast2	*new_ast2(t_asttype2 t)
{
	t_ast2	*node;

	node = malloc(sizeof(t_ast2));
	if (!node)
		return (NULL);
	node->type = t;
	node->left = NULL;
	node->right = NULL;
	node->child = NULL;
	node->argv = NULL;
	node->redir = NULL;
	return (node);
}

static void	add_argv2(t_ast2 *cmd, const char *raw_word)
{
	int		count;
	char	**tmp;

	/* 後で expand_token() するので、ここは raw_word をいったん保存 */
	count = 0;
	if (cmd->argv)
	{
		while (cmd->argv[count])
			count++;
	}
	tmp = realloc(cmd->argv, sizeof(char *) * (count + 2));
	if (!tmp)
		return ;
	cmd->argv = tmp;
	cmd->argv[count] = my_strdup(raw_word);
	cmd->argv[count + 1] = NULL;
}

static void	add_redir2(t_ast2 *cmd, t_redirtype2 rtype, const char *fname)
{
	t_redir2	*r;
	t_redir2	*x;

	r = malloc(sizeof(t_redir2));
	r->rtype = rtype;
	r->filename = my_strdup(fname);
	r->next = NULL;
	if (!cmd->redir)
		cmd->redir = r;
	else
	{
		x = cmd->redir;
		while (x->next)
			x = x->next;
		x->next = r;
	}
}

/* parse_redirs() */
static void	parse_redirs2(t_parser2 *p, t_ast2 *node)
{
	const char	*op = curr_token2(p)->value;

	while (true)
	{
		if (!match_type2(p, TOK_OP))
			break ;
		if (!my_strcmp(op, "<"))
		{
			next_token2(p);
			if (match_type2(p, TOK_WORD))
			{
				add_redir2(node, REDIR_IN, curr_token2(p)->value);
				next_token2(p);
			}
			else
			{
				fprintf(stderr, "syntax error after <\n");
				break ;
			}
		}
		else if (!my_strcmp(op, "<<"))
		{
			next_token2(p);
			if (match_type2(p, TOK_WORD))
			{
				add_redir2(node, REDIR_HEREDOC, curr_token2(p)->value);
				next_token2(p);
			}
			else
			{
				fprintf(stderr, "syntax error after <<\n");
				break ;
			}
		}
		else if (!my_strcmp(op, ">"))
		{
			next_token2(p);
			if (match_type2(p, TOK_WORD))
			{
				add_redir2(node, REDIR_OUT, curr_token2(p)->value);
				next_token2(p);
			}
			else
			{
				fprintf(stderr, "syntax error after >\n");
				break ;
			}
		}
		else if (!my_strcmp(op, ">>"))
		{
			next_token2(p);
			if (match_type2(p, TOK_WORD))
			{
				add_redir2(node, REDIR_APPEND, curr_token2(p)->value);
				next_token2(p);
			}
			else
			{
				fprintf(stderr, "syntax error after >>\n");
				break ;
			}
		}
		else
		{
			break ;
		}
	}
}

/* forward */
static t_ast2		*parse_cmdline2(t_parser2 *p);

static t_ast2	*parse_command2(t_parser2 *p)
{
	t_ast2	*sub;
	t_ast2	*node;
	t_ast2	*cmd;

	if (match_type2(p, TOK_LPAREN))
	{
		next_token2(p);
		sub = parse_cmdline2(p);
		if (!match_type2(p, TOK_RPAREN))
		{
			fprintf(stderr, "missing )\n");
		}
		else
		{
			next_token2(p);
		}
		node = new_ast2(AST_SUBSHELL);
		node->child = sub;
		parse_redirs2(p, node);
		return (node);
	}
	else
	{
		cmd = new_ast2(AST_CMD);
		while (match_type2(p, TOK_WORD))
		{
			add_argv2(cmd, curr_token2(p)->value);
			next_token2(p);
		}
		parse_redirs2(p, cmd);
		return (cmd);
	}
}

/* pipeline := command { '|' command } */
static t_ast2	*parse_pipeline2(t_parser2 *p)
{
	t_ast2	*left;
	t_ast2	*right;
	t_ast2	*pipe;

	left = parse_command2(p);
	while (match_op2(p, "|"))
	{
		next_token2(p);
		right = parse_command2(p);
		pipe = new_ast2(AST_PIPE);
		pipe->left = left;
		pipe->right = right;
		left = pipe;
	}
	return (left);
}

/* logical_and := pipeline { '&&' pipeline } */
static t_ast2	*parse_logical_and2(t_parser2 *p)
{
	t_ast2	*left;
	t_ast2	*right;
	t_ast2	*node;

	left = parse_pipeline2(p);
	while (match_op2(p, "&&"))
	{
		next_token2(p);
		right = parse_pipeline2(p);
		node = new_ast2(AST_AND);
		node->left = left;
		node->right = right;
		left = node;
	}
	return (left);
}

/* logical_or := logical_and { '||' logical_and } */
static t_ast2	*parse_logical_or2(t_parser2 *p)
{
	t_ast2	*left;
	t_ast2	*right;
	t_ast2	*node;

	left = parse_logical_and2(p);
	while (match_op2(p, "||"))
	{
		next_token2(p);
		right = parse_logical_and2(p);
		node = new_ast2(AST_OR);
		node->left = left;
		node->right = right;
		left = node;
	}
	return (left);
}

/* cmdline := logical_or */
static t_ast2	*parse_cmdline2(t_parser2 *p)
{
	return (parse_logical_or2(p));
}

/* parser入口 */
static t_ast2	*parse_tokens2(t_token **tokens)
{
	t_parser2	pr;
	t_ast2		*root;

	pr.toks = tokens;
	pr.pos = 0;
	root = parse_cmdline2(&pr);
	if (tokens[pr.pos] && tokens[pr.pos]->type != TOK_EOF)
	{
		fprintf(stderr, "warning: trailing tokens?\n");
	}
	return (root);
}

/******************************************************************************
 * AST解放
 ******************************************************************************/
static void	free_redirs2(t_redir2 *r)
{
	t_redir2	*nx;

	while (r)
	{
		nx = r->next;
		free(r->filename);
		free(r);
		r = nx;
	}
}
static void	free_ast2(t_ast2 *node)
{
	if (!node)
		return ;
	free_ast2(node->left);
	free_ast2(node->right);
	free_ast2(node->child);
	if (node->argv)
	{
		for (int i = 0; node->argv[i]; i++)
			free(node->argv[i]);
		free(node->argv);
	}
	free_redirs2(node->redir);
	free(node);
}

/******************************************************************************
 * Executor:
 *  - 全コマンドを子プロセスで実行 (ビルトイン含む) => exitしても親死なない
 *  - pipe/and/or/subshell
 ******************************************************************************/

typedef struct s_shell
{
	int last_status; // for $?
}					t_shell2;

/* ビルトインcheck */
static bool	is_builtin2(const char *cmd)
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

static int	run_builtin2(char **argv)
{
	int			code;
	const char	*home = getenv("HOME");
	int			i;

	/* exit [n] */
	if (!my_strcmp(argv[0], "exit"))
	{
		code = 0;
		if (argv[1])
			code = atoi(argv[1]);
		_exit(code);
	}
	else if (!my_strcmp(argv[0], "cd"))
	{
		if (!argv[1])
		{
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
	else if (!my_strcmp(argv[0], "echo"))
	{
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
	return (0);
}

static int	apply_redirects2(t_redir2 *r)
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
			if (pipe(pipefd) < 0)
			{
				perror("pipe");
				return (1);
			}
			while (true)
			{
				printf("> ");
				fflush(stdout);
				line = NULL;
				cap = 0;
				n = getline(&line, &cap, stdin);
				if (n <= 0)
				{
					free(line);
					break ;
				}
				if (line[n - 1] == '\n')
					line[n - 1] = '\0';
				if (!my_strcmp(line, r->filename))
				{
					free(line);
					break ;
				}
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

/* run_cmdnode():
 *  1) expand_token() each argv
 *  2) fork
 *    - child: apply_redirect => if builtin => run & exit, else execvp
 */
static int	run_cmdnode(t_ast2 *cmdnode, t_shell2 *sh)
{
	char	*expanded;
	pid_t	pid;
	int		st;
	int		wstatus;

	/* expand argv */
	for (int i = 0; cmdnode->argv && cmdnode->argv[i]; i++)
	{
		expanded = expand_token(cmdnode->argv[i], sh->last_status);
		free(cmdnode->argv[i]);
		cmdnode->argv[i] = expanded;
	}
	if (!cmdnode->argv || !cmdnode->argv[0])
		return (0);
	/* fork+exec */
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		/* child */
		if (apply_redirects2(cmdnode->redir))
			_exit(1);
		/* builtin? */
		if (is_builtin2(cmdnode->argv[0]))
		{
			st = run_builtin2(cmdnode->argv);
			_exit(st);
		}
		else
		{
			execvp(cmdnode->argv[0], cmdnode->argv);
			perror(cmdnode->argv[0]);
			_exit(127);
		}
	}
	else
	{
		wstatus = 0;
		waitpid(pid, &wstatus, 0);
		if (WIFEXITED(wstatus))
			return (WEXITSTATUS(wstatus));
		if (WIFSIGNALED(wstatus))
			return (128 + WTERMSIG(wstatus));
		return (1);
	}
}

static int			executor2(t_ast2 *node, t_shell2 *sh);

static int	exec_pipe2(t_ast2 *node, t_shell2 *sh)
{
	int		pipefd[2];
	pid_t	pid;
	int		st;
	pid_t	pid2;
	int		st2;
	int		st1;

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
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
		st = executor2(node->left, sh);
		_exit(st);
	}
	else
	{
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
			st = executor2(node->right, sh);
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
			return (1);
		}
	}
}

static int	exec_subshell2(t_ast2 *node, t_shell2 *sh)
{
	pid_t	pid;
	int		st;
	int		wstatus;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		if (apply_redirects2(node->redir))
			_exit(1);
		st = executor2(node->child, sh);
		_exit(st);
	}
	else
	{
		wstatus = 0;
		waitpid(pid, &wstatus, 0);
		if (WIFEXITED(wstatus))
			return (WEXITSTATUS(wstatus));
		if (WIFSIGNALED(wstatus))
			return (128 + WTERMSIG(wstatus));
		return (1);
	}
}

/* AND/OR */
static int	exec_and_or2(t_ast2 *node, t_shell2 *sh)
{
	int	left_st;

	left_st = executor2(node->left, sh);
	if (node->type == AST_AND)
	{
		if (left_st == 0)
			return (executor2(node->right, sh));
		else
			return (left_st);
	}
	else
	{
		/* OR */
		if (left_st != 0)
			return (executor2(node->right, sh));
		else
			return (left_st);
	}
}

static int	executor2(t_ast2 *node, t_shell2 *sh)
{
	if (!node)
		return (0);
	switch (node->type)
	{
	case AST_CMD:
		return (run_cmdnode(node, sh));
	case AST_PIPE:
		return (exec_pipe2(node, sh));
	case AST_AND:
	case AST_OR:
		return (exec_and_or2(node, sh));
	case AST_SUBSHELL:
		return (exec_subshell2(node, sh));
	}
	return (0);
}

/******************************************************************************
 * main: readlineループ
 ******************************************************************************/
int	main(void)
{
	t_shell2	shell;
	char		*line;
	int			err;
	t_token		**tokens;
	t_ast2		*root;
	int			st;

	shell.last_status = 0;
	while (true)
	{
		line = readline("mini> ");
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		if (*line)
			add_history(line);
		err = 0;
		tokens = lexer(line, &err);
		free(line);
		if (!tokens || err != 0)
		{
			if (tokens)
				free_tokens(tokens);
			continue ;
		}
		root = parse_tokens2(tokens);
		if (!root)
		{
			free_tokens(tokens);
			continue ;
		}
		st = executor2(root, &shell);
		shell.last_status = st;
		free_ast2(root);
		free_tokens(tokens);
	}
	return (0);
}
