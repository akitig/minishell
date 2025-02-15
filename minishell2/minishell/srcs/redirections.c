#include "../includes/minishell.h"
#include "../libft/libft.h"
#include <sys/stat.h>

static void	do_heredoc(t_cmd *cmd, t_shell *shell)
{
	int		pipefd[2];
	char	*line;
	char	*exp;

	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		exit(1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ; // ctrl+D
		if (ft_strcmp(line, cmd->heredoc_delim) == 0)
		{
			free(line);
			break ;
		}
		if (!cmd->heredoc_quote)
		{
			// $展開
			exp = expand_and_remove_quotes(line, shell);
			free(line);
			line = exp;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	cmd->heredoc_fd = pipefd[0];
}

/*
** handle_redirections():
**   <infile, >outfile, >>outfile, << heredoc
**   複数あれば順に処理。途中失敗なら exit(1)
*/
int	handle_redirections(t_cmd *cmd, t_shell *shell)
{
	int	fd;

	(void)shell;
	// heredoc
	if (cmd->heredoc)
	{
		do_heredoc(cmd, shell);
		dup2(cmd->heredoc_fd, 0);
		close(cmd->heredoc_fd);
	}
	// <infile
	if (cmd->infile)
	{
		fd = open(cmd->infile, O_RDONLY);
		if (fd < 0)
{
    if (errno == ENOENT)
        fprintf(stderr, "minishell: %s:  No such file or directory\n", cmd->infile);
    else if (errno == EACCES)
        fprintf(stderr, "minishell: %s:  Permission denied\n", cmd->infile);
    else
        perror("minishell");
    return (1);
}
		dup2(fd, 0);
		close(fd);
	}
	// outfiles
	for (int i = 0; i < cmd->out_count; i++)
	{
		if (cmd->append[i])
			fd = open(cmd->outfiles[i], O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(cmd->outfiles[i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
		{
			if (errno == ENOENT)
				fprintf(stderr, "  No such file or directory\n");
			else if (errno == EACCES)
				fprintf(stderr, "  Permission denied\n");
			else
				perror("open");
			return (1);
		}
		dup2(fd, 1);
		close(fd);
	}
	return (0);
}
