/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_launch_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nflan <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/06 10:29:00 by nflan             #+#    #+#             */
/*   Updated: 2022/06/15 19:03:04 by nflan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

extern int	sc;

int	ft_deeper_bt(t_big_token *b_tokens, t_big_token **tmp_b)
{
	int	i;

	i = 0;
	*tmp_b = b_tokens;
	if (!tmp_b)
		return (-1);
	while ((*tmp_b)->child)
	{
		*tmp_b = (*tmp_b)->child;
		i++;
	}
	return (i);
}

int	ft_wash_btoken(t_info *info, t_big_token *b_tokens)
{
	t_token	*tokens;
	int	i;

	i = b_tokens->ind_tok_start;
	tokens = info->tokens;
//	printf("WASH: Start = %d && length = %d\n", b_tokens->ind_tok_start, b_tokens->length);
//	printf("value = ");
//	print_s_tokens(&info->tokens, b_tokens->ind_tok_start, b_tokens->length);
//	printf("\n");
	while (tokens && i--)
	{
	//	printf("tokens->start = %d\n", tokens->start);
	//	printf("tokens->value = %s\n", tokens->value);
		tokens = tokens->next;
	}
	if (!tokens)
		return (1);
	if (tokens->token == TOK_SEP)
	{
		tokens = tokens->next;
		b_tokens->ind_tok_start++;
		b_tokens->length--;
		if (b_tokens->ind_tok_start == len_ll_list(info->tokens))
		{
			printf("Que des separateurs dans le bat7 (' ')\n");
			return (1);
		}
	}
	return (0);
}

/*t_cmd	*ft_convert_bt_cmd(t_info *info, t_big_token *b_tokens)
{
	t_cmd	*tmp;
	t_token	*tokens;
	int	i;

	i = b_tokens->ind_tok_start;
	tokens = info->tokens;
	while (tokens && i--)
		tokens = tokens->next;
	i = b_tokens->length;
	if (!tokens)
		return (NULL);
	tmp = ft_calloc(sizeof(t_cmd), 1);
	if (!tmp)
		return (NULL);
	while (tokens && i--)
	{
		tmp->cmd = ft_strjoin_free(tmp->cmd, tokens->value, 1);
		tokens = tokens->next;
	}
	if (!tmp->cmd)
		return (free(tmp), NULL);
	tmp->cmd_p = ft_split(tmp->cmd, ' ');
	if (!tmp->cmd_p)
		return (free(tmp->cmd), free(tmp), NULL);
	tmp->envp = ft_env_to_tab(info->env);
	tmp->fdin = 0;
	tmp->fdout = 1;
	return (tmp);
}*/

int	ft_check_builtins(t_info *info, t_big_token *b_tokens)
{
	int	len;

	(void)info;
	len = ft_strlen(b_tokens->cmd_args[0]) + 1;
	if (!ft_strncmp(b_tokens->cmd_args[0], "unset", len))
		return (0);
	else if (!ft_strncmp(b_tokens->cmd_args[0], "export", len))
		return (0);
	else if (!ft_strncmp(b_tokens->cmd_args[0], "cd", len))
		return (0);
	else if (!ft_strncmp(b_tokens->cmd_args[0], "exit", len) || !ft_strncmp(b_tokens->cmd_args[0], "exit ", 5))
		return (0);
	else if (!ft_strncmp(b_tokens->cmd_args[0], "pwd", len))
		return (0);
	else if (!ft_strncmp(b_tokens->cmd_args[0], "env", len))
		return (0);
	else if (!ft_strncmp(b_tokens->cmd_args[0], "echo", len))
		return (0);
	return (1);
}

int	ft_builtins(t_info *info, t_big_token *b_tokens)
{
	int	len;

	len = ft_strlen(b_tokens->cmd_args[0]) + 1;
	if (!ft_strncmp(b_tokens->cmd_args[0], "unset", len))
	{
		if (!ft_unset(info, b_tokens))
			return (1);
		else
			return (0);
	}
	else if (!ft_strncmp(b_tokens->cmd_args[0], "export", len))
		return (ft_export(info, b_tokens));
	else if (!ft_strncmp(b_tokens->cmd_args[0], "cd", len))
		return (ft_cd(info, b_tokens));
	else if (!ft_strncmp(b_tokens->cmd_args[0], "exit", len) || !ft_strncmp(b_tokens->cmd_args[0], "exit ", 5))
		return (ft_exit(info, b_tokens));
	else if (!ft_strncmp(b_tokens->cmd_args[0], "pwd", len))
		return (ft_pwd());
	else if (!ft_strncmp(b_tokens->cmd_args[0], "env", len))
		return (ft_env(info));
	else if (!ft_strncmp(b_tokens->cmd_args[0], "echo", len))
		return (ft_echo(info, b_tokens));
	return (1);
}

