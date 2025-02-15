/******************************************************************************
 * mini_parser.c
 *
 * 簡易的に複合コマンド( && / || / | / ( ) ) とリダイレクトを扱うASTパーサ例。
 *
 * 構成:
 *  1) Lexer   : lexer() / read_word() / read_operator()
 *  2) Parser  : 再帰下降パーサ
 *              parse_cmdline() -> parse_logical_or() -> parse_logical_and()
 *              -> parse_pipeline() -> parse_command()
 *  3) AST表示: dump_ast()
 *
 * 対応構文 (簡易版):
 *   cmdline            := logical_or
 *   logical_or         := logical_and { "||" logical_and }
 *   logical_and        := pipeline { "&&" pipeline }
 *   pipeline           := command { "|" command }
 *   command            := ( WORD... [redirection...] )
 *                       | "(" cmdline ")" [redirection...]
 *                       (ここでは "コマンド or サブシェル" + 後ろに複数リダイレクトが付くイメージ)
 *   redirection        := "<" WORD | "<<" WORD | ">" WORD | ">>" WORD
 *
 *   WORD(トークン)はクォートを除去済みの通常文字列。
 *   Bashほど厳密ではありません。セミコロン(;)や複雑な構文は未サポート。
 *
 * コンパイル:
 *   gcc -Wall -Wextra -Werror mini_parser.c -o mini_parser
 *
 * 実行例:
 *   ./mini_parser "echo hello && (cd /tmp || echo fail) | grep something"
 *
 *   => Lexer結果, Parser結果(AST)がコンソールにダンプされる。
 *
 * 注意:
 *   - $? や $VAR などの変数展開は行いません(必要なら後段or Lexerで実装)。
 *   - エスケープ文字(\)は未対応。
 *   - '(' や ')' の個数不一致など、エラー時にやや雑に処理している部分があります。
 *   - Executor(コマンド実行)は未実装。ASTをどう実行するかは各自で拡張してください。
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * (1) Lexer: トークン種別と構造体
 ******************************************************************************/
typedef enum e_toktype
{
	TOK_WORD,   // 通常文字列
	TOK_OP,     // 演算子(&&, ||, |, <, <<, >, >> など)
	TOK_LPAREN, // (
	TOK_RPAREN, // )
	TOK_EOF
}					t_toktype;

typedef struct s_token
{
	t_toktype		type;
	char			*value;
}					t_token;

/******************************************************************************
 * (1-A) Lexer用関数
 ******************************************************************************/

/* トークンを配列に追加 (tokensがNULL終端になるように) */
static bool	add_token(t_token ***ptokens, int *count, t_toktype type,
		const char *val)
{
	t_token	**tokens;
	t_token	*newt;
	t_token	**tmp;

	tokens = *ptokens;
	newt = malloc(sizeof(t_token));
	if (!newt)
		return (false);
	newt->type = type;
	newt->value = val ? my_strdup(val) : NULL;
	tmp = realloc(tokens, sizeof(t_token *) * (*count + 2));
	if (!tmp)
	{
		free(newt->value);
		free(newt);
		return (false);
	}
	tokens = tmp;
	tokens[*count] = newt;
	(*count)++;
	tokens[*count] = NULL;
	*ptokens = tokens;
	return (true);
}

/* トークン配列解放 */
static void	free_tokens(t_token **tokens)
{
	if (!tokens)
		return ;
	for (int i = 0; tokens[i]; i++)
	{
		free(tokens[i]->value);
		free(tokens[i]);
	}
	free(tokens);
}

/* 演算子の先頭になりうる文字 */
static bool	is_metachar(char c)
{
	return (c == '<' || c == '>' || c == '|' || c == '&' || c == '('
		|| c == ')');
}

/* 2文字演算子を優先チェック */
static bool	read_operator(const char *line, int *pos, t_token ***ptoks,
		int *count)
{
	char	c;
	char	d;
	char	op[2] = {c, '\0'};

	c = line[*pos];
	d = line[*pos + 1];
	/* ( ) */
	if (c == '(')
	{
		(*pos)++;
		return (add_token(ptoks, count, TOK_LPAREN, "("));
	}
	if (c == ')')
	{
		(*pos)++;
		return (add_token(ptoks, count, TOK_RPAREN, ")"));
	}
	/* 2文字 */
	if (c == '<' && d == '<')
	{
		(*pos) += 2;
		return (add_token(ptoks, count, TOK_OP, "<<"));
	}
	if (c == '>' && d == '>')
	{
		(*pos) += 2;
		return (add_token(ptoks, count, TOK_OP, ">>"));
	}
	if (c == '&' && d == '&')
	{
		(*pos) += 2;
		return (add_token(ptoks, count, TOK_OP, "&&"));
	}
	if (c == '|' && d == '|')
	{
		(*pos) += 2;
		return (add_token(ptoks, count, TOK_OP, "||"));
	}
	/* 1文字 */
	if (c == '<' || c == '>' || c == '|')
	{
		(*pos)++;
		return (add_token(ptoks, count, TOK_OP, op));
	}
	return (false); // 想定外演算子
}

