#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

int	main(void)
{
	int		number;
	FILE	*fp;
	int		ret;

	// 2: No such file or directory
	fp = fopen("no_exist_filepath", "r");
	number = errno;
	if (fp == NULL)
	{
		perror("print error string by perror  ");
		printf("print error string by strerror: %s\n", strerror(number));
		printf("print error code: %d\n", number);
	}
	else
	{
		fclose(fp);
	}
	// 22: invalid argument
	errno = 0;
	ret = getpriority(10000000, 1000000);
	number = errno;
	if (ret == -1)
	{
		printf("\n");
		perror("print error string by perror  ");
		printf("print error string by strerror: %s\n", strerror(number));
		printf("print error code: %d\n", number);
		printf("print return (code: %d\n", ret));
	}
	return (0);
}
