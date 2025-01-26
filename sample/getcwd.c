#include <stdio.h>
#include <unistd.h>

int	main(void)
{
	char	*ret;

	ret = getcwd(NULL, 0);
	printf("%s\n", ret);
	free(ret);
	return (0);
}
