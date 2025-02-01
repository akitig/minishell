/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sigaction.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/01 22:32:03 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
インクルード：<signal.h>
役割: シグナルを設定、変更する
返り値：成功時は0、失敗時は-1(errnoあり)


	actがNULLの場合は変更なし、oldactがNULLの場合は設定前のactの保存なし
	SIGKILLとSIGSTOPは変更不可

flagの例
	SA_RESTART: シグナルハンドラ終了後に割り込まれたシステムコールを再開
	SA_NOCLDWAIT: SIGCHID受信時に子プロセスのゾンビ化を防ぐ
	SA_SIGINFO: actで詳細情報を受取る
*/

// struct sigaction {
//     void     (*sa_handler)(int);
//     void     (*sa_sigaction)(int, siginfo_t *, void *);
//     sigset_t   sa_m	act.sa_handler = handler;
//     int        sa_flags;
//     void     (*sa_restorer)(void);
// };



#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// int main() {

// 	struct sigaction act;
// 	act.sa_handler = handler;
// 	sigemptyset(&act.sa_mask);
// 	act.sa_flags = 0;

// 	if (sigaction(SIGINT, &act, NULL) == -1)
// 	{
// 		printf("sigaction failed\n");
// 		printf("Error No.%d\n", errno);
// 		printf("Error MSG: %s\n", strerror(errno));
// 		exit(1);
// 	}

// 	while (1)
// 	{
// 		printf("Waiting for signal...\n");
// 		sleep(1);
// 	}

//     return 0;
// }

/* result
Waiting for signal...
Waiting for signal...
Waiting for signal...
Waiting for signal...
^CCaught signal 2
Waiting for signal...
Waiting for signal...
Waiting for signal...
*/

// Fail

void handler(int signum)
{
	printf("Hi\n");
}

int main()
{
	struct sigaction act;
	act.sa_handler = handler;
	// sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	int result = sigaction(SIGKILL, &act, NULL);
	if (result == -1)
	{
		printf("sigaction failed\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
		exit(1);
	}
	return 0;
}

/*
sigaction failed
Error No.22
Error MSG: Invalid argument
*/