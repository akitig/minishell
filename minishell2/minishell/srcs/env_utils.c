#include "../includes/minishell.h"
#include "../libft/libft.h"

static t_env	*new_env_node(const char *key, const char *val, bool has_eq)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	node->value = val ? ft_strdup(val) : NULL;
	node->has_equal = has_eq;
	node->next = NULL;
	return (node);
}

static t_env	*env_node_from_string(const char *str)
{
	char	*eq;
	size_t	keylen;
	char	*k;
	char	*v;
	t_env	*node;

	eq = ft_strchr(str, '=');
	if (eq)
	{
		keylen = eq - str;
		k = ft_strndup(str, keylen);
		v = ft_strdup(eq + 1);
		node = new_env_node(k, v, true);
		free(k);
		free(v);
		return (node);
	}
	else
		return (new_env_node(str, NULL, false));
}

t_env	*envlist_from_environ(char **environ)
{
	t_env	*head;
	t_env	*tail;
	int		i;
	t_env	*node;

	head = NULL;
	tail = NULL;
	i = 0;
	while (environ[i])
	{
		node = env_node_from_string(environ[i]);
		if (!head)
			head = node;
		else
			tail->next = node;
		tail = node;
		i++;
	}
	return (head);
}

char	*envlist_get_value(t_env *list, const char *key)
{
	while (list)
	{
		if (ft_strcmp(list->key, key) == 0)
			return (list->value);
		list = list->next;
	}
	return (NULL);
}

void	envlist_set_value(t_env **list, const char *key, const char *value)
{
	t_env	*cur;
	t_env	*newn;

	cur = *list;
	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
		{
			if (cur->value)
				free(cur->value);
			cur->value = ft_strdup(value);
			cur->has_equal = true;
			return ;
		}
		cur = cur->next;
	}
	newn = new_env_node(key, value, true);
	newn->next = *list;
	*list = newn;
}

void	remove_envkey(t_env **list, const char *key)
{
	t_env	*prev = NULL, *cur;

	prev = NULL, cur = *list;
	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
		{
			if (prev)
				prev->next = cur->next;
			else
				*list = cur->next;
			free(cur->key);
			if (cur->value)
				free(cur->value);
			free(cur);
			return ;
		}
		prev = cur;
		cur = cur->next;
	}
}

void	envlist_print(t_env *list)
{
	while (list)
	{
		if (list->has_equal)
		{
			if (list->value)
				printf("%s=%s\n", list->key, list->value);
			else
				printf("%s=\n", list->key);
		}
		list = list->next;
	}
}

void	free_envlist(t_env *list)
{
	t_env	*tmp;

	while (list)
	{
		tmp = list->next;
		free(list->key);
		if (list->value)
			free(list->value);
		free(list);
		list = tmp;
	}
}
