#include "../includes/minishell.h"

void	init_shell(t_shell *shell, char **envp)
{
	shell->envlist = envlist_from_environ(envp);
	shell->last_exit_status = 0;
}

int	minishell_loop(t_shell *shell)
{
	char	*input;
	t_cmd	*cmds;
	int		status;

	while (1)
	{
		input = readline("minishell> ");
		if (!input)
		{
			printf("exit\n");
			break ;
		}
		if (*input)
			add_history(input);
		cmds = parse_line(input, shell);
		free(input);
		if (cmds)
		{
			status = execute_cmds(cmds, shell);
			shell->last_exit_status = status;
			free_cmdlist(cmds);
		}
	}
	return (0);
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*res;
	size_t	i;

	res = malloc(n + 1);
	if (!res)
		return (NULL);
	i = 0;
	for (; i < n; i++)
		res[i] = s[i];
	res[i] = '\0';
	return (res);
}

char	*ft_strdup2(const char *s)
{
	size_t	len;
	char	*res;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	ft_strlcpy(res, s, len + 1);
	return (res);
}

void	ft_free_split(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

int	ft_isalnum_(char c)
{
	return (ft_isalnum(c) || c == '_');
}
int	ft_isalpha_(char c)
{
	return (ft_isalpha(c) || c == '_');
}

long	ft_atol(const char *s)
{
	int		i;
	long	sign;
	long	base;

	sign = 1;
	base = 0;
	i = 0;
	if (s[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (s[i] == '+')
	{
		i++;
	}
	while (ft_isdigit(s[i]))
	{
		base = base * 10 + (s[i] - '0');
		i++;
	}
	return (base * sign);
}

char	*get_env_value(t_shell *shell, const char *key)
{
	int		i;
	size_t	key_len;

	if (!shell || !shell->envp || !key)
		return (NULL);
	key_len = ft_strlen(key);
	i = 0;
	while (shell->envp[i])
	{
		// "KEY=VALUE" の形式を探索
		if (!ft_strncmp(shell->envp[i], key, key_len)
			&& shell->envp[i][key_len] == '=')
		{
			// '='の直後が値
			return (&shell->envp[i][key_len + 1]);
		}
		i++;
	}
	return (NULL);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	signal(SIGPIPE, SIG_IGN);
	setup_signal_handlers();
	init_shell(&shell, envp);
	minishell_loop(&shell);
	free_envlist(shell.envlist);
	return (0);
}
