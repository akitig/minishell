/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ttyname.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/26 15:16:21 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
char *ttyname(int fd);
インクルード：<unistd.h>
役割: 標準入力、標準出力、または他のファイルディスクリプタがターミナルに接続されている場合、
　　　その端末の名前（例えば /dev/tty1 や /dev/pts/0 など）を取得することができる
返り値：成功時は端末の名前(char *)、失敗時はNULL
*/

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main()
{
	char *ret = ttyname(0);
	printf("%s\n", ret);
	char *ret2 = ttyname(1);
	printf("%s\n", ret2);
	char *ret3 = ttyname(2);
	printf("%s\n", ret3);
	char *ret4 = ttyname(5000);
	printf("errno: %d\n", errno);
	printf("errmsg: %s\n", strerror(errno));
	printf("error return of ttyname: %s\n", ret4);
	return (0);
}

/*result

/dev/pts/0
/dev/pts/0
/dev/pts/0
errno: 9
errmsg: Bad file descriptor
error return of ttyname: (null)

*/