/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:43:20 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/08 17:43:40 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

void	ft_mv_in_quote(char const *str, unsigned int *i)
{
	char	quote;

	if (str[*i] == '"' || str[*i] == '\'')
	{
		quote = str[*i];
		(*i)++;
		while (str[*i] && str[*i] != quote)
			(*i)++;
	}
}

void	ft_free_split(char **str)
{
	int	i;

	i = 0;
	if (!str)
		return ;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*dup;
	size_t	len;

	len = 0;
	while (s[len] && len < n)
		len++;
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	for (size_t i = 0; i < len; i++)
		dup[i] = s[i];
	dup[len] = '\0';
	return (dup);
}

char	*ft_strcat(char *dest, const char *src)
{
	char	*ptr;

	ptr = dest + ft_strlen(dest);
	while (*src)
		*ptr++ = *src++;
	*ptr = '\0';
	return (dest);
}
