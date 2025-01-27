/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ioctl.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/27 22:27:43 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int ioctl(int fd, unsigned long request, ...);
インクルード：<sys/ioctl.h>
役割: カーネルとやり取りを行いread, write, openではできないような高度な操作を行う
返り値：成功時は0か正の値、失敗時は-1(errnoあり)

	requestコードは環境による
	windowsizeの設定とかもできるっぽい、ターミナル制御に使うぽい
*/


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main() {
    struct winsize ws;

    // ioctl で端末のウィンドウサイズを取得
	// ioctl success
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl");
    }
	printf("Terminal size: %d rows x %d columns\n", ws.ws_row, ws.ws_col);

	//ioctl fail
	if (ioctl(-1, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl");
    }

    return 0;
}

/* result
Terminal size: 27 rows x 61 columns
ioctl: Bad file descriptor
*/

