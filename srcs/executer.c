/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omoudni <omoudni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/09 19:39:38 by omoudni           #+#    #+#             */
/*   Updated: 2022/06/17 17:00:11 by nflan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	sc;

int no_sib_has_child(t_big_token *b_tokens)
{
	t_big_token *tmp_b;

	tmp_b = b_tokens;
	while (tmp_b)
	{
		if (tmp_b->child && tmp_b->sc == -1)
			return (0);
		tmp_b = tmp_b->sibling;
	}
	tmp_b = b_tokens;
	if (tmp_b->type == TOK_CLEAN)
		return (1);
	if (tmp_b->type == TOK_LEFT_OR)
		return (2);
	if (tmp_b->type == TOK_LEFT_AND)
		return (3);
	if (tmp_b->type == TOK_LEFT_PIPE)
		return (4);
	return (0);
}

void	ft_open_all_fdout(t_big_token *b_tokens)
{
	t_fd	*tmp_fd;

	tmp_fd = b_tokens->fd_out;
	if (tmp_fd)
	{
		while (tmp_fd)
		{
			printf("file = %s\n", tmp_fd->file);
			printf("red = %d\n", tmp_fd->red);
			if (!tmp_fd->red)
			{
				printf("allo\n");
				tmp_fd->fd = open(tmp_fd->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				printf("tmp_fd->fd = %d\n", tmp_fd->fd);
			}
			else
				tmp_fd->fd = open(tmp_fd->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (tmp_fd->fd < 0)
			{
				ft_putstr_error("Open error\n");
				b_tokens->sc = 1;
				tmp_fd->fd = 1;
			}
			b_tokens->fdout = tmp_fd->fd;
			tmp_fd = tmp_fd->next;
		}
	}
}

void	ft_open_all_fdin(t_big_token *b_tokens)
{
	t_fd	*tmp_fd;

	tmp_fd = b_tokens->fd_in;
	if (tmp_fd)
	{
		while (tmp_fd)
		{
			printf("file = %s\n", tmp_fd->file);
			printf("red = %d\n", tmp_fd->red);
			if (!tmp_fd->red)
				tmp_fd->fd = open(tmp_fd->file, O_RDONLY);
			if (tmp_fd->fd < 0)
			{
				ft_putstr_error("Open error\n");
				b_tokens->sc = 1;
				tmp_fd->fd = 0;
			}
			b_tokens->fdin = tmp_fd->fd;
			tmp_fd = tmp_fd->next;
		}
	}
}

void	ft_open_fd(t_big_token *b_tokens)
{
	if (b_tokens)
	{
		while (b_tokens)
		{
			if (b_tokens->fd_out)
				ft_open_all_fdout(b_tokens);
			if (b_tokens->fd_in)
				ft_open_all_fdin(b_tokens);
			b_tokens = b_tokens->sibling;
		}
	}
}

void	ft_close_all_fd(t_fd *fd, int fd_type)
{
	if (fd)
	{
		while (fd)
		{
			if (fd_type || (!fd_type && !fd->red))
				if (fd->fd != 0 && fd->fd != 1)
					close(fd->fd);
			fd = fd->next;
		}
	}
}

void	ft_close_fd(t_big_token *b_tokens)
{
	if (b_tokens)
	{
		while (b_tokens)
		{
			if (b_tokens->fd_out)
				ft_close_all_fd(b_tokens->fd_out, 1);
			if (b_tokens->fd_in)
				ft_close_all_fd(b_tokens->fd_in, 0);
			b_tokens = b_tokens->sibling;
		}
	}
}

int	ft_exec_pipex(t_info *info, t_big_token *b_tokens, int *pid)
{
	int	i;

	i = 0;
	while (b_tokens)
	{
		if (ft_wash_btoken(info, b_tokens))
			return (2147483647);
		else if (b_tokens->sc == -1 || b_tokens->sc == 1)
		{
			ft_launch_cmd_pipex(info, b_tokens, pid[i]);
			info->nb_cmd++;
			b_tokens->sc = info->status;
			i++;
		}
		b_tokens = b_tokens->sibling;
	}
	return (0);
}

int	ft_init_pipex(t_info *info, t_big_token *b_tokens)
{
	t_big_token	*tmp_b;
	int	*pid;
	int	i;

	tmp_b = b_tokens;
	i = 0;
	while (tmp_b && ++i)
		tmp_b = tmp_b->sibling;
	pid = ft_calloc(sizeof(int), i);
		if (!pid)
			return (1);
	tmp_b = b_tokens;
	if (pipe(info->pdes) == -1)
		return (ft_error(5, info, NULL));
	if (ft_exec_pipex(info, b_tokens, pid) == 2147483647)
		return (2147483647);
	i = -1;
	while (++i < info->nb_cmd - 1)
		waitpid(pid[i], &pid[i], 0);
	free(pid);
	return (0);
}

int	ft_exec_simple(t_info *info, t_big_token *b_tokens)
{
	t_big_token	*tmp_b;

	tmp_b = b_tokens;
	if (ft_wash_btoken(info, tmp_b))
		return (2147483647);
	if (tmp_b->sc == -1)
	{
		ft_launch_cmd(info, tmp_b);
		tmp_b->sc = info->status;
	}
	return (0);
}

int exec_the_bulk(t_info *info, int sib_child, t_big_token *b_tokens)
{
	info->nb_cmd = 0;
	ft_open_fd(b_tokens);
	if (sib_child >= 1 && sib_child <= 3)
		ft_exec_simple(info, b_tokens);
	else if (sib_child == 4)
		ft_init_pipex(info, b_tokens);
	ft_close_fd(b_tokens);
	return (0);
}

void	give_parent_sc(t_big_token **child, t_big_token **parent)
{
	(*parent)->sc = (*child)->sc;
}

int	rec_exec(t_info *info, t_big_token **b_tokens, int and_or)
{
	t_big_token *tmp_b;
	t_token *tmp_s;
	int i;
	int fc;

	fc = 0;
	i = 0;
	tmp_b = *b_tokens;
	tmp_s = info->tokens;
//	printf("b_tokens->type = %d\n", (*b_tokens)->type);
	while (tmp_b)
	{
		if (tmp_b->child && tmp_b->sc == -1)
		{
			if (!tmp_b->par)
				rec_exec(info, &(tmp_b->child), 0);
		}
		else if ((!i && no_sib_has_child(tmp_b) && tmp_b->parent && tmp_b->sc == -1 && tmp_b->type == TOK_LEFT_PIPE))
		{
//			printf("value b_token si sibling pas child: "); print_s_tokens(&info->tokens, tmp_b->ind_tok_start, tmp_b->length); printf("\n"); printf("je lance sibling sans child !! (start = %d && length = %d)\n", tmp_b->ind_tok_start, tmp_b->length);
			if (exec_the_bulk(info, no_sib_has_child(tmp_b), tmp_b))
				return (1);
			if (tmp_b->parent)
				give_parent_sc(&(tmp_b), &(tmp_b->parent));
			return (0);
		}
		if (tmp_b->type == TOK_PIPE_LAST)
		{
//				printf("value b_token dans le pipe last\n"); print_s_tokens(&info->tokens, tmp_b->ind_tok_start, tmp_b->length); printf("\nFC == 1 && b_tok->sc = %d\n", tmp_b->sc);
			if (exec_the_bulk(info, 4, *b_tokens))
				return (1);
			if (tmp_b->parent)
				give_parent_sc(&(tmp_b), &(tmp_b->parent));
			return (0);
		}
		if (tmp_b->type == TOK_LEFT_AND && i == and_or)
			fc = 1;
		else if (tmp_b->type == TOK_LEFT_OR && i == and_or)
			fc = 2;
		else if (tmp_b->type == TOK_LAST || tmp_b->type == TOK_CLEAN)
			fc = 3;
		if (fc)
			break ;
		tmp_b = tmp_b->sibling;
		i++;
	}
	if (tmp_b && tmp_b->sc == -1)// && !tmp_b->child) //execute le bloc tmp_b tout seul and get the sc;
	{
//		printf("value b_token (commande solo) \n");
//		print_s_tokens(&info->tokens, tmp_b->ind_tok_start, tmp_b->length);
//		printf("\n");
		if (exec_the_bulk(info, 1, tmp_b))
			return (1);
		if (tmp_b->parent)
			give_parent_sc(&(tmp_b), &(tmp_b->parent));
	}
	if (tmp_b && fc > 0 && fc < 3)
	{
		//printf("value b_token dans le FC (%d)\n", fc);
		//print_s_tokens(&info->tokens, tmp_b->ind_tok_start, tmp_b->length);
		//printf("\nb_tok->sc = %d\n", tmp_b->sc);
		if ((fc == 1 && sc == 0) || (fc == 2 && sc))
		{
			rec_exec(info, b_tokens, and_or + 1);
			return (0);
		}
		else if (fc == 2 && !sc)
		{
			while (tmp_b && tmp_b->type != TOK_LEFT_AND)
			{
				tmp_b = tmp_b->sibling;
				and_or++;
			}
		}
		else if (fc == 1 && sc)
		{
			while (tmp_b && tmp_b->type != TOK_LEFT_OR)
			{
				tmp_b = tmp_b->sibling;
				and_or++;
			}
		}
		if (tmp_b)
			rec_exec(info, &(tmp_b), and_or);
	}
//	printf("je pars des parentheses\n");
	return (0);
}
