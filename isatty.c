/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isatty.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/26 15:07:58 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int isatty(int fd);
インクルード：<unistd.h>
役割: 入出力が端末かパイプか判定
返り値：端末なら1、それ以外とエラーは0(errno利用可能)
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main() {

	int ret = isatty(500);
	printf("when error occured: %s\n", strerror(errno));
	printf("when error occured: %d\n", ret);

	errno = 0;
	int ret2 = isatty(STDIN_FILENO);
	printf("errorno: %d\n", errno);
    if (ret2 == 1) {
        printf("Standard input is a terminal.\n");
    } else if (ret2 == 0) {
        printf("Standard input is a file or pipe.\n");
    }

	// errno = 0;
	int ret3 = isatty(STDOUT_FILENO);
	// printf("errorno: %d\n", errno);
	// printf("errormsg: %s\n", strerror(errno));
    if (ret3 == 1) {
        printf("Standard output is a terminal.\n");
    } else if (ret3 == 0) {
        printf("Standard output is a file or pipe.\n");
    }

    return 0;
}

/* out.txt
	when error occured: Bad file descriptor
	when error occured: 0
	errorno: 0
	Standard input is a terminal.
	Standard output is a file or pipe.
*/

