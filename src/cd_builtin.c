/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eperaita <eperaita@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/19 17:12:52 by eperaita          #+#    #+#             */
/*   Updated: 2022/01/19 18:56:22 by eperaita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

//////////////////CD///////////////////////////
static char	*special_paths(char **env, char *command)
{
	int		a;
	char	*temp;
	char	*find;

	if (ft_strcmp(command, "-"))
		find = "OLD_PWD=";
	else if (command[0] == '~')
		find = "HOME=";
	a = -1;
	while (env[++a] && (ft_strcmp(command, "-") || command[0] == '~'))
	{
		if (!ft_strncmp(env[a], find, ft_strlen(find)))
		{
			if (command[0] == '~')
				temp = ft_strdup(&command[1]);
			else
				temp = (char *)ft_calloc(1, 1);
			free(command);
			command = ft_strjoin(ft_strdup(&env[a][ft_strlen(find)]), temp);
			free(temp);
			break ;
		}
	}
	return (command);
}

static char	**ft_print_return(int err)
{
	ft_putstr_fd("Pink peanuts:", 2);
	ft_putstr_fd(strerror(err), 2);
	ft_putstr_fd("\n", 2);
	return (NULL);
}

static int	cd_builtin_error(char *command_split, int err)
{
	if (command_split[0] == '-')
	{
		ft_putstr_fd("Pink peanuts: OLDPWD not set\n", 2);
		return (1);
	}
	if (chdir(command_split) != 0)
	{
		ft_print_return(err);
		return (1);
	}
	return (0);
}

char	**cd_builtin(char **env, char *command_split, char **new_vars)
{
	int		a;
	char	path[1024];

	new_vars[0] = ft_strjoin("OLD_PWD=", ft_strdup(getcwd(path, 1024)));
	if (!command_split)
	{
		a = -1;
		while (env[++a])
		{
			if (!ft_strncmp(env[a], "HOME=", ft_strlen("HOME=")))
			{
				if (chdir(&env[a][5]) != 0)
					return (ft_print_return(errno));
			}
		}
	}
	else
	{
		command_split = special_paths(env, command_split);
		if (cd_builtin_error(command_split, errno))
			return (NULL);
	}
	new_vars[1] = ft_strjoin("PWD=", ft_strdup(getcwd(path, 1024)));
	new_vars[2] = NULL;
	return (new_vars);
}
