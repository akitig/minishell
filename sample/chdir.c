#include <stdio.h>
#include <unistd.h>

int	main(void)
{
	char *pwd;
	pwd = getcwd(NULL, 0);
	printf("pwd before chdir: %s\n", pwd);
	chdir("/home/akitig/Desktop/42/honda");
	pwd = getcwd(NULL, 0);
	printf("pwd after  chdir: %s\n", pwd);
	return (0);
}