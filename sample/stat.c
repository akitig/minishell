#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int	main(void)
{
	int fd;
	struct stat file_stat;
	int ret;
	// ret = stat("/home/akitig/Desktop/42/honda/sample/test.txt", &file_stat);
	fd = open("/home/akitig/Desktop/42/honda/sample/test.txt", O_RDONLY);
	ret = fstat(fd, &file_stat);
	close(fd);
	printf("%d\n", ret);

	if (ret == 0)
	{
		printf("File size: %ld bytes\n", file_stat.st_size);
		printf("File mode: %o\n", file_stat.st_mode & 0777);
	}
	else
	{
		perror("stat error");
	}
	return (0);
}