/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tget.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhonda <rhonda@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:58:09 by rhonda            #+#    #+#             */
/*   Updated: 2025/02/02 23:22:18 by rhonda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	{
		printf("環境変数TERMを設定してください\n");
		return EXIT_FAILURE;
	}

	// tgetent
	int result2 = tgetent(NULL, "AAA");
	if (result2 == 0)
	{
		printf("tgetent fail A\n");
	}

	int result = tgetent(NULL, term);
	if (result == 1)
	{
		printf("tgetent success\n");
	}
	else if (result == 0)
	{
		fprintf(stderr, "%s の情報がデータベースにありません\n", term);
		return EXIT_FAILURE;
	}
	else
	{
		fprintf(stderr, "データベースを開けません\n");
		return EXIT_FAILURE;
	}

	// tgetstr
	char buffer[2048];
	char *ptr = buffer;

	char *result3 = tgetstr("", &ptr);
	if (result3 == NULL)
	{
		printf("tgetstr fail\n");
	}
	
	char *clear = tgetstr("cl", &ptr);
	if (clear)
	{
		printf("画面クリアの制御文字列: %s\n", clear);
		printf("%s", clear_screen);
	}
	else
	{
		fprintf(stderr, "画面クリアの制御文字列を取得できません\n");
		return EXIT_FAILURE;
	}

	// tgetnum
	int result4 = tgetnum("");
	if (result4 == -1)
		printf("tgetnum fail\n");
	
	int colum_cnt = tgetnum("co");
	int line_cnt = tgetnum("li");
	if (colum_cnt != -1)
		printf("幅: %d\n", colum_cnt);
	else
		printf("colum not found\n");
	
	if (line_cnt != -1)
		printf("高さ: %d\n", line_cnt);
	else
		printf("line not found\n");


	//tgetflag
	int result5 = tgetflag("");
	if (result5 == 0)
		printf("tgetflag fail\n");
	
	int bs = tgetflag("bs");
	if (bs == 1)
		printf("back space can be used\n");
	else
		printf("back space cannot be used\n");
    return EXIT_SUCCESS;
}

/* result
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ cc tget.c -o a.out -lncurses -ltinfo
rhonda@TABLET-PM80C8FG:~/minishell/minishell$ ./a.out
tgetent fail A
tgetent success
tgetstr fail
画面クリアの制御文字列:
tgetnum fail
幅: 114
高さ: 27
tgetflag fail
back space can be used
rhonda@TABLET-PM80C8FG:~/minishell/minishell$




*/
