/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/03 23:15:00 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ssize_t read(int fd, void *buf, size_t count);
インクルード：<unistd.h>
役割: ファイルの中身を読む
返り値：成功時は読んだバイト数、失敗時は-1(errnoあり)
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>


int main() {

	int fd = open("execve.c", O_RDONLY);
	if (fd == -1)
	{
		printf("open failed\n");
		return 1;
	}
	
	char buf[1000];

	// null file
	int fd2 = open("null.txt", O_RDONLY);
	ssize_t result3 = read(fd2, buf, sizeof(buf) - 1);
	if (result3 == 0)
	{
		printf("this file is empty\n");
		close(fd2);
	}

	// fail
	ssize_t result2 = read(1024000, buf, sizeof(buf) - 1);
	if (result2 == -1)
	{
		printf("read failed :<\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}

	// success
	ssize_t result = read(fd, buf, sizeof(buf) - 1);
	if (result == -1)
	{
		printf("read failed :<\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	buf[result] = '\0';
	printf("================ read ================\n");
	printf("%s\n", buf);
	printf("======================================\n");
	close(fd);
	return 0;
}



/* result
this file is empty
read failed :<
Error No.9
Error MSG: Bad file descriptor
================ read ================
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/03 22:39:17 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
int execve(const char *pathname, char *const argv[], char *const envp[]);
インクルード：<unistd.h
======================================
*/

