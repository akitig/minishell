#include "../includes/minishell.h"
#include "../libft/libft.h"
#include <sys/stat.h>

static char *ft_strjoin3(const char *s1, const char *s2, const char *s3)
{
	size_t len1;
	size_t len2;
	size_t len3;
	size_t total;
	char *res;

	if (!s1 || !s2 || !s3)
		return (NULL);
	// 長さ計算
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	len3 = ft_strlen(s3);
	total = len1 + len2 + len3;
	res = malloc(total + 1);
	if (!res)
		return (NULL);
	// 連結
	ft_strlcpy(res, s1, len1 + 1);
	ft_strlcat(res, s2, len1 + len2 + 1);
	ft_strlcat(res, s3, total + 1);
	return (res);
}

/******************************************************************************
 * (2) 簡易的な handle_redirections スタブ
 *     実際には <infile, >outfile, >>, << などを open(), dup2() する処理を書く。
 *     今は常に0を返して成功扱い。
 ******************************************************************************/
// int	handle_redirections(t_cmd *cmd, t_shell *shell)
// {
// 	(void)cmd;
// 	(void)shell;
// 	// ここにリダイレクト実装を書く
// 	return (0);
// }

/******************************************************************************
 * (3) envlist_to_envp:
 *     envlist を "KEY=VALUE" 形式のNULL終端配列に変換
 ******************************************************************************/
static char **envlist_to_envp(t_env *list)
{
	int count;
	t_env *cur;
	char **envp;
	int i;
	size_t len;
	char *kv;
	char *tmp;

	count = 0;
	cur = list;
	while (cur)
	{
		count++;
		cur = cur->next;
	}
	envp = malloc(sizeof(char *) * (count + 1));
	if (!envp)
		return (NULL);
	cur = list;
	i = 0;
	while (cur)
	{
		if (cur->has_equal)
		{
			len = ft_strlen(cur->key) + 1 + (cur->value ? ft_strlen(cur->value) : 0);
			kv = malloc(len + 2);
			if (!kv)
				return (NULL);
			ft_strlcpy(kv, cur->key, len + 2);
			ft_strlcat(kv, "=", len + 2);
			if (cur->value)
				ft_strlcat(kv, cur->value, len + 2);
			envp[i++] = kv;
		}
		else
		{
			// keyのみ
			tmp = ft_strdup(cur->key);
			envp[i++] = tmp;
		}
		cur = cur->next;
	}
	envp[i] = NULL;
	return (envp);
}

/******************************************************************************
 * (4) check_dir_exec:
 *     pathがディレクトリなら 126、そうでなければ -1
 ******************************************************************************/
static int check_dir_exec(const char *path)
{
	struct stat st;

	if (stat(path, &st) == 0)
	{
		if (S_ISDIR(st.st_mode))
		{
			fprintf(stderr, "minishell: %s: is a directory\n", path);
			return (126);
		}
	}
	return (-1); // not directory => continue
}

