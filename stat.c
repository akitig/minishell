/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/03 21:28:37 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 int stat(const char *pathname, struct stat *statbuf);
インクルード：<sys/stat.h> <sys/types.h> <unistd.h>
役割: ファイル名をもとにファイル情報を取得する、シンボリックリンクはその先の実体の情報を取得する
返り値：成功時は0、失敗時は-1(errnoあり)
*/

/*
 int lstat(const char *pathname, struct stat *statbuf);
インクルード：<sys/stat.h> <sys/types.h> <unistd.h>
役割: ファイル名をもとにファイル情報を取得する、シンボリックリンクはシンボリックリンクそのものの情報を取得する
返り値：成功時は0、失敗時は-1(errnoあり)
*/

/*
 int fstat(int fd, struct stat *statbuf);
インクルード：<sys/stat.h> <sys/types.h> <unistd.h>
役割: fdをもとに情報をファイル取得する、シンボリックリンクはその先の実体の情報を取得する
返り値：成功時は0、失敗時は-1(errnoあり)
*/

// struct stat {
//     dev_t     st_dev;     // デバイスID
//     ino_t     st_ino;     // inode番号
//     mode_t    st_mode;    // ファイルタイプとパーミッション
//     nlink_t   st_nlink;   // ハードリンク数
//     uid_t     st_uid;     // 所有者のユーザーID
//     gid_t     st_gid;     // 所有者のグループID
//     dev_t     st_rdev;    // 特殊ファイルの場合のデバイスID
//     off_t     st_size;    // ファイルのサイズ
//     time_t    st_atime;   // 最後のアクセス時間
//     time_t    st_mtime;   // 最後の変更時間
//     time_t    st_ctime;   // 最後の状態変更時間
//     blksize_t st_blksize; // 推奨されるI/Oバッファサイズ
//     blkcnt_t  st_blocks;  // 割り当てられたブロック数
// };


#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>


int main() {

	// stat success
	struct stat sb;
	int result = stat("egg.lnk", &sb);
	if (result == -1)
	{
		printf("stat failed A\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	else if (result == 0)
	{
		printf("stat success A\n");
		printf("file size: %lld bytes\n", (long long)sb.st_size);
	}

	// stat fail
	int result2 = stat("", &sb);
	if (result2 == -1)
	{
		printf("stat failed B\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}

	// fstat success
	int fd = open("egg.lnk", O_RDONLY);
	int result3 = fstat(fd, &sb);
	if (result3 == -1)
	{
		printf("fstat failed C\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	else if (result3 == 0)
	{
		struct tm *tm = localtime(&sb.st_atime);
		char str[20];
		strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", tm);
		printf("fstat success C\n");
		printf("last access: %s\n", str);
	}
	
	// fstat fail
	int result4 = fstat(-9999, &sb);
	if (result4 == -1)
	{
		printf("fstat failed D\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}

	// lstat success
	int result5 = lstat("egg.lnk", &sb);
	if (result5 == -1)
	{
		printf("lstat failed E\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	else if (result5 == 0)
	{
		printf("lstat success E\n");
		printf("file size: %lld bytes\n", (long long)sb.st_size);
	}

	// lstat fail
	int result6 = lstat("", &sb);
	if (result4 == -1)
	{
		printf("lstat failed F\n");
		printf("Error No.%d\n", errno);
		printf("Error MSG: %s\n", strerror(errno));
	}
	return 0;
}

/* result
stat success A
file size: 17840 bytes
stat failed B
Error No.2
Error MSG: No such file or directory
fstat success C
last access: 2025-02-03 21:17:46
fstat failed D
Error No.9
Error MSG: Bad file descriptor
lstat success E
file size: 7 bytes
lstat failed F
Error No.2
Error MSG: No such file or directory
*/
