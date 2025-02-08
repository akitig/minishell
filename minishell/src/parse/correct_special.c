/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   correct_special.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 13:49:18 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/08 17:41:50 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

// 特殊文字の修正に必要な新しい長さを計算
static int	ft_correct_len(char **str, char *spec)
{
	int	len;

	len = 1;
	if (!*str)
		return (0);
	while (str[0] && str[1])
	{
		if (ft_strchr(spec, str[0][0]) && (str[0][0] == str[1][0])
			&& ft_strlen(str[0]) == 1 && ft_strlen(str[1]) == 1)
		{
			str++;
		}
		len++;
		str++;
	}
	return (len);
}

// 2つの文字が並んでいる場合 (`<<`, `>>` など) の処理
char	*ft_fill_wrd(char *str)
{
	char	*n_wrd;

	n_wrd = malloc(3);
	if (!n_wrd)
		return (NULL);
	n_wrd[0] = str[0];
	n_wrd[1] = str[0];
	n_wrd[2] = '\0';
	return (n_wrd);
}

// 配列に正しい修正を適用
static char	**ft_correct_fill(char **res, char **str, char *spec)
{
	int		len;
	char	*n_wrd;

	len = 0;
	if (!*str)
		return (0);
	while (str[0] && str[1])
	{
		if (ft_strchr(spec, str[0][0]) && (str[0][0] == str[1][0])
			&& ft_strlen(str[0]) == 1 && ft_strlen(str[1]) == 1)
		{
			n_wrd = ft_fill_wrd(str[0]);
			free(str[0]);
			free(str[1]);
			res[len++] = n_wrd;
			str++;
		}
		else
			res[len++] = *str;
		str++;
	}
	if (str)
		res[len++] = *str;
	res[len] = NULL;
	return (res);
}

// 特殊文字 (`<`, `>`, `|`) の修正を行う
char	**ft_correct_special(char **str, char *spec)
{
	int		n_len;
	char	**res;

	n_len = ft_correct_len(str, spec);
	res = malloc(sizeof(char *) * (n_len + 1));
	if (!res)
		return (NULL);
	res = ft_correct_fill(res, str, spec);
	if (!res)
		return (str);
	free(str);
	return (res);
}
