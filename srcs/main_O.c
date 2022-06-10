#include "../include/minishell.h"

char *concat_argvs(int argc, char **argv)
{
	int i;
	char *ret;
	char *tmp;
	char *tmp2;

	i = 2;
	tmp = NULL;
	tmp2 = NULL;
	ret = ft_strdup(argv[1]);
	while (i < argc)
	{
		tmp2 = ft_strjoin(ret, " ");
		tmp = ft_strjoin(tmp2, argv[i]);
		free(tmp2);
		if (ret)
			free(ret);
		ret = tmp;
		i++;
	}
	return (ret);
}

void	free_all_tokens(t_token **tokens)
{
	t_token *tmp;
	t_token *tmp2;

	tmp = *tokens;
	while (tmp)
	{
		if (tmp->value)
			free(tmp->value);
		if (tmp->next)
		{
			tmp2 = tmp->next;
			free(tmp);
			tmp = tmp2;
		}
		else
		{
			free(tmp);
			tmp = NULL;
		}
	}
}

int main_agent_O(t_info *info)
{
	t_big_token	*tmp_b;

	info->tokens = NULL;
	info->parse = NULL;
	detect_tokens(&info->tokens, info->rdline);
	fill_tok_value(&info->tokens, info->rdline);
	index_toks(&info->tokens, 0, len_ll_list(info->tokens));
	if (syntax_err_handler(&info->tokens))
	{
		printf("Error number: %d\n", syntax_err_handler(&info->tokens));
		free_all_tokens(&info->tokens);
		ft_putstr_fd("Parsing error\n", 2);
		return (1);
	}
	parse(&info->parse, &info->tokens, 0, len_ll_list(info->tokens));
	tmp_b = info->parse;
	if (info->nb_cmd == 10)
		print_all_everything(&tmp_b, &info->tokens);
	return (0);
}