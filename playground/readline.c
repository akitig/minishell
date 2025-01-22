#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	char *line = NULL;
	int count;

	count = 0;
	while (count < 3)
	{
		line = readline("minishell > ");
		if (line == NULL)
		{
			free(line);
			break ;
		}
		printf("line:%s\n", line);
		add_history(line);
		free(line);
		if (rl_line_buffer != NULL)
		{
			printf("rl_point:%d\n", rl_point);
			printf("rl_end:%d\n", rl_end);
			printf("rl_line_buffer:%s\n", rl_line_buffer);
		}
		count++;
	}
	rl_clear_history();

	count = 0;
	while (count < 3)
	{
		line = readline("minishell > ");
		if (line == NULL)
		{
			free(line);
			break ;
		}
		printf("line:%s\n", line);
		add_history(line);
		free(line);
		if (rl_line_buffer != NULL)
		{
			printf("rl_point:%d\n", rl_point);
			printf("rl_end:%d\n", rl_end);
			printf("rl_line_buffer:%s\n", rl_line_buffer);
		}
		count++;
	}

	printf("finish\n");
	return (0);
}