/* クォート除去しながら1ワード読み取り */
static char	*read_word(const char *line, int *pos)
{
	int		capacity;
	char	*buf;
	int		len;
	char	*tmp;

	capacity = 128;
	buf = malloc(capacity);
	if (!buf)
		return (NULL);
	len = 0;
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
			{ // 不一致
				free(buf);
				return (NULL);
			}
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
			// 通常文字
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

/*
 * lexer本体
 */
static t_token	**lexer(const char *line, int *err)
{
	t_token	**tokens;
	int		tcount;
	int		pos;
	int		length;
	char	*w;

	*err = 0;
	tokens = malloc(sizeof(t_token *));
	if (!tokens)
		return (NULL);
	tokens[0] = NULL;
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
			if (!read_operator(line, &pos, &tokens, &tcount))
			{
				fprintf(stderr, "[Lexer] unknown operator near '%c'\n",
					line[pos]);
				*err = 2; // syntax error
				free_tokens(tokens);
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
				free_tokens(tokens);
				return (NULL);
			}
			if (!add_token(&tokens, &tcount, TOK_WORD, w))
			{
				free(w);
				free_tokens(tokens);
				*err = 2;
				return (NULL);
			}
			free(w);
		}
	}
	// EOF
	add_token(&tokens, &tcount, TOK_EOF, "");
	return (tokens);
}

/******************************************************************************
 * (2) AST定義
 ******************************************************************************/
typedef enum e_asttype
{
	AST_CMD,      // 単純コマンド(引数+リダイレクト)
	AST_PIPE,     // left | right
	AST_AND,      // left && right
	AST_OR,       // left || right
	AST_SUBSHELL, // ( cmdline ) + リダイレクト
}					t_asttype;

/* リダイレクトの種類 */
typedef enum e_redirtype
{
	REDIR_IN,      // <
	REDIR_HEREDOC, // <<
	REDIR_OUT,     // >
	REDIR_APPEND   // >>
}					t_redirtype;

typedef struct s_redir
{
	t_redirtype		rtype;
	char *filename; // or delimiter
	struct s_redir	*next;
}					t_redir;

/* ASTノード本体 */
typedef struct s_ast
{
	t_asttype		type;

	/* サブノード (AST_PIPE/AST_AND/AST_ORで使用) */
	struct s_ast	*left;
	struct s_ast	*right;

	/* サブシェル(AST_SUBSHELL)の場合、内部にさらにASTが入る */
	struct s_ast *child; // ( ... )

	/* コマンド(AST_CMD)の場合: args[] リスト */
	char			**argv;

	/* リダイレクトリスト: <file, <<word, >file, >>file 等 */
	t_redir			*redir;

}					t_ast;

/******************************************************************************
 * ASTの生成ヘルパ
 ******************************************************************************/
static t_ast	*new_ast(t_asttype t)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
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

/* argv に1つ追加 */
static void	add_argv(t_ast *cmdnode, const char *word)
{
	int		count;
	char	**tmp;

	if (!cmdnode || cmdnode->type != AST_CMD)
		return ;
	count = 0;
	if (cmdnode->argv)
	{
		while (cmdnode->argv[count])
			count++;
	}
	tmp = realloc(cmdnode->argv, sizeof(char *) * (count + 2));
	if (!tmp)
		return ;
	cmdnode->argv = tmp;
	cmdnode->argv[count] = my_strdup(word);
	cmdnode->argv[count + 1] = NULL;
}

/* リダイレクトをcmdnode->redirに追加 */
static void	add_redir(t_ast *cmdnode, t_redirtype rt, const char *filename)
{
	t_redir	*r;
	t_redir	*p;

	if (!cmdnode)
		return ;
	r = malloc(sizeof(t_redir));
	if (!r)
		return ;
	r->rtype = rt;
	r->filename = my_strdup(filename);
	r->next = NULL;
	if (!cmdnode->redir)
	{
		cmdnode->redir = r;
	}
	else
	{
		p = cmdnode->redir;
		while (p->next)
			p = p->next;
		p->next = r;
	}
}

