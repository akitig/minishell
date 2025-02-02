/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tgoto.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/03 01:25:40 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 char *tgoto(const char *cap, int col, int row);
インクルード：<curses.h> <term.h> or <termcap.h>
役割: カーソル移動用のシーケンスを生成
返り値: 成功時は生成された文字列、失敗時はNULL
*/

/*
 int tputs(const char *str, int affcnt, int (*putc)(int));
インクルード：<curses.h> <term.h> or <termcap.h>
役割: 制御文字列をターミナルに送信、出力する
返り値: 成功時は出力した文字数、失敗時は-1
*/

/*====================================================*/

/*
 int tgetent(char *bp, const char *term);
インクルード：<curses.h> <term.h> or <termcap.h>
役割: ターミナルの情報を取得する
返り値：成功時は1、失敗時は0か-1
*/

/*
char *tgetstr(const char *id, char **area);
インクルード：<curses.h> <term.h> or <termcap.h>
役割: ターミナルの文字列機能（エスケープシーケンス）を取得する
返り値: 成功時はエスケープシーケンス、失敗時はNULL
*/

/*
int tgetnum(const char *id);
インクルード：<curses.h> <term.h> or <termcap.h>
役割: ターミナルの数値型の機能を取得
返り値: 成功時は数値、失敗時は-1
*/

/*
int tgetflag(const char *id);
インクルード：<curses.h> <term.h> or <termcap.h>
役割: ターミナルの数値型の機能を取得
返り値: あれば1、なければ0
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <term.h>
#include <termcap.h>

int main() {


	char *term = getenv("TERM");
	if (term == NULL)
		printf("環境変数TERMを設定してください\n");

	int result = tgetent(NULL, term);
	if (result == 0 || result == -1)
		printf("tgetent fail\n");

	char buffer[2048];
	char *ptr = buffer;
	char *cm = tgetstr("cm", &ptr);
	if (!cm)
		printf("tgetstr fail\n");

	// tgoto
	char *result2 = tgoto(NULL, 0, 0);
	if (result2 == NULL)
		printf("tgoto fail here\n");
	char *move = tgoto(cm, 1, 10);
	if (!move)
		printf("tgoto fail\n");
	
	// tputs
	int result3 = tputs(NULL, 1, putchar);
	if (result3 == -1)
		printf("tputs fail here\n");

	tputs(move, 1, putchar);
	char *clear = tgetstr("cl", &ptr);
	if (!clear)
		printf("tgetstr fail\n");
	int cnt = tputs(clear, 1, putchar);
	if (cnt == -1)
		printf("tputs fail\n");
	else
		printf("put: %d chars\n", cnt);
    return EXIT_SUCCESS;
}

/* result
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ ./a.out
tgoto fail here
tputs fail here
put: 0 chars
rhonda@TABLET-PM80C8FG:~/minishell/minishell$




*/
