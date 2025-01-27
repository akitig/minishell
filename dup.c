/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dup.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/27 21:29:52 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int dup(int oldfd);
インクルード：<unistd.h>
役割：fdを複製する、返るのは使われていないなかで最小のfd
返り値：成功時は新しいfd、失敗時は-1(errnoあり)

	オフセットとフラグは共有
*/

/*
int dup2(int oldfd, int newfd);
インクルード：<unistd.h>
役割：fdを複製する、newfdは指定できる
返り値：成功時は新しいfd、失敗時は-1(errnoあり)

	オフセットとフラグは共有
	newfdが開いていたら一度閉じて、上書きして返す
	もし oldfd = newfd でも成功扱いでnewfdが返る
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>


int main() {

	int oldfd = open("./out.txt", O_RDWR);
	if (oldfd < 0)
		printf("out.txt open failed\n");

	//dup success
	int fd2 = dup(oldfd);
	write(oldfd, "Oldfd Wrote Here\n", 17);
	write(fd2, "Newfd Wrote Here\n", 17);

	close(oldfd);
	close(fd2);

	//dup fail
	oldfd = open("./none.txt", O_RDWR);
	if (oldfd < 0)
		printf("none.txt open failed\n");
	int fd3 = dup(oldfd);
	if (fd3 == -1)
	{
		printf("fd3 dup failed\n");
		printf("%d\n", errno);
		printf("%s\n", strerror(errno));
	}
    return 0;
}

/* out.txt
Oldfd Wrote Here
Newfd Wrote Here

*/

/* result
none.txt open failed
fd3 dup failed
9
Bad file descriptor
*/
