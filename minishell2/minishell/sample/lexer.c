/******************************************************************************
 * lexer_example.c
 *
 * 1つのソースにすべてまとめたサンプル実装です。
 * - token構造体/enumの定義
 * - ユーティリティ関数
 * - lexer本体 (lexer関数)
 * - テスト用 main 関数
 *
 * 注意:
 *   - bashほど厳密ではなく、エスケープ文字(\)等の詳細仕様は省略。
 *   - 未対応ケース(多重 &&&, 誤字演算子, {...}展開など)はあります。
 *   - クォート不一致などでエラー時は lexer 内でステータス2を設定し、
 *     NULLを返す設計にしています。(TOK_ERRORを返す方式でも可)
 ******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* トークン種別を表す列挙型 */
typedef enum e_toktype
{
	TOK_WORD,   // 通常文字列
	TOK_OP,     // 演算子(&&, ||, |, <, <<, >, >> など)
	TOK_LPAREN, // (
	TOK_RPAREN, // )
	TOK_EOF     // 入力の終端
				// TOK_ERROR  // 必要ならエラー用トークンも
}					t_toktype;

/* トークンを表す構造体 */
typedef struct s_token
{
	t_toktype type; // 種別
	char *value;    // 実際の文字列("ls", "&&", "<<", "hello"等)
}					t_token;

/******************************************************************************
 * ユーティリティ関数
 ******************************************************************************/

/* 空白文字かどうか判定する */
static bool	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v'
		|| c == '\f');
}

/* 演算子の可能性がある文字かどうか */
static bool	is_metachar(char c)
{
	// &, |, <, >, (, ) などを対象とする
	return (c == '&' || c == '|' || c == '<' || c == '>' || c == '('
		|| c == ')');
}

/******************************************************************************
 * トークン配列の管理
 ******************************************************************************/

/* 新しいトークンを作成する */
static t_token	*create_token(t_toktype type, const char *value)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->type = type;
	if (value)
		tok->value = strdup(value);
	else
		tok->value = NULL;
	return (tok);
}

/* トークン配列に1つ追加: tokens はNULL終端配列を想定 */
static bool	add_token(t_token ***tokens_ptr, int *count, t_toktype type,
		const char *val)
{
	t_token	**tokens;
	t_token	*newtok;
	t_token	**tmp;

	// tokens_ptr は (t_token **) へのポインタなので、ここでは二重ポインタ参照が必要
	tokens = *tokens_ptr;
	newtok = create_token(type, val);
	if (!newtok)
		return (false);
	// 配列サイズを count+2 に拡張（+1は新トークン分、+1はNULL終端）
	tmp = realloc(tokens, sizeof(t_token *) * (*count + 2));
	if (!tmp)
	{
		free(newtok->value);
		free(newtok);
		return (false);
	}
	tokens = tmp; // 成功したら更新
	tokens[*count] = newtok;
	(*count)++;
	tokens[*count] = NULL; // 終端を常に NULL にしておく
	*tokens_ptr = tokens;  // 呼び出し元の配列ポインタを更新
	return (true);
}

/* トークン配列を開放する */
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

