/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doller.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:41:25 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/04 19:48:48 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

// 環境変数の値を取得する（= の後の部分を取得）
char	*get_env_value(char *env)
{
	while (*env && *env != '=')
		env++;
	if (*env == '=')
		env++;
	return (env);
}

// 置換後の新しい文字列の長さを計算する
int	calculate_new_str_length(char *env, char *str, int index)
{
	int		env_value_len;
	int		after_dollar_len;
	char	*env_value;

	env_value_len = 0;
	after_dollar_len = 0;
	env_value = get_env_value(env);
	// 環境変数の値の長さを計算
	while (env_value[env_value_len])
		env_value_len++;
	// `$VAR_NAME` の後の文字列の長さを計算
	while (str[index + 1 + ft_strlen_var(&str[index + 1]) + after_dollar_len])
		after_dollar_len++;
	// `$` の前の部分の長さ + 環境変数の値の長さ + 残りの文字列の長さ + NULL終端
	return (index + env_value_len + after_dollar_len + 1);
}

// `$VAR_NAME` を環境変数の値に置き換えた新しい文字列を作成
char	*replace_env_variable(char *env, char *str, int index)
{
	int		i;
	char	*ret;
	int		new_str_len;

	i = 0;
	new_str_len = calculate_new_str_length(env, str, index);
	ret = (char *)malloc(new_str_len);
	if (!ret)
		return (NULL);
	while (i < index)
	{
		ret[i] = str[i];
		i++;
	}
	ret[i] = '\0';
	ft_strlcat(ret, get_env_value(env), new_str_len + index + 2);
	ft_strlcat(ret, &str[index + 1 + ft_strlen_var(&str[index + 1])],
		new_str_len + index + 2);
	return (ret);
}

char	*ft_expand_daller(char **env, char *str, int index)
{
	int		i;
	char	*ret;

	i = 0;
	while (env[i])
	{
		if (!ft_strncmp(env[i], &str[index + 1], ft_strlen_var(&str[index + 1]))
			&& env[i][ft_strlen_var(&str[index + 1])] == '=')
		{
			ret = replace_env_variable(env[i], str, index);
			free(str);
			return (ret);
		}
		i++;
	}
	return (NULL);
}

/*
void	input_daller(char **str, char **env)
{
	while (*str)
	{
		if (*str[0] == '$')
			*str = ft_expand_daller(*str, env);
		str++;
	}
}
*/