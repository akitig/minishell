/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:40:32 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/04 19:44:36 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

size_t	ft_strlen_var(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] && s[i] != ' ')
		i++;
	return (i);
}

/*
static int	ft_word_len(char const *str, char c)
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
*/

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

int	ft_char_len(char const *s, char c)
{
	unsigned int	i;

	i = 0;
	while (s[i] != c && s[i])
	{
		ft_mv_in_quote(s, &i);
		i++;
	}
	return (i);
}

static int	ft_word_num(char const *s, char c)
{
	unsigned int	i;
	unsigned int	flg;
	int				num;

	i = 0;
	flg = 0;
	num = 0;
	while (s[i])
	{
		ft_mv_in_quote(s, &i);
		if (s[i] != c)
			flg = 1;
		else if (flg && s[i] == c)
		{
			num++;
			flg = 0;
		}
		i++;
	}
	if (flg)
		num++;
	return (num);
}

static char	*ft_fill_word(char *word, char *str, int len)
{
	int	i;

	i = 0;
	while (i < len)
	{
		word[i] = str[i];
		i++;
	}
	word[i] = '\0';
	return (word);
}

static int	ft_split_low(char **array, char const *s, char c)
{
	char	*str;
	char	*word;
	int		len;
	int		i;

	str = (char *)s;
	i = 0;
	while (i < ft_word_num(s, c))
	{
		len = ft_char_len(str, c);
		if (len > 0)
		{
			word = malloc(sizeof(char) * (len + 1));
			if (word == NULL)
				return (i + 1);
			word = ft_fill_word(word, str, len);
			array[i] = word;
			str += len + 1;
			i++;
		}
		else
			str++;
	}
	array[ft_word_num(s, c)] = NULL;
	return (0);
}

char	**ft_split_str(char const *str, char c)
{
	char	**array;
	int		err;
	int		i;

	array = malloc(sizeof(char *) * (ft_word_num(str, c) + 1));
	if (array == NULL)
		return (NULL);
	err = ft_split_low(array, str, c);
	if (err)
	{
		i = 0;
		while (i < err - 1)
		{
			free(array[i]);
			i++;
		}
		free(array);
		return (NULL);
	}
	return (array);
}

void	ft_check_var(char **str, char **env)
{
	int	i;

	while (*str)
	{
		i = 0;
		while (*str && (*str)[i])
		{
			if ((*str)[i] == '\'')
			{
				i++;
				while ((*str)[i] != '\'' && (*str)[i])
					i++;
			}
			else if ((*str)[i] == '$')
			{
				*str = ft_expand_daller(env, *str, i);
			}
			i++;
		}
		str++;
	}
}
int	main(int argc, char **argv, char **env)
{
	// char	*str = "$USER -la | grep \"Ma$USER ke file\"da $HOME";
	// char	*str = "ls -la << EOF |awk \"<$HOME d\" | cat";
	// char	*str = "ls -l | cat << lim | cat > outfile | cat >> outfile";
	// char	*str = "ls -l | cat << lim | cat | cat";
	// char	*str = "ls | cat | cat | cat > file | cat | cat |cat >> outfile";
	char *str = "USER|grep $HOME";
	// char *str = "$USER -la | gre p \'Ma ke file\' $HOME";
	char **res;
	int i;

	i = 0;
	(void)argc;
	(void)argv;

	res = ft_split_str(str, ' ');
	ft_check_var(res, env);
	while (res[i])
	{
		printf("%s\n", res[i]);
		i++;
	}
	printf("pipe etc\n");
	printf("%i\n", ft_array_len(res, "<|>"));
	printf("Split result:\n");
	for (int j = 0; res[j]; j++)
	{
		printf("[%s]\n", res[j]);
	}
	free(res);
	return (0);
}