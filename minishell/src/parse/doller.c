/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doller.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:41:25 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/08 17:42:32 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

// 環境変数の値を取得する
char	*get_env_value(char **env, const char *key)
{
	int		i;
	size_t	key_len;

	i = 0;
	key_len = ft_strlen(key);
	while (env[i])
	{
		if (ft_strncmp(env[i], key, key_len) == 0 && env[i][key_len] == '=')
			return (ft_strdup(env[i] + key_len + 1)); // '=' の後の値を返す
		i++;
	}
	return (ft_strdup("")); // 存在しない場合は空文字を返す
}

// クォート処理: `'` の場合は `$` を展開しない
char	*ft_handle_quotes(char *str)
{
	char	*new_str;
	char	quote;

	int i, j;
	i = 0, j;
	new_str = malloc(ft_strlen(str) + 1);
	if (!new_str)
		return (NULL);
	i = 0, j = 0;
	quote = '\0';
	while (str[i])
	{
		printf("[DEBUG] i: %d, str[i]: %c, quote: %c\n", i, str[i],
			quote ? quote : '-');
		// クォートの処理
		if ((str[i] == '"' || str[i] == '\''))
		{
			if (quote == '\0')
				quote = str[i]; // クォート開始
			else if (quote == str[i])
				quote = '\0'; // クォート終了
			i++;              // `i` を進めることで無限ループを防ぐ
		}
		else
		{
			new_str[j++] = str[i++];
		}
	}
	new_str[j] = '\0';
	return (new_str);
}

// `$VAR` を展開する処理
char	*ft_expand_daller(char **env, char *str)
{
	char	*new_str;
	char	quote;
	int		var_start;
	char	*var_name;
	char	*var_value;

	int i, j;
	i = 0, j;
	new_str = malloc(ft_strlen(str) * 2);
	if (!new_str)
		return (NULL);
	i = 0, j = 0;
	quote = '\0';
	while (str[i])
	{
		printf("[DEBUG] i: %d, str[i]: %c, quote: %c\n", i, str[i],
			quote ? quote : '-');
		// クォートの処理
		if ((str[i] == '"' || str[i] == '\''))
		{
			if (quote == '\0')
				quote = str[i]; // クォート開始
			else if (quote == str[i])
				quote = '\0';        // クォート終了
			new_str[j++] = str[i++]; // クォート記号をそのままコピー
		}
		// `$` の処理（シングルクォートの中では展開しない）
		else if (str[i] == '$' && quote != '\'' && (ft_isalnum(str[i + 1])
			|| str[i + 1] == '_'))
		{
			var_start = ++i;
			while (ft_isalnum(str[i]) || str[i] == '_')
				i++;
			var_name = ft_strndup(&str[var_start], i - var_start);
			var_value = get_env_value(env, var_name);
			free(var_name);
			strcpy(&new_str[j], var_value);
			j += ft_strlen(var_value);
			free(var_value);
		}
		else
		{
			new_str[j++] = str[i++];
		}
	}
	new_str[j] = '\0';
	return (new_str);
}

char	*ft_join_tokens(char **tokens)
{
	int		len;
	char	*joined;

	if (!tokens || !tokens[0])
		return (NULL);
	len = 0;
	for (int i = 0; tokens[i]; i++)
		len += ft_strlen(tokens[i]) + 1; // スペースの分も考慮
	joined = malloc(len + 1);
	if (!joined)
		return (NULL);
	joined[0] = '\0';
	for (int i = 0; tokens[i]; i++)
	{
		ft_strcat(joined, tokens[i]);
		if (tokens[i + 1])
			ft_strcat(joined, " ");
	}
	return (joined);
}

// `$` を含む文字列を解析し、環境変数を展開しつつ、クォートを処理する
void	input_daller(char **str, char **env)
{
	char	*expanded;
	char	*cleaned;
	char	*final_str;

	if (!str || !(*str))
		return ;
	printf("[DEBUG] Before Expansion: %s\n", *str);
	// 環境変数展開
	expanded = ft_expand_daller(env, *str);
	printf("[DEBUG] After Expansion: %s\n", expanded);
	// クォート処理
	cleaned = ft_handle_quotes(expanded);
	printf("[DEBUG] After Quotes Handling: %s\n", cleaned);
	free(expanded);
	// 特殊文字の処理
	char **tokens = ft_split_str(cleaned, ' '); // 文字列をトークンに分割
	tokens = ft_correct_special(tokens, "<>|"); // 特殊文字の修正
	// `tokens` を再構築
	final_str = ft_join_tokens(tokens);
	free(*str);
	*str = final_str;
	printf("[DEBUG] After Special Handling: %s\n", *str);
}
