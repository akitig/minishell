#include "../includes/minishell.h"
#include <stdlib.h>
#include <string.h>

/**
 * append_char():
 *   バッファ(*res)の末尾に1文字cを追加し、必要ならreallocする。
 *   size_t型の長さ(*j)と容量(*cap)を使う例。
 */
void	append_char(char **res, size_t *j, size_t *cap, char c)
{
	if (*j + 2 >= *cap)
	{
		size_t new_cap = (*cap) * 2;
		char *tmp = malloc(new_cap);
		if (!tmp)
			return ; // エラー処理は適宜追加
		ft_memcpy(tmp, *res, *j);
		free(*res);
		*res = tmp;
		*cap = new_cap;
	}
	(*res)[(*j)++] = c;
	(*res)[*j] = '\0';
}