#include "../includes/minishell.h"
#include "../libft/libft.h"

bool	is_builtin(const char *cmd)
{
	if (!cmd)
		return (false);
	if (ft_strcmp(cmd, "echo") == 0)
		return (true);
	if (ft_strcmp(cmd, "cd") == 0)
		return (true);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (true);
	if (ft_strcmp(cmd, "export") == 0)
		return (true);
	if (ft_strcmp(cmd, "unset") == 0)
		return (true);
	if (ft_strcmp(cmd, "env") == 0)
		return (true);
	if (ft_strcmp(cmd, "exit") == 0)
		return (true);
	return (false);
}

int	exec_builtin(char **argv, t_shell *shell)
{
	if (!argv[0])
		return (0);
	if (ft_strcmp(argv[0], "echo") == 0)
		return (builtin_echo(argv));
	if (ft_strcmp(argv[0], "cd") == 0)
		return (builtin_cd(argv, shell));
	if (ft_strcmp(argv[0], "pwd") == 0)
		return (builtin_pwd(argv, shell));
	if (ft_strcmp(argv[0], "export") == 0)
		return (builtin_export(argv, shell));
	if (ft_strcmp(argv[0], "unset") == 0)
		return (builtin_unset(argv, shell));
	if (ft_strcmp(argv[0], "env") == 0)
		return (builtin_env(argv, shell));
	if (ft_strcmp(argv[0], "exit") == 0)
		return (builtin_exit(argv, shell));
	return (0);
}

