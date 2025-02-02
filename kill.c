/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kill.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/02 16:33:32 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 int kill(pid_t pid, int sig);
インクルード：<signal.h> <sys/types.h>
役割: プロセスにシグナルを送る
返り値：一つでもシグナルを送れたら0、失敗時は-1(errnoあり)

	引数のpid
		0より大きい: 指定したPIDのプロセス
		0: 親プロセスと同じグループのすべてのプロセス
		-1: 親プロセスが権限のあるシステムすべてのプロセス
		0より小さい: 指定したPIDグループのすべてプロセス
	
	引数のsig
		0: シグナルは送られないが、PIDの存在チェックは走る
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

void handler(int signum)
{
	printf("Received signal %d\n", signum);
}

int main() {

	if (kill(12345678, SIGTERM) == -1)
	{
		printf("kill fail A\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}

	int mypid = getpid();
	if (kill(mypid, SIGTERM) == -1)
	{
		printf("kill fail B\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	
    return 0;
}

/* result
kill fail A
Error No.3
Error MSG: No such process
Terminated
*/
