/* lexer.c */

#include "../includes/minishell.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ダミー実装例:
   ここでは "空白" の判定のみする */
bool	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

/* ダミー実装例:
   ここでは '|', '<', '>' 等を "メタ文字" として処理 */
bool	is_metachar(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '(' || c == ')' || c == '&'
		|| c == ';');
}

/*
** add_token():
**   t_token** を再確保して末尾に追加
*/
bool	add_token(t_token ***tokens, int *count, t_toktype type,
		const char *val)
{
	t_token	**tmp;
	t_token	*newtok;

	tmp = realloc(*tokens, sizeof(t_token *) * (*count + 2));
	if (!tmp)
		return (false);
	*tokens = tmp;
	newtok = malloc(sizeof(t_token));
	if (!newtok)
		return (false);
	newtok->type = type;
	if (val)
		newtok->value = ft_strdup2(val);
	else
		newtok->value = NULL;
	(*tokens)[*count] = newtok;
	(*count)++;
	(*tokens)[*count] = NULL;
	return (true);
}

/*
** free_tokens():
**   t_token** 配列を解放
*/
void	free_tokens(t_token **tokens)
{
	int	i;

	if (!tokens)
		return ;
	i = 0;
	while (tokens[i])
	{
		free(tokens[i]->value);
		free(tokens[i]);
		i++;
	}
	free(tokens);
}

/*
** 実際の lexer (使わないなら削除orコメントアウト)
*/
t_token	**lexer(const char *line, int *exit_status)
{
	(void)exit_status;
	(void)line;
	// 例: 空実装
	// 実際には read_operator() や read_word() を駆使して tokens を作る

	// とりあえず ダミーで TOK_EOF 1つだけ返す
	t_token **tokens = malloc(sizeof(t_token *) * 2);
	tokens[0] = malloc(sizeof(t_token));
	tokens[0]->type = TOK_EOF;
	tokens[0]->value = ft_strdup2("");
	tokens[1] = NULL;
	return (tokens);
}