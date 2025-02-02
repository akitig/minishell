/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/02 16:59:53 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
void exit(int status);
インクルード：<stdlib.h>
役割: プログラムを終了し、OSや親プログラムに終了コードを返す
返り値：ない

	終了前にクリーンアップ
		開かれているファイルを閉じる
		メモリを開放する
		stdin, stdout, strerrのバッファをフラッシュする(fflush)
	終了コード
		EXIT_SUCCESS: 通常0
		EXIT_FAILURE: 通常1
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// void exit_wrapper()
// {
// 	exit(EXIT_SUCCESS);
// }

// int main() {

// 	for (int i = 0; i < 5; i++)
// 	{
// 		printf("program running...\n");
// 		sleep(1);
// 	}
// 	exit_wrapper();
// 	printf("I don't know why I'm here..\n");
//     return 0;
// }

/* result
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ ./a.out
program running...
program running...
program running...
program running...
program running...
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ echo $?
0
*/


void exit_wrapper2()
{
	exit(EXIT_FAILURE);
}

int main() {

	for (int i = 0; i < 5; i++)
	{
		printf("program running...\n");
		sleep(1);
	}
	exit_wrapper2();
	printf("I don't know why I'm here..\n");
    return 0;
}

/* result
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ ./a.out
program running...
program running...
program running...
program running...
program running...
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ echo $?
1
*/