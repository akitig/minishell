/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:40:32 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/08 17:37:56 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

int	ft_word_num(char const *s, char c)
{
	unsigned int	i;
	unsigned int	flag;
	int				num;

	i = 0;
	flag = 0;
	num = 0;
	while (s[i])
	{
		ft_mv_in_quote(s, &i);
		if (s[i] != c)
			flag = 1;
		else if (flag && s[i] == c)
		{
			num++;
			flag = 0;
		}
		i++;
	}
	if (flag)
		num++;
	return (num);
}

static int	ft_split_low(char **array, char const *s, char c)
{
	char	*str;
	char	*word;

	str = (char *)s;
	int len, i = 0;
	while (i < ft_word_num(s, c))
	{
		len = ft_char_len(str, c);
		if (len > 0)
		{
			word = malloc(sizeof(char) * (len + 1));
			if (!word)
			{
				fprintf(stderr, "Error: malloc failed in ft_split_low()\n");
				return (i + 1);
			}
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

	int i, words;
	words = ft_word_num(str, c);
	array = malloc(sizeof(char *) * (words + 1));
	if (!array)
	{
		fprintf(stderr, "Error: malloc failed in ft_split_str()\n");
		return (NULL);
	}
	for (i = 0; i < words + 1; i++)
		array[i] = NULL;
	if (ft_split_low(array, str, c))
	{
		for (i = 0; i < words; i++)
			if (array[i])
				free(array[i]);
		free(array);
		return (NULL);
	}
	// Debugging output
	printf("=== Debug: Split Tokens ===\n");
	for (i = 0; array[i]; i++)
	{
		printf("Token[%d]: %s (addr: %p)\n", i, array[i], (void *)array[i]);
	}
	printf("===========================\n");
	return (array);
}

#include "parse.h"

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

char	*ft_fill_word(char *word, char *str, int len)
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
/*
int	main(int argc, char **argv, char **env)
{
	char	*str;
	char	**res;
	int		i;

	str = ">>$USER|grep \"<$HOME d\" >file";
	i = 0;
	(void)argc;
	(void)argv;
	str = ft_correct_str(str);
	res = ft_split_str(str, ' ');
	if (!res)
	{
		perror("Memory allocation failed in ft_split_str()");
		free(str);
		return (1);
	}
	printf("=== Debug: Split Result ===\n");
	for (int j = 0; res[j]; j++)
	{
		if (res[j] == NULL)
			printf("Debug: res[%d] is NULL\n", j);
		else
			printf("res[%d]: %s\n", j, res[j]);
	}
	printf("===========================\n");
	ft_check_var(res, env);
	ft_correct_special(res, "<>|&");
	i = 0;
	while (res[i])
	{
		if (!res[i])
		{
			fprintf(stderr, "Error: res[%d] is NULL\n", i);
			continue ;
		}
		if ((uintptr_t)res[i] < 0x1000) // Invalid memory address detection
		{
			fprintf(stderr, "Error: res[%d] has an invalid address: %p\n", i,
				res[i]);
			continue ;
		}
		printf("%s\n", res[i]);
		i++;
	}
	ft_free_split(res);
	free(str);
	return (0);
}
*/

int	main(int argc, char **argv, char **env)
{
	char	*str1;
	char	*str2;
	char	*str3;

	str1 = ft_strdup(">>$USER|grep \"<$HOME d\" >file");
	str2 = ft_strdup("'$USER' \"$HOME\"");
	str3 = ft_strdup("echo \"$USER\" | grep 'home'");
	(void)argc;
	(void)argv;
	printf("\n=== Before Expansion ===\n");
	printf("str1: %s\n", str1);
	printf("str2: %s\n", str2);
	printf("str3: %s\n", str3);
	fflush(stdout);
	input_daller(&str1, env);
	input_daller(&str2, env);
	input_daller(&str3, env);
	printf("\n=== After Expansion ===\n");
	printf("str1: %s\n", str1);
	printf("str2: %s\n", str2);
	printf("str3: %s\n", str3);
	free(str1);
	free(str2);
	free(str3);
	return (0);
}
