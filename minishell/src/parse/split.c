/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:40:32 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/03 23:12:41 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

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
void	ft_mv_in_quote(char const *str, unsigned int *i)
{
	char	quote;

	if (str[*i] == '\"' || str[*i] == '\'')
	{
		quote = str[*i];
		(*i)++;
		while (str[*i])
		{
			if (str[*i] == quote)
				break ;
			(*i)++;
		}
	}
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

int	ft_correct_len(char *str, char *mark)
{
	unsigned int	i;
	int				extra_mark;

	i = 0;
	extra_mark = 0;
	while (str[i])
	{
		ft_mv_in_quote(str, &i);
		if (ft_strchr(mark, str[i]))
			extra_mark += 2;
		i++;
	}
	// printf("%d %d\n", i, extra_mark);
	return (i + extra_mark);
}

void	ft_fix_correct_str(char *ret, char *str, int *i, int *extra)
{
	char	quote;

	if (str[*i] == '\"' || str[*i] == '\'')
	{
		quote = str[*i];
		ret[*i + *extra] = str[*i];
		(*i)++;
		while (str[*i])
		{
			ret[*i + *extra] = str[*i];
			if (str[*i] == quote)
				break ;
			(*i)++;
		}
		if (ft_strchr("<>|&", str[*i]))
		{
			ret[*i + *extra] = ' ';
			ret[*i + *extra + 1] = str[*i];
			ret[*i + *extra + 2] = ' ';
			(*extra) += 2;
		}
		else
			ret[*i + *extra] = str[*i];
	}
}

char	*ft_correct_str(char *str)
{
	char	*ret;
	int		i;
	int		extra;

	i = 0;
	extra = 0;
	ret = malloc(ft_correct_len(str, "<>|&") + 1);
	if (!ret)
		return (NULL);
	while (str[i])
	{
		ft_fix_correct_str(ret, str, &i, &extra);
		i++;
	}
	ret[i + extra] = '\0';
	printf("%c\n", ret[0]);
	return (ret);
}

int	main(int argc, char **argv, char **env)
{
	// char	*str = "$USER -la | grep \"Ma$USER ke file\"da $HOME";
	// char	*str = "ls -la << EOF |awk \"<$HOME d\" | cat";
	// char	*str = "ls -l | cat << lim | cat > outfile | cat >> outfile";
	// char	*str = "ls -l | cat << lim | cat | cat";
	// char	*str = "ls | cat | cat | cat > file | cat | cat |cat >> outfile";
	char *str = "$USER -la | gre p \'Ma ke file\' $HOME";
	char **res;
	int i;

	i = 0;
	(void)argc;
	(void)argv;
	// str = ft_correct_str(str);
	printf("%s\n", str);
	res = ft_split_str(str, ' ');
	input_daller(res, env);
	// res = ft_correct_special(res, "<>|&");
	while (res[i])
	{
		printf("%s\n", res[i]);
		free(res[i]);
		i++;
	}
	free(res);
	return (0);
}