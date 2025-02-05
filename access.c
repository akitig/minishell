/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   access.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/05 19:57:36 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 int access(const char *pathname, int mode);
インクルード：<unistd.h>
役割: 
返り値：権限ありは0、権限なしは-1(errnoあり)

	引数mode
		R_OK: 読み取り
		W_OK: 書き込み
		X_OK: 実行
		F_OK: 存在するか
	accessの結果よりもopenやreadの返り値でエラーチェック推奨
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main() {

	char *file = "a.out";

	if (access(file, F_OK) == 0)
	{
		printf("%s は存在します\n", file);
		
		if (access(file, R_OK) == 0)
			printf("読み取り可能です\n");
		else
			printf("読み取り不可です\n");
		if (access(file, W_OK) == 0)
			printf("書き込み可能です\n");
		else
			printf("書き込み不可です\n");
		if (access(file, X_OK) == 0)
			printf("実行可能です\n");
		else
			printf("実行不可です\n");
	}
	else
	{
		printf("access failed\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}

	if (access("", F_OK) == 0)
	{
		printf("%s は存在します\n", file);
		
		if (access(file, R_OK) == 0)
			printf("読み取り可能です\n");
		else
			printf("読み取り不可です\n");
		if (access(file, W_OK) == 0)
			printf("書き込み可能です\n");
		else
			printf("書き込み不可です\n");
		if (access(file, X_OK) == 0)
			printf("実行可能です\n");
		else
			printf("実行不可です\n");
	}
	else
	{
		printf("access failed\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
    return 0;
}

/* result
a.out は存在します
読み取り可能です
書き込み可能です
実行可能です
access failed
Error No.2
Error MSG: No such file or directory
*/

