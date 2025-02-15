#include "../includes/minishell.h"
#include "../libft/libft.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * expand_dollar:
 *   ダブルクォート内またはクォート外での $ 展開を行う。
 *   シングルクォート内の場合は呼ばれない前提。
 */
static void	expand_dollar(char **res, size_t *j, size_t *cap, const char *token,
		int *i, t_shell *shell)
{
	char	*num;
	int		start;
	int		varlen;
	char	*varname;
	char	*val;

	(*i)++; // '$' を消費
	/* $? の場合 */
	if (token[*i] == '?')
	{
		num = ft_itoa(shell->last_exit_status);
		for (int k = 0; num[k]; k++)
			append_char(res, j, cap, num[k]);
		free(num);
		(*i)++; // '?' を消費
		return ;
	}
	/* $VAR の場合：先頭が英字または '_' であるとする */
	if (ft_isalpha_(token[*i]) || token[*i] == '_')
	{
		start = *i;
		while (ft_isalnum_(token[*i]) || token[*i] == '_')
			(*i)++;
		varlen = *i - start;
		varname = ft_strndup(&token[start], varlen);
		val = envlist_get_value(shell->envlist, varname);
		if (!val)
			val = "";
		for (int k = 0; val[k]; k++)
			append_char(res, j, cap, val[k]);
		free(varname);
		return ;
	}
	else
	{
		/* 変数名にならない場合は '$' をそのまま出力 */
		append_char(res, j, cap, '$');
	}
}

/**
 * expand_and_remove_quotes:
 *   入力 token を走査し、bash 仕様に従ってクォート除去および変数展開を行う。
 *
 *   - シングルクォート内では、$ は展開せずリテラルとして出力する
 *   - ダブルクォート内およびクォート外では $ 展開を行う
 *   - 外側の引用符（' や "）は出力に含めない
 *   - 内部に現れる引用符（例：ダブルクォート内の '）はそのまま出力する
 */
char	*expand_and_remove_quotes(const char *token, t_shell *shell)
{
	if (!token)
		return (NULL);

	size_t cap = 128;
	char *res = malloc(cap);
	if (!res)
		return (NULL);
	res[0] = '\0';
	size_t j = 0;

	bool in_single = false;
	bool in_double = false;
	int i = 0;
	while (token[i])
	{
		char c = token[i];
		/* シングルクォートの開始／終了（ダブルクォート中は無視） */
		if (c == '\'' && !in_double)
		{
			in_single = !in_single;
			i++; // クォート自体は出力しない
			continue ;
		}
		/* ダブルクォートの開始／終了（シングルクォート中は無視） */
		if (c == '"' && !in_single)
		{
			in_double = !in_double;
			i++; // クォート自体は出力しない
			continue ;
		}
		/* '$' の処理：シングルクォート内ならリテラルとして出力 */
		if (c == '$')
		{
			if (in_single)
			{
				append_char(&res, &j, &cap, '$');
				i++;
			}
			else
			{
				expand_dollar(&res, &j, &cap, token, &i, shell);
			}
			continue ;
		}
		/* その他の文字はそのまま出力 */
		append_char(&res, &j, &cap, c);
		i++;
	}
	res[j] = '\0';
	return (res);
}