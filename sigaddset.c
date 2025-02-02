/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sigaddset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/02 13:33:45 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 int sigaddset(sigset_t *set, int signum);
インクルード：<signal.h>
役割: シグナルセット(sigset_t)にシグナルを追加する
返り値：成功時は0、失敗時は-1(errnoあり)

	使用前にsigemptysetで初期化する必要あり
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

void handler(int signum)
{
	printf("Received signal %d\n", signum);
}

int main() {

	struct sigaction act;

	sigemptyset(&act.sa_mask);

	if (sigaddset(&act.sa_mask, 56379420) == -1)
	{
		printf("sigaddset fail A\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	
	if (sigaddset(&act.sa_mask, SIGINT) == -1)
	{
		printf("sigaddset fail B\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	
	printf("=====Current signal=====\n");
	for (int i = 1; i < 64; i++)
	{
		if (sigismember(&act.sa_mask, i))
			printf("Signal No.%d is in the set.\n", i);
	}
	
    return 0;
}

/* result
sigaddset fail A
Error No.22
Error MSG: Invalid argument
=====Current signal=====
Signal No.2 is in the set.
*/
