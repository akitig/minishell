/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ttyslot.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/26 16:37:54 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int ttyslot(void);
インクルード：<unistd.h>
役割: 現在のシステムの端末のスロット番号を取得
返り値：成功時はスロット番号、失敗時は-1
*/

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main()
{
	int ret = ttyslot();
	printf("%d\n", ret);
	if (ret == -1)
	{
		printf("errno: %d\n", errno);
		printf("errmsg: %s\n", strerror(errno));
		printf("error return of ttyname: %d\n", ret);
	}
	return (0);
}

/*result
0
*/