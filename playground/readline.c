#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	char *line = NULL;

	while (1)
	{
		line = readline("minishell > ");
		if (line == NULL)
		{
			free(line);
			break ;
		}
		printf("line:%s\n", line);
		free(line);
	}
	printf("finish\n");
	return (0);
}