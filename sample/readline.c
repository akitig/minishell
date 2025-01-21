/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 10:03:47 by akunimot          #+#    #+#             */
/*   Updated: 2025/01/08 10:11:00 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h> // printf
#include <stdlib.h> // free
#include <readline/readline.h> // readline

int ft_strlen(char *str)
{
	int i;
	int len;

	i = 0;
	len = 0;
	while (str[i])
	{
		len++;
		i++;
	}
	return (len);
}

int main()
{
	char *line = NULL;
	while (1)
	{
		line = readline("> ");
		if (line == NULL || ft_strlen(line) == 0)
		{
			free(line);
			break;
		}
		printf("line is '%s'\n", line);
		// add_history(line);

		free(line);
	}
	printf("exit\n");
	return (0);
}
