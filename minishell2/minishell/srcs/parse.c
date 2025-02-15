#include "../includes/minishell.h"
#include "../libft/libft.h"

/*
** parse_line():
**   1) tokenize_bashlike -> char** tokens
**   2) 各トークンを expand_and_remove_quotes してクォート除去 + 環境変数展開
**   3) リダイレクト (< > >> <<) を解析し、cmdリストを作る
*/

static t_cmd	*new_cmd(void)
{
	t_cmd	*c;

	c = malloc(sizeof(t_cmd));
	if (!c)
		return (NULL);
	c->args = NULL;
	c->infile = NULL;
	for (int i = 0; i < 10; i++)
	{
		c->outfiles[i] = NULL;
		c->append[i] = false;
	}
	c->out_count = 0;
	c->heredoc = false;
	c->heredoc_quote = false;
	c->heredoc_delim = NULL;
	c->heredoc_fd = -1;
	c->next = NULL;
	return (c);
}

static void	add_arg(t_cmd *cmd, const char *arg)
{
	int		count;
	char	**newargs;

	count = 0;
	if (cmd->args)
	{
		while (cmd->args[count])
			count++;
	}
	newargs = malloc(sizeof(char *) * (count + 2));
	for (int i = 0; i < count; i++)
		newargs[i] = cmd->args[i];
	newargs[count] = ft_strdup2(arg);
	newargs[count + 1] = NULL;
	free(cmd->args);
	cmd->args = newargs;
}

static t_cmd	*append_cmd(t_cmd **head)
{
	t_cmd	*c;
	t_cmd	*tmp;

	c = new_cmd();
	if (!*head)
		*head = c;
	else
	{
		tmp = *head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = c;
	}
	return (c);
}

t_cmd	*parse_line(const char *input, t_shell *shell)
{
	char	**tokens;
	int		i;
	char	*expanded;
	t_cmd	*head;
	t_cmd	*current;
	char	*del;
	size_t	l;
	char	*sub;

	if (!input || !*input)
		return (NULL);
	tokens = tokenize_bashlike(input);
	if (!tokens || !*tokens)
	{
		ft_free_split(tokens);
		return (NULL);
	}
	// (1) クォート除去 + 展開
	i = 0;
	while (tokens[i])
	{
		expanded = expand_and_remove_quotes(tokens[i], shell);
		free(tokens[i]);
		tokens[i] = expanded;
		i++;
	}
	// (2) リダイレクトなどを見てcmdリスト構築
	head = NULL;
	current = NULL;
	i = 0;
	while (tokens[i])
	{
		if (!current)
			current = append_cmd(&head);
		if (ft_strcmp(tokens[i], "|") == 0)
		{
			// パイプ => 次のcmdへ
			current = NULL;
		}
		else if (ft_strcmp(tokens[i], "<") == 0)
		{
			if (tokens[i + 1])
			{
				free(current->infile);
				current->infile = ft_strdup2(tokens[++i]);
			}
		}
		else if (ft_strcmp(tokens[i], "<<") == 0)
		{
			current->heredoc = true;
			if (tokens[i + 1])
			{
				del = tokens[++i];
				// クォート付きかチェック
				l = ft_strlen(del);
				if (l >= 2 && ((del[0] == '\'' && del[l - 1] == '\'')
						|| (del[0] == '"' && del[l - 1] == '"')))
				{
					current->heredoc_quote = true;
					sub = ft_substr(del, 1, l - 2);
					current->heredoc_delim = sub;
				}
				else
					current->heredoc_delim = ft_strdup2(del);
			}
		}
		else if (ft_strcmp(tokens[i], ">") == 0)
		{
			if (tokens[i + 1])
			{
				current->outfiles[current->out_count] = ft_strdup2(tokens[++i]);
				current->append[current->out_count] = false;
				current->out_count++;
			}
		}
		else if (ft_strcmp(tokens[i], ">>") == 0)
		{
			if (tokens[i + 1])
			{
				current->outfiles[current->out_count] = ft_strdup2(tokens[++i]);
				current->append[current->out_count] = true;
				current->out_count++;
			}
		}
		else
		{
			add_arg(current, tokens[i]);
		}
		i++;
	}
	ft_free_split(tokens);
	return (head);
}

