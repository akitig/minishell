/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dir.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/26 17:59:10 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
DIR *opendir(const char *name);
インクルード：<sys/types.h> <dirent.h>
役割: ディレクトリをオープンする
返り値：成功時はディレクトリのポインタ、失敗時はNULL
*/

/*
struct dirent *readdir(DIR *dirp);
インクルード：<dirent.h>
役割: ディレクトリの情報を読む
返り値：成功時はdirentのポインタ、ディレクトリ情報の終端時はNULLでerrno変更なし、失敗時はNULLでerrno設定される
		終端かエラーか判断するために、readdir前でerrnoを0にし、値を確認する必要あり
*/

// struct dirent {
//                ino_t          d_ino;       /* Inode number */
//                off_t          d_off;       /* Not an offset; see below */
//                unsigned short d_reclen;    /* Length of this record */
//                unsigned char  d_type;      /* Type of file; not supported
//                                               by all filesystem types */
//                char           d_name[256]; /* Null-terminated filename */
// };


/*
int closedir(DIR *dirp);
インクルード：<sys/types.h> <dirent.h>
役割: ディレクトリをクローズする、dripに関連るするファイルディスクリプタもクローズする
返り値：成功時は0、失敗時は-1(errnoあり)
*/


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"


int main() {

	//opendir success
	DIR *ret = opendir("/home/rhonda/Cursus/minishell/sandbox");
	if (ret)
	{
		printf("opendir sucsess\n");
	}

	//opendir fail
	DIR *ret2 = opendir("/home/rhonda/Cursus/minishell/fail");
	if (ret2 == NULL)
	{
		printf("opendir fail\n");
		printf("errorno: %d\n", errno);
		printf("errormsg: %s\n", strerror(errno));
	}

	//readdir success
	errno = 0;
	struct dirent *ent = readdir(ret);
	if (ent)
	{
		printf("readdir sucsess\n");
	}
	while (ent && errno == 0)
	{
		printf("name: %s\n", ent->d_name);
		ent = readdir(ret);
	}

	//readdir fail
	// struct dirent *ret4 = readdir(ret2);
	// if (ret4 == NULL)
	// {
	// printf("readdir fail\n");
	// 	printf("errorno: %d\n", errno);
	// 	printf("errormsg: %s\n", strerror(errno));
	// }

	//closedir success
	int ret5 = closedir(ret);
	if (ret5 == 0)
	{
		printf("closedir sucsess\n");
	}

	//closedir fail
	int ret6 = closedir(NULL);
	if (ret6 == -1)
	{
		printf("closedir fail\n");
		printf("errorno: %d\n", errno);
		printf("errormsg: %s\n", strerror(errno));
	}


    return 0;
}

/* result

% ./a.out 
opendir sucsess
opendir fail
errorno: 2
errormsg: No such file or directory
readdir sucsess
name: .
name: ..
name: .git
name: .gitignore
name: README.md
name: .vscode
name: func.txt
name: readline.c
name: out.txt
name: isatty.c
name: ttyname.c
name: ttyslot.c
name: nohup.out
name: dir.c
name: a.out
closedir sucsess
closedir fail
errorno: 22
errormsg: Invalid argument

*/

