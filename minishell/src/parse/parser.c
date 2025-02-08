/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 13:39:43 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/08 15:48:57 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

t_node	*ft_parse_loop(char **str)
{
	t_node	node;
	int	num_arg;

	if (!ft_strncmp("<", *str, 2))
	{
		node.input = open(*(str + 1), O_RDONLY);
		if (node.input < 0)
			perror("can not opne the file");
		str += 2;
	}
	while (*str && !ft_strncmp(*str, "|", 2))
	{
		node.path = ft_get_path(*str);
		num.arg = ft_num_args(str);
		node.cmd = malloc(sizeof(char *) * (num_arg + 1));
		if (!node.cmd)
			return (NULL);
	}
}
