/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/31 22:46:03 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int pipe(int pipefd[2]);
インクルード：<unistd.h>
役割: 親子プロセス間通信のパイプを作る
返り値：成功時は0、失敗時は-1(errnoあり、pipefdの値はそのまま保持される)

	引数として渡したpipefd[0]はread end = 読み取り用、pipefd[1]はwrite end = 書き込み用
		のfdを設定してくれる
	単方向通信のみ可能
*/

/*
pid_t fork(void);
インクルード: 
役割: 現在のプロセスを複製して子プロセスとして作成する
親プロセス返り値: 子プロセスのPID(> 0)、失敗時は-1(errnoあり)
子プロセス返り値: 成功時は0、失敗時は-1(errnoあり)

	親プロセスと子プロセスは並列実行される
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// Success
// int main()
// {
//     int pipefd[2];
//     char buf[100];

//     if (pipe(pipefd) == -1) {
//         perror("pipe");
//         exit(EXIT_FAILURE);
//     }

// 	const char *msg = "Hello, Pipe!";
// 	// pipeに書き込み

// 	write(pipefd[1], msg, strlen(msg) + 1);
// 	// pipeから読み込み
// 	read(pipefd[0], buf, sizeof(buf));

// 	printf("パイプから受信： %s\n", buf);

// 	close(pipefd[0]);
// 	close(pipefd[1]);

// 	return 0;
// }

/* result
パイプから受信： Hello, Pipe!
*/


// Error
int main()
{
	int pipefd[2];
	int count = 0;

	while (1)
	{
		if (pipe(NULL) == -1)
		{
			printf("pipe error no.%d\n", errno);
			printf("error message: %s\n", strerror(errno));
			break ;
		}
		close(pipefd[0]);
		close(pipefd[1]);
		count++;
		usleep(1000);
	}
	printf("%d pipes were created.\n", count);
	return 0;
}

/* result
pipe error no.14
error message: Bad address
0 pipes were created.
*/