/******************************************************************************
 * (2-B) パーサ: トークン列を読み取り、再帰下降でASTを生成
 *
 *  grammar(簡易):
 *   cmdline        := logical_or
 *   logical_or     := logical_and { "||" logical_and }
 *   logical_and    := pipeline { "&&" pipeline }
 *   pipeline       := command { "|" command }
 *   command        := ( WORD... [ redir... ] )
 *                   | "(" cmdline ")" [redir...]
 *   redir          := "<" WORD | "<<" WORD | ">" WORD | ">>" WORD
 *
 *  (ただし厳密なエラー処理・bash的細かい規則は省略)
 ******************************************************************************/
typedef struct s_parser
{
	t_token			**tokens;
	int				pos;
}					t_parser;

static t_token	*curr_token(t_parser *p)
{
	return (p->tokens[p->pos]);
}
static t_token	*next_token(t_parser *p)
{
	return (p->tokens[++(p->pos)]);
}
static bool	match_op(t_parser *p, const char *op)
{
	t_token	*tk;

	tk = curr_token(p);
	return (tk->type == TOK_OP && !my_strcmp(tk->value, op));
}
static bool	match_type(t_parser *p, t_toktype t)
{
	t_token	*tk;

	tk = curr_token(p);
	return (tk->type == t);
}

/* forward宣言 */
static t_ast		*parse_cmdline(t_parser *p);

/* redirection部を読み取る: <file, <<word, >file, >>file 連続可 */
static void	parse_redirs(t_parser *p, t_ast *cmdnode)
{
	char	*op;

	while (curr_token(p)->type == TOK_OP)
	{
		op = curr_token(p)->value;
		if (!my_strcmp(op, "<"))
		{
			next_token(p);
			if (match_type(p, TOK_WORD))
			{
				add_redir(cmdnode, REDIR_IN, curr_token(p)->value);
				next_token(p);
			}
			else
			{
				fprintf(stderr, "syntax error: expected filename after <\n");
				return ;
			}
		}
		else if (!my_strcmp(op, "<<"))
		{
			next_token(p);
			if (match_type(p, TOK_WORD))
			{
				add_redir(cmdnode, REDIR_HEREDOC, curr_token(p)->value);
				next_token(p);
			}
			else
			{
				fprintf(stderr, "syntax error: expected delimiter after <<\n");
				return ;
			}
		}
		else if (!my_strcmp(op, ">"))
		{
			next_token(p);
			if (match_type(p, TOK_WORD))
			{
				add_redir(cmdnode, REDIR_OUT, curr_token(p)->value);
				next_token(p);
			}
			else
			{
				fprintf(stderr, "syntax error: expected filename after >\n");
				return ;
			}
		}
		else if (!my_strcmp(op, ">>"))
		{
			next_token(p);
			if (match_type(p, TOK_WORD))
			{
				add_redir(cmdnode, REDIR_APPEND, curr_token(p)->value);
				next_token(p);
			}
			else
			{
				fprintf(stderr, "syntax error: expected filename after >>\n");
				return ;
			}
		}
		else
		{
			// リダイレクトでなければbreak
			break ;
		}
	}
}

/* command := ( WORD... [redir...] ) | ( '(' cmdline ')' [redir...] ) */
static t_ast	*parse_command(t_parser *p)
{
	t_ast	*subnode;
	t_ast	*node;

	if (match_type(p, TOK_LPAREN))
	{
		/* サブシェル */
		next_token(p); // consume '('
		subnode = parse_cmdline(p);
		if (!match_type(p, TOK_RPAREN))
		{
			fprintf(stderr, "syntax error: missing ')'\n");
			return subnode; // not robust
		}
		next_token(p); // consume ')'
		// さらに後ろにリダイレクト付く可能性
		node = new_ast(AST_SUBSHELL);
		node->child = subnode;
		parse_redirs(p, node);
		return node;
	}
	else
	{
		/* 単純コマンド */
		node = new_ast(AST_CMD);
		// まず WORDをなるだけ読む
		while (match_type(p, TOK_WORD))
		{
			add_argv(node, curr_token(p)->value);
			next_token(p);
		}
		// リダイレクトが連続するかもしれない
		parse_redirs(p, node);
		return node;
	}
}

