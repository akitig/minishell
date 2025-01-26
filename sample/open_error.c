#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// エラーの時openは - 1を返す

int	main(void)
{
	int number;

	int fd1 = open("/test/", O_RDONLY);
	// int fd2 = open("/test2/", O_RDONLY);
	// int fd3 = open("/home/akitig/Desktop/42/honda/sample/readline.c");

	if (fd1 < 0)
	{
		number = errno;
		perror("error : ");
		printf("fd1 error code : %d\n", number);
		printf("fd1 error str  : %s\n", strerror(number));
	}
	else
	{
		printf("fd1:%d\n", fd1);
		close(fd1);
	}

	// 22: invalid argument
	errno = 0;
	int ret = getpriority(10000000, 1000000);
	number = errno;
	if (ret == -1)
	{
		printf("\n");

		perror("print error string by perror  ");
		printf("print error string by strerror: %s\n", strerror(number));

		printf("print error code: %d\n", number);
		printf("print returncode: %d\n", ret);
	}
	close(fd1);
	return (0);
}