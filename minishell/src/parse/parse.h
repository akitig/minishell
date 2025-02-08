/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:53:39 by akunimot          #+#    #+#             */
/*   Updated: 2025/02/08 17:43:49 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
#define PARSE_H

#include "../../libft/libft.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// === doller.c (環境変数処理) ===
void input_daller(char **str, char **env);
char *ft_expand_daller(char **env, char *str);
size_t ft_strlen_var(const char *str);
char *get_env_value(char **env, const char *key);
char *ft_handle_quotes(char *str);

// === utils.c ===
void ft_mv_in_quote(char const *str, unsigned int *i);
int ft_array_len(char **s, char *act);
char *ft_strndup(const char *s, size_t n);
void ft_free_split(char **str);
char *ft_strcat(char *dest, const char *src);

// === split.c ===
int ft_word_num(char const *s, char c);
char **ft_split_str(char const *str, char c);
int ft_char_len(char const *s, char c);
char *ft_fill_word(char *word, char *str, int len);

// === correct.c ===
char *ft_correct_str(char *str);

// === correct_special.c ===
char **ft_correct_special(char **str, char *spec);

// === parser.c ===
void ft_check_var(char **str, char **env);

#endif /* PARSE_H */
