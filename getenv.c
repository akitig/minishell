/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getenv.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/27 22:46:30 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
char *getenv(const char *name);
インクルード：<stdlib.h>
役割: 与えられたkeyをもとに環境変数のvalueを取得
返り値：valueのポインタ(がちテーブルなので編集しない方がよい)、失敗時はNULL(errnoなし)

	スレッドセーフでないので競合しないように注意必要
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


int main() {

	//getenv success
	char *env = getenv("HOME");
	if (env != NULL)
		printf("%s\n", env);
	else
		perror("getenv fail\n");

	//getenv fail
	env = getenv("ABO");
	if (env == NULL)
		perror("getenv fail\n");
	else
		printf("%s\n", env);
    return 0;
}

/* result
/home/rhonda
getenv fail
: Success
*/

