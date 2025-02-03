/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unlink.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/03 22:08:54 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int unlink(const char *pathname);
インクルード：<unistd.h>
役割: ファイルを削除する（ファイルが使用されていないとき）
返り値：成功時は0、失敗時は-1(errnoあり)
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main() {

	int result = unlink("a.out");
	if (result == -1)
	{
		printf("unlink fail A\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	else if (result == 0)
	{
		printf("unlink success B\n");
	}

	int result2 = unlink("egg3.lnk");
	if (result2 == -1)
	{
		printf("unlink fail C\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	else if (result2 == 0)
	{
		printf("unlink success D\n");
	}

	int result3 = unlink("");
	if (result3 == -1)
	{
		printf("unlink success E\n");	
	}
    return 0;
}

/* result
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ ls
README.md  egg2.lnk  isatty.c     sigemptyset.c  ttyname.c
a.out      egg3.lnk  kill.c       signal.c       ttyslot.c
dir.c      exit.c    out.txt      stat.c         unlink.c
dup.c      fork.c    pipe.c       tc_attr.c      wait.c
dup2.c     func.txt  readline.c   tget.c         wait3.c
egg.lnk    getenv.c  sigaction.c  tgoto_tputs.c  wait4.c
egg.png    ioctl.c   sigaddset.c  try.c          waitpid.c
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ ./a.out
unlink success B
unlink success D
unlink success E
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ ls
README.md  exit.c    out.txt        stat.c         unlink.c
dir.c      fork.c    pipe.c         tc_attr.c      wait.c
dup.c      func.txt  readline.c     tget.c         wait3.c
dup2.c     getenv.c  sigaction.c    tgoto_tputs.c  wait4.c
egg.lnk    ioctl.c   sigaddset.c    try.c          waitpid.c
egg.png    isatty.c  sigemptyset.c  ttyname.c
egg2.lnk   kill.c    signal.c       ttyslot.c
*/

