/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omoudni <omoudni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 11:39:37 by nflan             #+#    #+#             */
/*   Updated: 2022/07/24 18:43:15 by nflan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	g_sc;

void	ft_end(t_info *info)
{
	ft_free_all(info, info->env);
	ft_exit(info, NULL);
}

int	ft_launch_minishell(t_info info, char *word)
{
	rl_outstream = stderr;
	while (1)
	{
		word = ft_rdline_word(&info);
		if (!word)
			return (ft_free_all(&info, info.env),
				ft_putstr_error("Error while reading readline\n"));
		ft_keep_history(&info, word);
		if (!info.rdline)
			break ;
		if (info.rdline[0] && !ft_init_info(&info))
		{
			if (info.nb_cmd != 10)
				rec_exec(&info, &info.parse, 0);
			ft_free_all(&info, NULL);
		}
		g_sc = 0;
	}
	ft_end(&info);
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	t_info		info;
	char		*word;

	(void) av;
	word = NULL;
	g_sc = 0;
	if (ac > 1)
		return (ft_first_error(av[1]));
	if (ft_init_first(&info, envp))
		return (ft_putstr_error("Error!\n"));
	return (ft_launch_minishell(info, word));
}
