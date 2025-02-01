/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait4.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/01 18:38:48 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 pid_t wait3(int *wstatus, int options, struct rusage *rusage);
インクルード：<sys/types.h> <sys/wait.h> <sys/time.h> <sys/resource.h>
役割: 親プロセスで（最も適切な）子プロセスの終了を待機する、リソース使用状況を取得する
返り値：成功時は終了した子プロセスのPID、失敗時は-1(errnoあり)

	wait3(wstatus, options, rusage); = waitpid(-1, wstatus, options);

	引数のoptions
		WNOHANG: 子プロセスがない場合はすぐ戻る
		WUNTRACED: 停止した子プロセスを待機する
		WCONTINUED: 継続された子プロセスを待機する
*/

/*
pid_t wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage);
インクルード：<sys/types.h> <sys/wait.h> <sys/time.h> <sys/resource.h>
役割: 親プロセスで特定の子プロセスの終了を待機する、リソース使用状況を取得する
返り値：成功時は終了した子プロセスのPID、失敗時は-1(errnoあり)

	 wait4(pid, wstatus, options, rusage); = waitpid(pid, wstatus, options);

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

// struct rusage {
// 	struct timeval ru_utime; /* user CPU time used */
//     struct timeval ru_stime; /* system CPU time used */
//     long   ru_maxrss;        /* maximum resident set size */
//     long   ru_ixrss;         /* integral shared memory size */
//     long   ru_idrss;         /* integral unshared data size */
//     long   ru_isrss;         /* integral unshared stack size */
//     long   ru_minflt;        /* page reclaims (soft page faults) */
//     long   ru_majflt;        /* page faults (hard page faults) */
//     long   ru_nswap;         /* swaps */
//     long   ru_inblock;       /* block input operations */
//     long   ru_oublock;       /* block output operations */
//     long   ru_msgsnd;        /* IPC messages sent */
//     long   ru_msgrcv;        /* IPC messages received */
//     long   ru_nsignals;      /* signals received */
//     long   ru_nvcsw;         /* voluntary context switches */
//     long   ru_nivcsw;        /* involuntary context switches */
// };

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/resource.h>

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
// 		struct rusage usage;
// 		pid_t child_pid = wait4(pid, &status, 0, &usage);

// 		if (child_pid == -1)
// 		{
// 			printf("wait4 failed\n");
// 			return 1;
// 		}
// 		if (WIFEXITED(status))
// 		{
// 			printf("Child exited with status %d\n", WEXITSTATUS(status));
// 			printf("============= RESOURCE USAGE =============\n");
// 			printf("User CPU time: %ld.%06ld seconds\n", 
//                    usage.ru_utime.tv_sec, usage.ru_utime.tv_sec);
// 			printf("Resource usage for child process:\n");
// 			printf("Page faults (minor): %ld\n", usage.ru_minflt);
//             printf("Page faults (major): %ld\n", usage.ru_majflt);
// 			printf("Input block operations: %ld\n", usage.ru_inblock);
//             printf("Output block operations: %ld\n", usage.ru_oublock);
//             printf("Messages sent: %ld\n", usage.ru_msgsnd);
//             printf("Messages received: %ld\n", usage.ru_msgrcv);
//             printf("Signals received: %ld\n", usage.ru_nsignals);
// 			printf("==========================================\n");
// 		}
// 		else
// 			printf("Child did not exit normally.\n");
// 	}
//     return 0;
// }

/* result
Child process
Child exited with status 0
============= RESOURCE USAGE =============
User CPU time: 0.000000 seconds
Resource usage for child process:
Page faults (minor): 31
Page faults (major): 0
Input block operations: 0
Output block operations: 0
Messages sent: 0
Messages received: 0
Signals received: 0
==========================================
*/


// Fail                   -- rusageがNULLでもwaitは成功する
int main()
{
	int status;
	struct rusage rusage;

	pid_t pid = fork();
	if (pid == -1)
	{
		printf("fork failed\n");
		return 1;
	}
	if (pid == 0)
		exit(0);

	int result = wait4(420000000, &status, 0, &rusage);
	if (result == -1)
	{
		printf("wait4 failed A\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	int result2 = wait4(pid, &status, 0, NULL);
	if (result2 == -1)
	{
		printf("wait4 failed B\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	else
	{
		if (WIFEXITED(status))
		{
			printf("wait4 success B\n");
			printf("Exited with status %d\n", WEXITSTATUS(status));
		}
	}
	int result3 = wait4(pid, NULL, 0, &rusage);
	if (result3 == -1)
	{
		printf("wait4 failed C\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	return 0;
}

/* result
wait4 failed A
Error No.10
Error MSG: No child processes
wait4 success B
Exited with status 0
wait4 failed C
Error No.10
Error MSG: No child processes
*/