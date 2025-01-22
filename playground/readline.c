#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	main(void)
{
	char *line = NULL;
	int count = 0;

	while (count < 3)
	{
		// プロンプトを表示して入力を取得
		line = readline("minishell > ");
		if (line == NULL)
		{
			break ; // EOF やエラーで終了
		}

		// 入力内容を出力
		printf("line: %s\n", line);

		// 入力を履歴に追加
		add_history(line);

		// `rl_line_buffer` の内容を確認
		if (rl_line_buffer != NULL)
		{
			printf("Before modification:\n");
			printf("  rl_point: %d\n", rl_point);
			printf("  rl_end: %d\n", rl_end);
			printf("  rl_line_buffer: %s\n", rl_line_buffer);

			// 新しい内容に置き換え
			rl_replace_line("pwd", 1); // 入力バッファを置き換え

			// カーソル位置を調整
			// rl_point = strlen("replaced content"); // カーソルを文字列の末尾に移動

			// 再描画の準備
			rl_on_new_line();
			rl_redisplay();

			// printf("After modification:\n");
			printf("\n  rl_point: %d\n", rl_point);
			printf("  rl_end: %d\n", rl_end);
			printf("  rl_line_buffer: %s\n", rl_line_buffer);
		}

		// 入力メモリを解放
		free(line);
		count++;
	}

	// 履歴をクリア
	rl_clear_history();

	printf("finish\n");
	return (0);
}