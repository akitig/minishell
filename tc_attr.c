/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tc_attr.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/27 23:36:57 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int tcgetattr(int fd, struct termios *termios_p);
インクルード：<termios.h> <unistd.h>
役割: 端末の設定を取得する
返り値：成功時は0、失敗時は-1(errnoあり)
*/

/*
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
インクルード：<termios.h> <unistd.h>
役割: 端末の設定を変更する
返り値：成功時は0、失敗時は-1(errnoあり)

*/

// struct termios {
// 	          tcflag_t c_iflag;      /* input modes */
//            tcflag_t c_oflag;      /* output modes */
//            tcflag_t c_cflag;      /* control modes */
//            tcflag_t c_lflag;      /* local modes */
//            cc_t     c_cc[NCCS];   /* special characters */
// }

#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>

int main() {

	struct termios ter;
	
	//tcgetattr success
	int ret = tcgetattr(STDIN_FILENO, &ter); 
	if (ret == -1)
		perror("ret tcgetattr fail\n");
	//カノニカルモード解除、入力を非表示
	ter.c_lflag &= ~(ICANON | ECHO);
	
	//tcgetattr fail
	int ret2 = tcgetattr(900, &ter);
	if (ret2 == -1)
		perror("ret2 tcgetattr fail\n");
	
	//tcsetattr success
	int ret3 = tcsetattr(STDIN_FILENO, TCSANOW, &ter);
	if (ret3 == -1)
		perror("ret3 tcgetattr fail\n");
	
	//tcsetattr fail
	int ret4 = tcsetattr(900, TCSANOW, &ter);
	if (ret4 == -1)
		perror("ret4 tcgetattr fail\n");

    return 0;
}

/* result
ret2 tcgetattr fail
: Bad file descriptor
ret4 tcgetattr fail
: Bad file descriptor
*/
