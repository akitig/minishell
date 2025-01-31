/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:40:32 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/01 00:04:03 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

static int	ft_wordlen(char const *str, char c)
{
	unsigned int	i;
	int				len;

	i = 0;
	len = 0;
	while (str[i])
	{
		if (str[i] != c)
		{
			len++;
			while (str[i] && str[i] != c)
				i++;
		}
		else
			i++;
	}
	return (len);
}

char	*ft_strndup(const char *src, size_t n)
{
	char	*dst;
	size_t	i;

	dst = (char *)malloc(sizeof(char) * (n + 1));
	if (!dst)
		return (NULL);
	i = 0;
	while (i < n && src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (dst);
}

static char	*ft_allocate_word(const char *str, char c)
{
	int		len;
	char	*word;

	len = 0;
	while (str[len] && str[len] != c)
		len++;
	word = malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	strncpy(word, str, len);
	word[len] = '\0';
	return (word);
}

static void	ft_free_array(char **array, int i)
{
	while (i > 0)
		free(array[--i]);
	free(array);
}

static char	*expand_daller(char *word, char **env)
{
	int		i;
	char	*ret;

	i = 0;
	while (env[i])
	{
		if (!ft_strncmp(env[i], word + 1, ft_strlen(word) - 1))
		{
			ret = ft_strdup(&env[i][5]);
			free(word);
			return (ret);
		}
		i++;
	}
	return (word);
}

static void	input_daller(char **str, char **env)
{
	while (*str)
	{
		if (*str[0] == '$')
			*str = expand_daller(*str, env);
		str++;
	}
}

char	**ft_split_str(char const *str, char c)
{
	char	**array;
	int		words;
	int		i;

	words = ft_wordlen(str, c);
	array = malloc(sizeof(char *) * (words + 1));
	if (!array)
		return (NULL);
	i = 0;
	while (*str)
	{
		if (*str != c)
		{
			array[i] = ft_allocate_word(str, c);
			if (!array[i])
				return (ft_free_array(array, i), NULL);
			while (*str && *str != c)
				str++;
			i++;
		}
		else
			str++;
	}
	array[i] = NULL;
	return (array);
}

int	main(int argc, char **argv, char **env)
{
	char *str = "$USER -la | gre p \'Ma ke file\' $HOME";
	char **res;
	int i;

	i = 0;
	(void)argc;
	(void)argv;
	res = ft_split_str(str, ' ');
	input_daller(res, env);
	while (res[i])
	{
		printf("%s\n", res[i]);
		free(res[i]);
		i++;
	}
	free(res);
	return (0);
}