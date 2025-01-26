/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   try.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/01/21 21:58:27 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main() {
    char *input;

    while (1) {
        // readline関数で入力を受け取る
        input = readline("prompt> ");

        // 入力がNULL（Ctrl+Dなどで終了）ならループを抜ける
        if (input == NULL) {
            printf("\nExiting...\n");
            break;
        }

        // 入力が空でない場合は履歴に追加
        if (*input) {
            add_history(input);
        }

        // 入力内容を表示
        printf("You entered: %s\n", input);

        // readlineで確保されたメモリを解放
        free(input);
    }

    return 0;
}
