/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   waitpid.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/01 16:58:27 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
pid_t waitpid(pid_t pid, int *wstatus, int options);
インクルード：<sys/types.h> <sys/wait.h>
役割: 親プロセスで特定の子プロセスの終了を待機する
返り値：成功時は終了した子プロセスのPID、失敗時(子プロセスがいない、シグナルで中断等)は-1(errnoあり)

	引数のpid
		0より大きい: 指定したPIDのプロセス
		0: 親プロセスと同じグループの任意のプロセス
		-1: 親プロセスにとって最も適切なプロセス
		0より小さい: 指定したPIDグループのプロセス
	
	引数のoptions
		WNOHANG: 子プロセスがない場合はすぐ戻る
		WUNTRACED: 停止した子プロセスを待機する
		WCONTINUED: 継続された子プロセスを待機する
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

// Success
// int main() {

// 	pid_t pid = fork();

// 	if (pid == -1)
// 	{
// 		printf("fork failed\n");
// 		return 1;
// 	}

// 	if (pid == 0) //child process
// 	{
// 		printf("Child process\n");
// 		exit(0);
// 	}
// 	else //parent process
// 	{
// 		int status;
// 		pid_t child_pid = waitpid(pid, &status, 0);

// 		if (child_pid == -1)
// 		{
// 			printf("waitpid failed\n");
// 			return 1;
// 		}
// 		if (WIFEXITED(status))
// 			printf("Child exited with status %d\n", WEXITSTATUS(status));
// 		else
// 			printf("Child did not exit normally.\n");
// 	}
//     return 0;
// }

/* result
Child process
Child exited with status 0
*/


// Fail
int main()
{
	int status;
	int result = waitpid(420000000, &status, 0);
	if (result == -1)
	{
		printf("waitpid failed\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	return 0;
}

/* result
waitpid failed
Error No.10
Error MSG: No child processes
*/