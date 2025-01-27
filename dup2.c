/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dup2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/27 21:52:07 by rhonda           ###   ########.fr       */
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

	//dup2 success
	int fd2 = dup2(oldfd, 100);
	if (fd2 == 100)
	{
		printf("fd = 100 dup2 success\n");
		write(oldfd, "Oldfd Wrote Here\n", 17);
		write(fd2, "Newfd Wrote Here\n", 17);
	}
	else if (fd2 == -1)
		printf("fd = 100 dup2 failed\n");

	//newfd already used
	int fd3 = dup2(oldfd, fd2);
	if (fd3 == -1)
		printf("already used dup2 failed\n");
	else if (fd3 == fd2)
	{
		printf("already used dup2 success\n");
		write(fd3, "Newfd from already used dup2 Wrote Here\n", 39);
	}

	// oldfd = newfd
	errno = 0;
	printf("oldfd: %d\n", oldfd);
	int fd4 = dup2(oldfd, oldfd);
	if (fd4 != -1)
		printf("newfd: %d\n", fd4);
	else
	{
		printf("%d\n", errno);
		printf("%s\n", strerror(errno));
	}

	close(oldfd);
	/* d2 already closed */
	close(fd3);
	close(fd4);

	//dup2 fail
	oldfd = open("./none.txt", O_RDWR);
	if (oldfd < 0)
		printf("none.txt open failed\n");
	int fd5 = dup2(oldfd, 100);
	if (fd5 == -1)
	{
		printf("fd5 dup failed\n");
		printf("%d\n", errno);
		printf("%s\n", strerror(errno));
	}
	close(oldfd);
    return 0;
}

/* out.txt
Oldfd Wrote Here
Newfd Wrote Here
Newfd from already used dup2 Wrote Here
*/

/* result
none.txt open failed
fd3 dup failed
9
Bad file descriptor
*/
