/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/02 13:00:15 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
sighandler_t signal(int signum, sighandler_t handler);
渡すシグナルハンドラ: typedef void (*sighandler_t)(int)
インクルード：<signal.h>
役割: シグナルハンドラを設定する
返り値：成功時は変更前のハンドラ、失敗時はSIG_ERR(errnoあり)

	※環境によって挙動が異なる -> POSIXではsighandler_tがないためvoid *を返す
	sigaction推奨
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

void handler (int signum)
{
	printf("Caught SIGINT\n");
}

int main() {

	void *old = signal(657438948, handler);
	if (old == SIG_ERR)
	{
		printf("signal failed A\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	
	old = signal(SIGINT, handler);
	if (old == SIG_ERR)
	{
		printf("signal failed B\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}

	while (1)
	{
		printf("waiting for signal...\n");
		sleep(5);
	}
    return 0;
}

/* result
signal failed A
Error No.22
Error MSG: Invalid argument
waiting for signal...
^CCaught SIGINT
waiting for signal...
^CCaught SIGINT
waiting for signal...
^CCaught SIGINT
waiting for signal...
waiting for signal...
*/
