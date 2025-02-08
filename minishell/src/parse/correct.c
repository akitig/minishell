/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   correct.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 20:19:54 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/08 16:26:51 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"
#include <stdlib.h>
#include <string.h>

char	*ft_correct_str(char *str)
{
	int		len;
	char	*new_str;
	int		i = 0, j;

	len = ft_strlen(str) + 10;
	new_str = malloc(len);
	if (!new_str)
		return (NULL);
	i = 0, j = 0;
	while (str[i])
	{
		if (str[i] == '>' || str[i] == '<' || str[i] == '|')
		{
			new_str[j++] = ' ';
			new_str[j++] = str[i];
			if (str[i + 1] == str[i])
				new_str[j++] = str[++i];
			new_str[j++] = ' ';
		}
		else
			new_str[j++] = str[i];
		i++;
	}
	new_str[j] = '\0';
	return (new_str);
}