int	builtin_echo(char **argv)
{
	int	i;
	int	newline;

	/* パイプライン内で echo が実行される場合、stdin を明示的に閉じる */
	// close(STDIN_FILENO);
	i = 1;
	newline = 1;
	if (argv[1] && ft_strcmp(argv[1], "-n") == 0)
	{
		newline = 0;
		i = 2;
	}
	for (; argv[i]; i++)
	{
		ft_putstr_fd(argv[i], STDOUT_FILENO);
		if (argv[i + 1])
			write(STDOUT_FILENO, " ", 1);
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	/* 出力を確実にフラッシュし、_exit() で即終了 */
	fsync(STDOUT_FILENO);
	_exit(0);
}

int	builtin_pwd(char **argv, t_shell *shell)
{
	char	cwd[PATH_MAX];

	(void)shell;
	(void)argv;
	if (getcwd(cwd, PATH_MAX))
	{
		printf("%s\n", cwd);
		return (0);
	}
	perror("pwd");
	return (1);
}

int	builtin_env(char **argv, t_shell *shell)
{
	(void)argv;
	envlist_print(shell->envlist);
	return (0);
}

/*
** exit
**  - no arg => exit(0)
**  - 1 arg numeric => exit(n)
**  - 1 arg non-numeric => exit(2) + "numeric argument required"
**  - >=2 args => "too many arguments" => return 1 (shell継続)
*/
static int	is_numeric_arg(const char *s)
{
	int	i;

	if (!s || !*s)
		return (0);
	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

int	builtin_exit(char **argv, t_shell *shell)
{
	int		argc;
	long	val;

	(void)shell;
	argc = count_args(argv);
	/* "exit" を出力（必要に応じて） */
	printf("exit\n");
	if (argc == 1)
		exit(0);
	if (!is_numeric_arg(argv[1]))
	{
		fprintf(stderr, "minishell: exit: %s: numeric argument required\n",
			argv[1]);
		exit(2);
	}
	val = ft_atol(argv[1]);
	if (argc > 2)
	{
		fprintf(stderr, "minishell: exit: too many arguments\n");
		return (1);
	}
	exit((unsigned char)val);
	return (0); // 到達しない
}

#include "../includes/minishell.h"
#include "../libft/libft.h"
#include <limits.h>

int	count_args(char **argv)
{
	int	i;

	i = 0;
	while (argv[i])
		i++;
	return (i);
}

static void	update_pwd(t_shell *shell)
{
	char	cwd[PATH_MAX];

	if (getcwd(cwd, PATH_MAX))
		envlist_set_value(&shell->envlist, "PWD", cwd);
}

int	builtin_cd(char **argv, t_shell *shell)
{
	int		argc;
	char	cwd[PATH_MAX];
	char	*home;
	char	*oldpwd;

	argc = count_args(argv);
	if (argc > 2)
	{
		fprintf(stderr, "cd: too many arguments\n");
		return (1);
	}
	if (argc == 1)
	{
		// cd だけなら HOME を使う
		home = envlist_get_value(shell->envlist, "HOME");
		if (!home)
		{
			fprintf(stderr, "cd: HOME not set\n");
			return (1);
		}
		if (chdir(home) != 0)
		{
			perror("cd");
			return (1);
		}
		update_pwd(shell);
		return (0);
	}
	if (ft_strcmp(argv[1], "-") == 0)
	{
		oldpwd = envlist_get_value(shell->envlist, "OLDPWD");
		if (!oldpwd)
		{
			fprintf(stderr, "cd: OLDPWD not set\n");
			return (1);
		}
		printf("%s\n", oldpwd);
		if (chdir(oldpwd) != 0)
		{
			perror("cd");
			return (1);
		}
	}
	else
	{
		if (chdir(argv[1]) != 0)
		{
			perror("cd");
			return (1);
		}
	}
	if (getcwd(cwd, PATH_MAX))
	{
		oldpwd = envlist_get_value(shell->envlist, "PWD");
		if (oldpwd)
			envlist_set_value(&shell->envlist, "OLDPWD", oldpwd);
		envlist_set_value(&shell->envlist, "PWD", cwd);
	}
	return (0);
}
#include "../includes/minishell.h"
#include "../libft/libft.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int	is_numeric_argument(const char *s)
{
	int	i;

	i = 0;
	if (!s || !s[0])
		return (0);
	// '+' または '-' は先頭1文字だけ許容
	if (s[0] == '+' || s[0] == '-')
		i++;
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

/*
** minishell_exit:
**   - 引数がない場合: 0で exit()
**   - 引数が1個かつ数値: その値を 8bit折りたたみして exit()
**   - 引数が1個かつ非数値: エラーメッセージを出して exit(255)
**   - 引数が2個以上: "too many arguments" として終了しない (bash互換)
*/
int	minishell_exit(char **argv)
{
	long	exit_code;

	/*
	** 課題仕様によっては "exit\n" を出力するかどうか決まっている場合がある
	** bash はインタラクティブ時に "exit" を表示、スクリプト実行時は出さない など
	** ここでは簡易に表示する例
	*/
	ft_putendl_fd("exit", STDERR_FILENO);
	// argv[0] が "exit" と想定、argv[1]~が引数
	if (!argv[1])
	{
		// 引数なし => exit(0)
		exit(0);
	}
	// 引数1個目が数値でない場合 => 255
	if (!is_numeric_argument(argv[1]))
	{
		// bashフォーマット: bash: exit: {arg}: numeric argument required
		// minishellでは自由にメッセージを設定
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(argv[1], STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		exit(255);
	}
	// 数値の場合 => atolで変換
	errno = 0;
	exit_code = ft_atol(argv[1]);
	// 引数2個以上 => too many arguments
	if (argv[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		// bash互換: 終了しない, return 1
		return (1);
	}
	// 数値1個のみ => exit( exit_code & 255 )
	exit_code = exit_code & 255;
	exit(exit_code);
	return (0); // 到達しない
}

#include "../includes/minishell.h"
#include "../libft/libft.h"

/* builtins_export.c */

static bool	valid_identifier(const char *key)
{
	int	i;

	// 先頭が [a-zA-Z_] でなければ false
	if (!ft_isalpha_(key[0]))
		return (false);
	// 2文字目以降 [a-zA-Z0-9_]* でなければ false
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum_(key[i]))
			return (false);
		i++;
	}
	return (true);
}

int	builtin_export(char **argv, t_shell *shell)
{
	int		i;
	int		ret;
	char	*eq;
	t_env	*newn;
	size_t	klen;
	char	*key;
	char	*val;

	i = 1;
	ret = 0;
	if (!argv[1]) // 引数なければ envlist_print
	{
		envlist_print(shell->envlist);
		return (0);
	}
	while (argv[i])
	{
		eq = ft_strchr(argv[i], '=');
		if (!eq)
		{
			// "export KEY" → 新規にキーを追加(has_equal=false) or すでにあれば何もしない
			if (!envlist_get_value(shell->envlist, argv[i]))
			{
				if (!valid_identifier(argv[i]))
				{
					fprintf(stderr, "export: `%s': not a valid identifier\n",
						argv[i]);
					ret = 1;
				}
				else
				{
					// 新規追加
					newn = malloc(sizeof(t_env));
					newn->key = ft_strdup(argv[i]);
					newn->value = NULL;
					newn->has_equal = false;
					newn->next = shell->envlist;
					shell->envlist = newn;
				}
			}
		}
		else
		{
			// "KEY=VAL"
			klen = eq - argv[i];
			key = ft_strndup(argv[i], klen);
			val = ft_strdup(eq + 1);
			if (!valid_identifier(key))
			{
				fprintf(stderr, "export: `%s': not a valid identifier\n",
					argv[i]);
				ret = 1;
			}
			else
			{
				envlist_set_value(&shell->envlist, key, val);
			}
			free(key);
			free(val);
		}
		i++;
	}
	return (ret);
}
int	builtin_unset(char **argv, t_shell *shell)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		remove_envkey(&shell->envlist, argv[i]);
		i++;
	}
	return (0);
}
