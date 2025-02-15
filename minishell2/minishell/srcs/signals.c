#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include "../includes/minishell.h"


volatile sig_atomic_t g_signal_status = 0;

void sigint_handler(int signo)
{
    (void)signo;
    g_signal_status = SIGINT;
    rl_on_new_line();
    rl_replace_line("", 0);
    write(STDOUT_FILENO, "\n", 1);
    rl_redisplay();
}

void sigquit_handler(int signo)
{
    (void)signo;
    g_signal_status = SIGQUIT;
    rl_on_new_line();
    rl_redisplay();
}

void setup_signal_handlers(void)
{
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    /* 親シェルでは SIGPIPE の設定は行わない（子プロセスで必ずデフォルトに戻す） */
}