void	free_cmdlist(t_cmd *cmds)
{
	t_cmd	*tmp;
	int		i;

	while (cmds)
	{
		tmp = cmds->next;
		if (cmds->args)
		{
			i = 0;
			while (cmds->args[i])
				free(cmds->args[i++]);
			free(cmds->args);
		}
		if (cmds->infile)
			free(cmds->infile);
		for (int i = 0; i < cmds->out_count; i++)
		{
			if (cmds->outfiles[i])
				free(cmds->outfiles[i]);
		}
		if (cmds->heredoc_delim)
			free(cmds->heredoc_delim);
		free(cmds);
		cmds = tmp;
	}
}
/* parse_utils.c */

#include "../includes/minishell.h"
#include "../libft/libft.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * tokenize_bashlike():
 *   - 空白やタブでトークン区切り
 *   - 2文字演算子(<<, >>) / 1文字演算子(|,<,>) を切り出し
 *   - シングルクォート '、ダブルクォート " はそのまま文字に追加(除去しない)
 *   - 後段で expand_and_remove_quotes() を使う想定
 */
char **tokenize_bashlike(const char *input)
{
    if (!input)
        return NULL;

    // 仮のトークン格納用配列（十分大きな数を確保）
    char **tokens = malloc(sizeof(char *) * 1024);
    if (!tokens)
        return NULL;
    int tcount = 0;
    int i = 0;
    int len = ft_strlen(input);

    while (i < len)
    {
        // クォート外での空白はスキップ
        while (i < len && (input[i] == ' ' || input[i] == '\t'))
            i++;
        if (i >= len)
            break;

        // 演算子（<<, >>, |, <, >）の場合はその１文字または2文字をトークン化
        if ((input[i] == '<' || input[i] == '>') && i + 1 < len && input[i + 1] == input[i])
        {
            char tmp[3] = { input[i], input[i + 1], '\0' };
            tokens[tcount++] = ft_strdup2(tmp);
            i += 2;
            continue;
        }
        else if (input[i] == '|' || input[i] == '<' || input[i] == '>')
        {
            char tmp[2] = { input[i], '\0' };
            tokens[tcount++] = ft_strdup2(tmp);
            i++;
            continue;
        }

        // 通常のトークンを構築（クォート内は分割しない）
        char buf[4096];
        int j = 0;
        bool in_squote = false;
        bool in_dquote = false;
        while (i < len)
        {
            char c = input[i];

            // クォート開始／終了の判定（クォート文字はバッファに含める）
            if (c == '\'' && !in_dquote)
            {
                in_squote = !in_squote;
                buf[j++] = c;
                i++;
                continue;
            }
            else if (c == '"' && !in_squote)
            {
                in_dquote = !in_dquote;
                buf[j++] = c;
                i++;
                continue;
            }

            // クォート外の場合、空白・タブ、または演算子に達したらトークン終了
            if (!in_squote && !in_dquote &&
                (c == ' ' || c == '\t' ||
                 c == '|' || c == '<' || c == '>'))
                break;

            buf[j++] = c;
            i++;
        }
        buf[j] = '\0';
        tokens[tcount++] = ft_strdup2(buf);
    }
    tokens[tcount] = NULL;

    // 必要なサイズの配列を作成して返す
    char **res = malloc(sizeof(char *) * (tcount + 1));
    if (!res)
    {
        // エラー時は tokens 内の各文字列を解放する（省略）
        return NULL;
    }
    for (int k = 0; k < tcount; k++)
        res[k] = tokens[k];
    res[tcount] = NULL;
    free(tokens);
    return res;
}

/**
 * split_line_to_tokens():
 *   現状は tokenize_bashlike() を呼ぶだけの例。
 */
char **split_line_to_tokens(const char *input)
{
    return tokenize_bashlike(input);
}