/* pipeline := command { '|' command } */
static t_ast	*parse_pipeline(t_parser *p)
{
	t_ast	*left;
	t_ast	*right;
	t_ast	*pipe;

	left = parse_command(p);
	while (match_op(p, "|"))
	{
		next_token(p); // consume '|'
		right = parse_command(p);
		pipe = new_ast(AST_PIPE);
		pipe->left = left;
		pipe->right = right;
		left = pipe; // 連続パイプを左結合に
	}
	return left;
}

/* logical_and := pipeline { '&&' pipeline } */
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
	return left;
}

/* logical_or := logical_and { '||' logical_and } */
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
	return left;
}

/* cmdline := logical_or (本当は ';' や改行などもあるが省略) */
static t_ast	*parse_cmdline(t_parser *p)
{
	t_ast	*node;

	node = parse_logical_or(p);
	return node;
}

/******************************************************************************
 * パーサの入り口
 ******************************************************************************/
static t_ast	*parse_tokens(t_token **tokens)
{
	t_parser	parser;
	t_ast		*root;

	parser.tokens = tokens;
	parser.pos = 0;
	root = parse_cmdline(&parser);
	/* 一応、TOK_EOFを確認 */
	if (tokens[parser.pos] && tokens[parser.pos]->type != TOK_EOF)
	{
		fprintf(stderr, "syntax warning: trailing tokens?\n");
	}
	return root;
}

/******************************************************************************
 * ASTのダンプ (デバッグ用)
 ******************************************************************************/
static void	dump_redirs(const t_redir *r)
{
	const char	*tt = "??";

	for (; r; r = r->next)
	{
		if (r->rtype == REDIR_IN)
			tt = "<";
		else if (r->rtype == REDIR_HEREDOC)
			tt = "<<";
		else if (r->rtype == REDIR_OUT)
			tt = ">";
		else if (r->rtype == REDIR_APPEND)
			tt = ">>";
		printf(" %s %s", tt, r->filename);
	}
}
static void	dump_ast(t_ast *node, int indent)
{
	if (!node)
		return ;
	for (int i = 0; i < indent; i++)
		printf("  ");
	switch (node->type)
	{
	case AST_CMD:
		printf("CMD:");
		if (node->argv)
		{
			for (int i = 0; node->argv[i]; i++)
				printf(" [%s]", node->argv[i]);
		}
		dump_redirs(node->redir);
		printf("\n");
		break ;
	case AST_SUBSHELL:
		printf("SUBSHELL:");
		dump_redirs(node->redir);
		printf("\n");
		dump_ast(node->child, indent + 1);
		break ;
	case AST_PIPE:
		printf("PIPE:\n");
		dump_ast(node->left, indent + 1);
		dump_ast(node->right, indent + 1);
		break ;
	case AST_AND:
		printf("AND:\n");
		dump_ast(node->left, indent + 1);
		dump_ast(node->right, indent + 1);
		break ;
	case AST_OR:
		printf("OR:\n");
		dump_ast(node->left, indent + 1);
		dump_ast(node->right, indent + 1);
		break ;
	}
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
 * mainテスト
 ******************************************************************************/
int	main(int argc, char **argv)
{
	const char	*input = argv[1];
	int			err;
	t_token		**tokens;
	const char	*tname = "UNKNOWN";
	t_ast		*root;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s \"command line\"\n", argv[0]);
		return 1;
	}
	printf("Input: %s\n", input);
	err = 0;
	tokens = lexer(input, &err);
	if (!tokens)
	{
		fprintf(stderr, "Lexer error.\n");
		return err ? err : 1;
	}
	printf("\n--- Tokens ---\n");
	for (int i = 0; tokens[i]; i++)
	{
		if (tokens[i]->type == TOK_WORD)
			tname = "WORD";
		else if (tokens[i]->type == TOK_OP)
			tname = "OP";
		else if (tokens[i]->type == TOK_LPAREN)
			tname = "LPAREN";
		else if (tokens[i]->type == TOK_RPAREN)
			tname = "RPAREN";
		else if (tokens[i]->type == TOK_EOF)
			tname = "EOF";
		printf("[%d] %s('%s')\n", i, tname, tokens[i]->value);
	}
	if (err != 0)
	{
		free_tokens(tokens);
		return err;
	}
	printf("\n--- Parse to AST ---\n");
	root = parse_tokens(tokens);
	/* ダンプ */
	dump_ast(root, 0);
	/* 後始末 */
	free_ast(root);
	free_tokens(tokens);
	return 0;
}