int	ft_exit_cmd(t_info *info, t_big_token *b_tokens)
{
	if (b_tokens)
		ft_free_cmd(b_tokens);
	if (info)
		ft_free_all(info, info->env);
	exit (info->status);
}

void	ft_close_cmd(t_info *info, t_big_token *b_tokens, pid_t child)
{
//	(void)child;
	if (b_tokens->type == TOK_LEFT_PIPE)
	{
		if (!info->nb_cmd) 
		{
//			printf("je ferme premier pipex -> value b_token: ");
//			print_s_tokens(&info->tokens, b_tokens->ind_tok_start, b_tokens->length);
//			printf("\n");
			close(info->pdes[1]);
		}
		else //(b_tokens->type == TOK_LEFT_PIPE || (info->parse->child->sibling && info->parse->child->sibling->ind_tok_start != b_tokens->ind_tok_start)))
		{
//			printf("je ferme pipe to pipe -> value b_token: ");
//			print_s_tokens(&info->tokens, b_tokens->ind_tok_start, b_tokens->length);
//			printf("\n");
//			if (info->pdes[0] != 0)
			close(info->pdes[0]);
			info->pdes[0] = info->tmp[0];
	//		if (info->pdes[1] != 1 && info->pdes[1] != 2)
			close(info->pdes[1]);
		}
	}
	else
	{
//		printf("je ferme le dernier pipe -> value b_token: ");
//		print_s_tokens(&info->tokens, b_tokens->ind_tok_start, b_tokens->length);
//		printf("\n");
		waitpid(child, &child, 0);
	//	if (info->pdes[0] != 0)
		if (info->nb_cmd > 1)
		{
			close(info->pdes[1]);
			close(info->pdes[0]);
		}
	}
	if (WIFEXITED(child))
		info->status = WEXITSTATUS(child);
	sc = info->status;
}

int	ft_lead_fd(t_info *info, t_big_token *b_tokens)
{
	if (info->nb_cmd && b_tokens->type == TOK_LEFT_PIPE)
	{
		if (pipe(info->tmp) == -1)
			return (ft_error(5, info, NULL));
		info->pdes[1] = info->tmp[1];
	}
	if (b_tokens->fdin && *b_tokens->fdin != 0)
		info->pdes[0] = b_tokens->fdin[b_tokens->rd_inouthd[0]];
	if (b_tokens->fdout && *b_tokens->fdout != 1)
		info->pdes[1] = b_tokens->fdout[b_tokens->rd_inouthd[1]];
	return (0);
}

int	ft_launch_cmd(t_info *info, t_big_token *b_tokens, int sib_child, int pid)
{
	static int	i = 0;

	pid = -1;
//	cmd = ft_convert_bt_cmd(info, b_tokens);
//	if (!cmd)
//		return (1);
//	printf("tour %d :\npdes[0] = %d && pdes[1] = %d\ntmp[0] = %d && tmp[1] = %d\n", i, info->pdes[0], info->pdes[1], info->tmp[0], info->tmp[1]);
//	printf("tour %d :\npdes[0] = %d && pdes[1] = %d\n", i, info->pdes[0], info->pdes[1]);
	b_tokens->envp = ft_env_to_tab(info->env);
	if (ft_lead_fd(info, b_tokens))
		return (ft_putstr_error("FD problem\n"));;
//	printf("cmd->cmd = %s\n", cmd->cmd);
	if (sib_child == 1 && !ft_check_builtins(info, b_tokens))
		info->status = ft_builtins(info, b_tokens);
	else
	{
		pid = fork();
		if ((int) pid == -1)
			return (ft_error(2, info, NULL));
		else if ((int) pid == 0)
		{
			if (ft_pipex(info, b_tokens, sib_child))
				return (ft_free_cmd(b_tokens), 1);
			ft_exit_cmd(info, b_tokens);
		}
	}
	ft_close_cmd(info, b_tokens, pid);
	i++;
	if (b_tokens->type != TOK_LEFT_PIPE)
		i = 0;
	return (info->status);
}
