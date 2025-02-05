/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:43:20 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/04 19:49:43 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

void	ft_mv_in_quote(char const *str, unsigned int *i)
{
	char	quote;

	if (str[*i] == '\"' || str[*i] == '\'')
	{
		quote = str[(*i)++];
		while (str[*i])
		{
			if (str[*i] == quote)
				break ;
			(*i)++;
		}
	}
}

int	ft_array_len(char **s, char *act)
{
	unsigned int i;
	int j;
	int cnt;

	j = 0;
	cnt = 0;
	while (s && s[j])
	{
		i = 0;
		while (s[j][i])
		{
			ft_mv_in_quote(s[j], &i);
			if (ft_strchr(act, s[j][i]))
			{
				cnt++;
				if (i > 0)
					cnt++;
				if (s[j][i + 1])
					cnt++;
			}
			i++;
		}
		j++;
	}
	cnt += j;
	return (cnt);
}