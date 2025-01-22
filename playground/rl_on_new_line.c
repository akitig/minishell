#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

void	log_message(const char *message)
{
	// 入力行をリフレッシュする
	printf("\n%s\n", message); // ログを表示
	rl_on_new_line();          // 新しい行を開始
	rl_redisplay();            // 入力行を再表示
}

int	main(void)
{
	char *input;

	while (1)
	{
		input = readline("> ");
		if (!input)
			break ;

		if (*input)
			add_history(input);

		if (strcmp(input, "log") == 0)
		{
			log_message("This is a log message.");
		}

		free(input);
	}

	return (0);
}