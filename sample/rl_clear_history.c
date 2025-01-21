/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rl_clear_history.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 19:18:01 by akunimot          #+#    #+#             */
/*   Updated: 2025/01/10 17:31:23 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h> // printf
#include <stdlib.h> // free
#include <readline/readline.h> // readline
#include <readline/history.h> // rl_clear_history
	
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

	
	printf("rl_end         : %d\n", rl_end);
	printf("rl_point       : %d\n", rl_point);
	printf("rl_line_buffer : %s\n", rl_line_buffer);
	rl_redisplay();
	//printf("history length:%d\n", his_len);
	return (0);
}

