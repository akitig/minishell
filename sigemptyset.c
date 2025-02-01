/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sigemptyset.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/01 23:04:02 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int sigemptyset(sigset_t *set);
インクルード：<signal.h>
役割: シグナルセット(sigset_t)を空にし、すべてのシグナルをブロックしない状態にする
返り値：成功時は0、失敗時は-1(errnoあり)
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

// Success
void handler(int signum)
{
	printf("Received signal %d\n", signum);
}

int main() {

	struct sigaction act;
	
	act.sa_handler = handler;

	int result3 = sigemptyset(NULL);
	if (result3 == -1)
	{
		printf("sigemptyset result3 fail\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	else
	{
		printf("sigemptyset result3 success\n");
	}

	int result2 = sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (result2 == -1)
	{
		printf("sigemptyset result2 fail\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	else
	{
		printf("sigemptyset result2 success\n");
	}

	int result = sigaction(SIGINT, &act, NULL);
	if (result == -1)
	{
		printf("sigaction failed\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
		exit(1);
	}
	printf("Press Ctrl+C to send SIGINT\n");
	
	while (1)
		sleep(1);

    return 0;
}

/* result
sigemptyset result3 fail
Error No.22
Error MSG: Invalid argument
sigemptyset result2 success
Press Ctrl+C to send SIGINT
^CReceived signal 2
^CReceived signal 2
^CReceived signal 2
^CReceived signal 2

*/

