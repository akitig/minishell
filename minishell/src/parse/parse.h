/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:53:39 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/04 19:33:14 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
#define PARSE_H

#include "../../libft/libft.h"

// doller.c
void input_daller(char **str, char **env);
char *ft_expand_daller(char **env, char *str, int index);

// utils.c
void ft_mv_in_quote(char const *str, unsigned int *i);
int ft_array_len(char **s, char *act);

// split.c
int ft_array_len(char **array, char *delimiters);
size_t ft_strlen_var(const char *s);

#endif