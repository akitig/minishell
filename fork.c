/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/31 23:34:34 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
pid_t fork(void);
インクルード: <sys/types.h> <unistd.h>
役割: 現在のプロセスを複製して子プロセスとして作成する
親プロセス返り値: 子プロセスのPID(> 0)、失敗時は-1(errnoあり)
子プロセス返り値: 成功時は0、失敗時は-1(errnoあり)

	親プロセスと子プロセスは並列実行される
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// Success
int main() {

	pid_t pid = fork();

	if (pid == -1)
	{
		printf("fork failed\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
		return 1;
	}
	else if (pid == 0)
	{
		printf("child process: PID=%d, Parent PID=%d\n", getpid(), getppid());
	}
	else
	{
		printf("parent process: PID=%d, Child PID=%d\n", getpid(), pid);
		wait(NULL);
	}

    return 0;
}

/* result
child process: PID=44189, Parent PID=44188
parent process: PID=44188, Child PID=44189
*/

// Error
// int main() {
//     int max_forks = 500;
//     int i = 0;

//     while (i < max_forks)
// 	{
//         pid_t pid = fork();

//         if (pid == -1)
// 		{
// 			printf("errno: %d\n", errno);
//             perror("fork failed");
//             break;
//         }

// 		if (pid == 0)
// 			return 0;
		
// 		wait(NULL);
// 		i++;
//     }
//     return 0;
// }

/*
errno: 11
fork failed: Resource temporarily unavailable
*/
