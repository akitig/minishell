#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

/*
** グローバル変数: シグナル状態などを記録
*/
extern volatile sig_atomic_t	g_signal_status;

/*================================================================*/
/*            データ構造                                         */
/*================================================================*/

/* 環境変数リスト */
typedef struct s_env
{
	char						*key;
	char						*value;
	bool						has_equal;
	struct s_env				*next;
}								t_env;

/* コマンド1つ分 */
typedef struct s_cmd
{
	char **args;        // execve引数
	char *infile;       // < infile
	char *outfiles[10]; // > or >> 複数対応用
	bool append[10];    // それぞれが >> かどうか
	int out_count;      // outfiles に何個入ったか
	bool heredoc;       // << 有無
	bool heredoc_quote; // << のDELIMがクォート付きか
	char						*heredoc_delim;
	int							heredoc_fd;
	struct s_cmd				*next;
}								t_cmd;

/* トークンの種類 */
typedef enum e_toktype
{
	TOK_WORD,
	TOK_OP,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_EOF,
	TOK_ERROR
}								t_toktype;

typedef struct s_token
{
	t_toktype					type;
	char						*value;
}								t_token;

/* シェル全体の管理 */
typedef struct s_shell
{
	t_env						*envlist;
	int last_exit_status; // $?
	char **envp;          // 追加：環境変数配列
}								t_shell;

/*================================================================*/
/*          関数プロトタイプ                                     */
/*================================================================*/

/* main.c */
int								minishell_loop(t_shell *shell);

/* signals.c */
void							setup_signal_handlers(void);
void							sigint_handler(int signo);
void							sigquit_handler(int signo);

/* env_utils.c */
t_env							*envlist_from_environ(char **environ);
char							*envlist_get_value(t_env *list,
									const char *key);
void							envlist_set_value(t_env **list, const char *key,
									const char *value);
void							remove_envkey(t_env **list, const char *key);
void							envlist_print(t_env *list);
void							free_envlist(t_env *list);

/* shell_utils.c */
int								ft_strcmp(const char *s1, const char *s2);
char							*ft_strndup(const char *s, size_t n);
char							*ft_strdup2(const char *s);
int								ft_isalnum_(char c);
int								ft_isalpha_(char c);
long							ft_atol(const char *s);
void							ft_free_split(char **arr);

/* builtins */
bool							is_builtin(const char *cmd);
int								exec_builtin(char **argv, t_shell *shell);

/* builtins_cd.c */
int								builtin_cd(char **argv, t_shell *shell);
int								count_args(char **argv);

/* builtins_export.c */
int								builtin_export(char **argv, t_shell *shell);
int								builtin_unset(char **argv, t_shell *shell);

/* builtins_utils.c */
int								builtin_echo(char **argv);
int								builtin_pwd(char **argv, t_shell *shell);
int								builtin_env(char **argv, t_shell *shell);
int								builtin_exit(char **argv, t_shell *shell);

/* expansions.c */
char							*expand_and_remove_quotes(const char *token,
									t_shell *shell);

/* parse.c */
t_cmd							*parse_line(const char *input, t_shell *shell);
void							free_cmdlist(t_cmd *cmds);

/* parse_utils.c */
void							append_char(char **res, size_t *len,
									size_t *cap, char c);
char							**split_line_to_tokens(const char *input);
char							**tokenize_bashlike(const char *input);

/* execute.c */
int								execute_cmds(t_cmd *cmds, t_shell *shell);

/* redirections.c */
int								handle_redirections(t_cmd *cmd, t_shell *shell);

#endif