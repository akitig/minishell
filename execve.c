/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/03 22:39:17 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int execve(const char *pathname, char *const argv[], char *const envp[]);
インクルード：<unistd.h>
役割: プログラムを実行する
返り値：成功時はなし(プロセスが新しいプログラムに置き換わるため)、失敗時は-1(errnoあり)

	引数の配列はNULL終端必要
	プロセスIDは変わらない
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main() {

	char *argv[] = {"/bin/ls", "-al", NULL}; // NULL終端
	char *envp[] = {NULL}; //NULL終端

	printf("execve was called..\n");
	int result2 = execve("", argv, envp);
	if (result2 == -1)
	{
		printf("execve fail A\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	
	printf("execve was called..\n");
	int result = execve("/bin/ls", argv, envp);
	if (result == -1)
	{
		printf("execve fail B\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
    return 0;
}

/* result
execve was called..
execve fail A
Error No.2
Error MSG: No such file or directory
execve was called..
total 188
drwxr-xr-x 3 rhonda rhonda  4096 Feb  3 22:29 .
drwxr-xr-x 3 rhonda rhonda  4096 Jan 21 21:31 ..
drwxr-xr-x 8 rhonda rhonda  4096 Feb  3 21:46 .git
-rw-r--r-- 1 rhonda rhonda   450 Feb  2 12:15 .gitignore
-rw-r--r-- 1 rhonda rhonda    12 Jan 21 21:31 README.md
-rwxr-xr-x 1 rhonda rhonda 16200 Feb  3 22:29 a.out
-rw-r--r-- 1 rhonda rhonda  3854 Jan 27 20:54 dir.c
-rw-r--r-- 1 rhonda rhonda  2220 Jan 27 21:29 dup.c
-rw-r--r-- 1 rhonda rhonda  2896 Jan 27 21:53 dup2.c
lrwxrwxrwx 1 rhonda rhonda     7 Feb  3 21:22 egg.lnk -> egg.png
-rw-r--r-- 1 rhonda rhonda 17840 Jan 13 01:10 egg.png
lrwxrwxrwx 1 rhonda rhonda     7 Feb  3 21:38 egg2.lnk -> egg.png
-rw-r--r-- 1 rhonda rhonda  4054 Feb  3 22:29 execve.c
-rw-r--r-- 1 rhonda rhonda  2320 Feb  2 16:59 exit.c
-rw-r--r-- 1 rhonda rhonda  2424 Jan 31 23:49 fork.c
-rw-r--r-- 1 rhonda rhonda    81 Jan 27 20:54 func.txt
-rw-r--r-- 1 rhonda rhonda  1580 Jan 27 23:23 getenv.c
-rw-r--r-- 1 rhonda rhonda  1843 Jan 27 22:27 ioctl.c
-rw-r--r-- 1 rhonda rhonda  2003 Feb  1 16:38 isatty.c
-rw-r--r-- 1 rhonda rhonda  2117 Feb  2 16:33 kill.c
-rw-r--r-- 1 rhonda rhonda    73 Jan 27 21:50 out.txt
-rw-r--r-- 1 rhonda rhonda  2316 Jan 31 22:50 pipe.c
-rw-r--r-- 1 rhonda rhonda  1605 Jan 27 20:54 readline.c
-rw-r--r-- 1 rhonda rhonda  2858 Feb  2 12:15 sigaction.c
-rw-r--r-- 1 rhonda rhonda  2042 Feb  2 13:33 sigaddset.c
-rw-r--r-- 1 rhonda rhonda  2387 Feb  2 13:07 sigemptyset.c
-rw-r--r-- 1 rhonda rhonda  2154 Feb  2 13:00 signal.c
-rw-r--r-- 1 rhonda rhonda  4814 Feb  3 21:28 stat.c
-rw-r--r-- 1 rhonda rhonda  2407 Jan 27 23:36 tc_attr.c
-rw-r--r-- 1 rhonda rhonda  3846 Feb  2 23:22 tget.c
-rw-r--r-- 1 rhonda rhonda  3485 Feb  3 01:25 tgoto_tputs.c
-rw-r--r-- 1 rhonda rhonda  1605 Jan 21 21:58 try.c
-rw-r--r-- 1 rhonda rhonda  1788 Jan 27 20:54 ttyname.c
-rw-r--r-- 1 rhonda rhonda  1389 Jan 27 20:54 ttyslot.c
-rw-r--r-- 1 rhonda rhonda  2776 Feb  3 22:08 unlink.c
-rw-r--r-- 1 rhonda rhonda  3054 Feb  1 16:21 wait.c
-rw-r--r-- 1 rhonda rhonda  5036 Feb  1 18:18 wait3.c
-rw-r--r-- 1 rhonda rhonda  6452 Feb  1 18:56 wait4.c
-rw-r--r-- 1 rhonda rhonda  2977 Feb  1 17:06 waitpid.c
*/



