#include <stdio.h>
#include <unistd.h>

// pathが存在しないときは変わらない return = -1

int	main(void)
{
	char *ret;
	ret = getcwd(NULL, 0);
	printf("now path : %s\n", ret);
	int test = chdir("/test/");
	ret = getcwd(NULL, 0);
	printf("aft path : %s\n", ret);
	printf("return (: %d\n", test);
	return (0);
}