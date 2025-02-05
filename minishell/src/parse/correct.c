/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   correct.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 20:19:54 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/05 20:29:24 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

int	ft_correct_str_len(char *str, char *act)
{
	unsigned int	i;
	int				extra;

	i = 0;
	extra = 0;
	while (str[i])
	{
		ft_mv_in_quote(str, &i);
		if (ft_strchr(act, str[i]))
			extra += 2;
		i++;
	}
	return (i + extra);
}
int	ft_handle_quotes(char *res, char *str, int *i, int extra)
{
	char	quote;

	quote = str[*i];
	res[*i + extra] = str[*i]; // 開始の引用符をコピー
	(*i)++;
	while (str[*i])
	{
		res[*i + extra] = str[*i];
		if (str[*i] == quote)
			break ;
		(*i)++;
	}
	return (*i); // 引用符の位置を返す
}

int	ft_insert_spaces_around_markers(char *res, char *str, int i, int *extra,
		char *mark)
{
	if (strchr(mark, str[i])) // `<`, `>`, `|`
	{
		res[i + *extra] = ' ';
		res[i + *extra + 1] = str[i];
		res[i + *extra + 2] = ' ';
		*extra += 2;
		return (1); // 特殊文字があったことを示す
	}
	return (0);
}

void	ft_fill_correct_string(char *res, char *str, char *mark)
{
	int	i;
	int	extra;

	i = 0;
	extra = 0;
	while (str[i])
	{
		if (str[i] == '\"' || str[i] == '\'')
			i = ft_handle_quotes(res, str, &i, extra);
		if (!ft_insert_spaces_around_markers(res, str, i, &extra, mark))
			res[i + extra] = str[i];
		i++;
	}
	res[i + extra] = '\0';
}

char	*ft_correct_str(char *str)
{
	char *res;
	int new_len;

	new_len = ft_correct_str_len(str, "<>|");
	res = malloc(new_len + 1);
	if (!res)
		return (NULL);
	ft_fill_correct_string(res, str, "<>|");
	return (res);
}