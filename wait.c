/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/01 16:21:28 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
pid_t wait(int *wstatus);
インクルード：<sys/types.h> <sys/wait.h>
役割: 親プロセスで子プロセスの終了を待つ
返り値：成功時は終了した子プロセスのPID、失敗時(子プロセスがいない、シグナルで中断等)は-1(errnoあり)

	引数のwstatusに終了した子プロセスの終了ステータス等の情報が格納される
	引数にNULLを渡すと、終了ステータスの回収は行われず、プロセスの回収のみ行われる(ゾンビを防ぐ)
*/

/*
WIFEXITED(status);
wait if exited
子プロセスがexitやreturnによって正常に終了したらtrueを返す
シグナルによって異常終了したらfalseを返す

WEXITSTATUS(status)
wait exit status
子プロセスがexit(code)やreturn(code)で終了したら、そのコードを取得できる
先にWIFEXITED(status)がtrueか確認して使う
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

// // Success
// int main()
// {
// 	pid_t pid = fork();
	
// 	if (pid == -1)
// 	{
// 		printf("fork failed\n");
// 		exit 1;
// 	}
	
// 	if (pid == 0) //child process
// 	{
// 		printf("Child Process\n");
// 		exit (EXIT_SUCCESS);	
// 	}
// 	else // parent process
// 	{
// 		int status;
// 		pid_t child_pid = wait(&status);
// 		if (child_pid > 0)
// 		{
// 			printf("No.%d Proceess ended\n", child_pid);
// 			if (WIFEXITED(status))
// 				printf("No.%d process exited with status %d\n", child_pid, WEXITSTATUS(status));
// 		}
// 		else if (child_pid == -1)
// 		{
// 			printf("wait failed\n");
// 			printf("errno: %d\n", errno);
// 			printf("error msg: %s\n", strerror(errno));
// 		}
// 	}
// 	return 0;
	
// }

/* result
Child Process
No.14097 Proceess ended
No.14097 process exited with status 0
*/


// Fail
// int main()
// {
// 	int status;
// 	pid_t result = wait(&status);

// 	if (result == -1)
// 	{
// 		printf("wait failed\n");
// 		printf("errno: %d\n", errno);
// 		printf("error msg: %s\n", strerror(errno));
// 	}
// 	return 0;
// }

/* result
wait failed
errno: 10
error msg: No child processes
*/