static int do_execve(char **argv, t_shell *shell)
{
	/* 先頭の空文字列をスキップ */
	while (argv && argv[0] && argv[0][0] == '\0')
		argv++;
	if (!argv[0])
		return (0);
	/* "." や ".." は実行不可 */
	if (!ft_strcmp(argv[0], ".") || !ft_strcmp(argv[0], ".."))
		return (127);

	int st;
	char *cmd;
	int cd;
	char **envp;
	int e;
	char *pathvar;
	char **paths;
	bool found;
	int exitcode;
	char *join;

	/* 1) builtin のチェック */
	if (is_builtin(argv[0]))
	{
		st = exec_builtin(argv, shell);
		return st;
	}
	/* 2) コマンド実行：絶対／相対パスか PATH 探索か */
	cmd = argv[0];
	/* (a) '/' を含む場合 */
	if (ft_strchr(cmd, '/'))
	{
		cd = check_dir_exec(cmd);
		if (cd >= 0)
			return cd;
		if (access(cmd, X_OK) != 0)
		{
			if (errno == EACCES)
			{
				fprintf(stderr, "minishell: %s:  Permission denied\n", cmd);
				return (126);
			}
			return (127);
		}
		envp = envlist_to_envp(shell->envlist);
		execve(cmd, argv, envp);
		e = errno;
		for (int i = 0; envp[i]; i++)
			free(envp[i]);
		free(envp);
		if (e == EACCES)
			return (126);
		return (127);
	}
	/* (b) PATH 探索 */
	pathvar = envlist_get_value(shell->envlist, "PATH");
	if (!pathvar)
	{
		fprintf(stderr, "%s: command not found\n", cmd);
		return (127);
	}
	paths = ft_split(pathvar, ':');
	found = false;
	exitcode = 127;
	for (int i = 0; paths[i]; i++)
	{
		join = ft_strjoin3(paths[i], "/", cmd);
		cd = check_dir_exec(join);
		if (cd >= 0)
		{
			free(join);
			exitcode = cd;
			found = true;
			break;
		}
		/* 対象ファイルは存在するが実行権限がない場合 */
		if (access(join, F_OK) == 0 && access(join, X_OK) != 0)
		{
			fprintf(stderr, "minishell: %s:  Permission denied\n", join);
			found = true;
			exitcode = 126;
			free(join);
			break;
		}
		if (access(join, X_OK) == 0)
		{
			found = true;
			envp = envlist_to_envp(shell->envlist);
			execve(join, argv, envp);
			e = errno;
			for (int k = 0; envp[k]; k++)
				free(envp[k]);
			free(envp);
			free(join);
			if (e == EACCES)
				exitcode = 126;
			else
				exitcode = 127;
			break;
		}
		free(join);
	}
	ft_free_split(paths);
	if (!found)
		fprintf(stderr, "%s: command not found\n", cmd);
	return exitcode;
}

int execute_cmds(t_cmd *cmds, t_shell *shell)
{
	if (!cmds)
		return (0);
	int fd_in = 0;
	int pipefd[2];
	int status;
	t_cmd *cur = cmds;
	pid_t pids[1024];
	int pid_count = 0;
	while (cur)
	{
		if (cur->next)
		{
			if (pipe(pipefd) < 0)
			{
				perror("pipe");
				return 1;
			}
		}
		else
		{
			pipefd[0] = -1;
			pipefd[1] = -1;
		}
		pid_t pid = fork();
		if (pid < 0)
		{
			perror("fork");
			return 1;
		}
		if (pid == 0)
		{
			/* 子プロセスでは SIGPIPE をデフォルトに戻す */
			signal(SIGPIPE, SIG_DFL);
			if (fd_in != 0)
			{
				dup2(fd_in, 0);
				close(fd_in);
			}
			if (cur->next)
			{
				dup2(pipefd[1], 1);
				close(pipefd[1]);
			}
			if (pipefd[0] != -1)
				close(pipefd[0]);
			if (handle_redirections(cur, shell) != 0)
				exit(1);
			fflush(stdout);
			fflush(stderr);
			int code = do_execve(cur->args, shell);
			exit(code);
		}
		else
		{
			pids[pid_count++] = pid;
			if (pipefd[1] != -1)
				close(pipefd[1]);
			if (fd_in != 0)
				close(fd_in);
			fd_in = pipefd[0];
		}
		cur = cur->next;
	}
	if (fd_in != 0)
		close(fd_in);
	int last_exit = 0;
	bool broken_pipe = false;
	for (int i = 0; i < pid_count; i++)
	{
		waitpid(pids[i], &status, 0);
		if (WIFSIGNALED(status))
		{
			int sig = WTERMSIG(status);
			if (sig == SIGPIPE)
				broken_pipe = true;
			last_exit = 128 + sig;
		}
		else if (WIFEXITED(status))
		{
			last_exit = WEXITSTATUS(status);
		}
	}
	if (broken_pipe)
		fprintf(stderr, "Broken pipe\n");
	return last_exit;
}