/******************************************************************************
 * read_word():
 *   通常のワードを読み取る
 *   シングルクォート/ダブルクォートを検知し、対応する閉じクォートまで文字をコピー
 *   クォート文字自体は除去する
 *   クォートが閉じられなかった場合は NULLを返す (エラー扱い)
 *
 *   line[pos] が演算子文字 or 空白に到達したら終了とする
 ******************************************************************************/
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
	while (line[*pos] != '\0' && !is_whitespace(line[*pos])
		&& !is_metachar(line[*pos]))
	{
		if (line[*pos] == '\'')
		{
			// シングルクォート => 次の ' までコピー
			(*pos)++; // 開きクォートを飛ばす
			while (line[*pos] != '\0' && line[*pos] != '\'')
			{
				// バッファ拡張チェック
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
				(*pos)++; // 閉じクォート
			else
			{
				// 不一致 => エラー
				free(buf);
				return (NULL);
			}
		}
		else if (line[*pos] == '"')
		{
			// ダブルクォート => 次の " までコピー
			(*pos)++;
			while (line[*pos] != '\0' && line[*pos] != '"')
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
				// 不一致
				free(buf);
				return (NULL);
			}
		}
		else
		{
			// 通常文字として追加
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

/******************************************************************************
 * read_operator():
 *   演算子を読み取る。 (&&, ||, <<, >>, |, <, >, (, ) など)
 *   2文字演算子(&&, ||, >>, <<)を優先チェック
 *   '(' と ')' はそれぞれ専用トークンにする
 *   成功/失敗を bool で返す
 ******************************************************************************/
static bool	read_operator(const char *line, int *pos, t_token ***tokens_ptr,
		int *tcount)
{
	char	c;
	char	d;
	char	op[2] = {c, '\0'};

	c = line[*pos];
	d = line[*pos + 1];
	// 括弧は1文字トークン
	if (c == '(')
	{
		(*pos)++;
		return (add_token(tokens_ptr, tcount, TOK_LPAREN, "("));
	}
	if (c == ')')
	{
		(*pos)++;
		return (add_token(tokens_ptr, tcount, TOK_RPAREN, ")"));
	}
	// 2文字演算子を優先チェック
	if (c == '&' && d == '&') // &&
	{
		(*pos) += 2;
		return (add_token(tokens_ptr, tcount, TOK_OP, "&&"));
	}
	if (c == '|' && d == '|') // ||
	{
		(*pos) += 2;
		return (add_token(tokens_ptr, tcount, TOK_OP, "||"));
	}
	if (c == '<' && d == '<') // <<
	{
		(*pos) += 2;
		return (add_token(tokens_ptr, tcount, TOK_OP, "<<"));
	}
	if (c == '>' && d == '>') // >>
	{
		(*pos) += 2;
		return (add_token(tokens_ptr, tcount, TOK_OP, ">>"));
	}
	// 1文字演算子 (<, >, | など)
	if (c == '<' || c == '>' || c == '|')
	{
		(*pos)++;
		return (add_token(tokens_ptr, tcount, TOK_OP, op));
	}
	// 万が一ここまで来たら、未サポートの演算子
	return (false);
}

/******************************************************************************
 * lexer():
 *   入力文字列をトークン列に分割するメイン関数
 *   - 空白をスキップ
 *   - 演算子文字(&,|,<,>,(,))に遭遇したら read_operator()
 *   - 通常文字は read_word() でクォートを処理
 *   - クォート不一致等のエラーがあれば *exit_status = 2 をセットして NULL返却
 *   - 正常ならトークン配列(終端NULL)を返す
 ******************************************************************************/
static t_token	**lexer(const char *line, int *exit_status)
{
	t_token	**tokens;
	int		tcount;
	int		pos;
	int		len;
	char	*w;

	// トークン配列 (動的に増やしていく)
	tokens = malloc(sizeof(t_token *));
	if (!tokens)
		return (NULL);
	tokens[0] = NULL;
	tcount = 0;
	pos = 0;
	len = strlen(line);
	*exit_status = 0; // 初期値(エラーがあれば2にする)
	while (pos < len)
	{
		// 空白スキップ
		if (is_whitespace(line[pos]))
		{
			pos++;
			continue ;
		}
		// 演算子文字なら read_operator()
		if (is_metachar(line[pos]))
		{
			if (!read_operator(line, &pos, &tokens, &tcount))
			{
				// 想定外の演算子等 => シンタックスエラー扱い
				fprintf(stderr, "syntax error near unexpected operator\n");
				free_tokens(tokens);
				*exit_status = 2;
				return (NULL);
			}
		}
		else
		{
			// 通常ワードを読む
			w = read_word(line, &pos);
			if (!w)
			{
				// クォート不一致など
				fprintf(stderr, "syntax error: unclosed quote\n");
				free_tokens(tokens);
				*exit_status = 2;
				return (NULL);
			}
			if (!add_token(&tokens, &tcount, TOK_WORD, w))
			{
				free(w);
				free_tokens(tokens);
				*exit_status = 2;
				return (NULL);
			}
			free(w);
		}
	}
	// 入力終端 => EOFトークンを追加して終了(なくても良いが一応)
	add_token(&tokens, &tcount, TOK_EOF, "");
	return (tokens);
}

/******************************************************************************
 * デバッグ用: トークン種別を文字列で返す
 ******************************************************************************/
static const char	*toktype_name(t_toktype t)
{
	switch (t)
	{
	case TOK_WORD:
		return ("WORD");
	case TOK_OP:
		return ("OP");
	case TOK_LPAREN:
		return ("LPAREN");
	case TOK_RPAREN:
		return ("RPAREN");
	case TOK_EOF:
		return ("EOF");
	// case TOK_ERROR:  return ("ERROR");
	default:
		return ("UNKNOWN");
	}
}

/******************************************************************************
 * main():
 *   テスト用に、コマンドライン引数として入力行を受け取り、
 *   lexer() でトークンに分解して表示する。
 ******************************************************************************/
int	main(int argc, char **argv)
{
	const char	*input = argv[1];
	int			status;
	t_token		**tokens;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s \"command line\"\n", argv[0]);
		return 1;
	}
	// 入力文字列は argv[1] と仮定
	status = 0;
	tokens = lexer(input, &status);
	if (!tokens)
	{
		// lexer 内でエラーがあった場合
		fprintf(stderr, "Lexer error. status=%d\n", status);
		return status;
	}
	// トークン列を表示
	printf("Lexer result (status=%d):\n", status);
	for (int i = 0; tokens[i]; i++)
	{
		printf("  [%s] \"%s\"\n", toktype_name(tokens[i]->type),
			tokens[i]->value);
	}
	// 後始末
	free_tokens(tokens);
	return status;
}
