/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processes.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eperaita <eperaita@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/28 13:30:38 by eperaita          #+#    #+#             */
/*   Updated: 2022/01/08 14:31:50 by eperaita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//INPUT -> Del input [shell->line] determina n* procesos [shell->my_pro->nbr_process]y splitea line en [shell->my_pro->orders]
//ALLOC_PROCESS -> aloja array de pipes, crea pipes y aloja array de pids de procesos.
//CREATE PROCESSES -> Fork * nbr_process -> redirige cada uno a CHILD_PROCESS
////CHILD PROCESS -> Ejecucion de hija
	//Contamos piquitos
	//Chop churro comando (chop_order.c)
	//Split real_comando (chop_order.c)
	//Redirecciones (exe.c)
	//Unlock siguiente hija
	//Exe (exe.c)
//MOTHER PROCESS -> Ejecucion de madre//
	
//imprimir(shell->my_pro->child);

#include "../include/minishell.h"
#include <signal.h>

//	CHILD PROCESS //

void	child_process(char *order, t_shell *shell)
{
	int     i;

    i = -1;
//	if (tcsetattr(0,TCSANOW, &shell->old) ==-1) 
//		 perror ("ioctl/TCSETA changed:");
    shell->my_pro->child->order = order;
    ///OJO! Y si el comando es ./ ????

    /*CONTAMOS PIQUITOS*/
	count_piquitos(shell->my_pro->child, &shell->my_pro->child->nbr_infile, '<', &shell->my_pro->child->infile_t);
    count_piquitos(shell->my_pro->child, &shell->my_pro->child->nbr_outfile, '>', &shell->my_pro->child->outfile_t);

	/*CHOP EL CHURRO COMANDO */
	chop_order(shell->my_pro->child);
  

  	/*SPLIT EL ARRAY REAL_COMANDO*/
	shell->my_pro->child->command_real = ft_split_2(shell->my_pro->child->comando_bueno, ' ', &shell->my_pro->child->nbr_command); 
	
	/*HACER LAS REDIRECIONES */
	is_redirected(shell->my_pro);
	re_pipe(shell);	
	
	/*BUILTINGS*/
	ft_sort_builtins(shell);
	
	/*DESBLOQUEAR SIGUIENTE HIJA */
	if (shell->my_pro->child->id_child != shell->my_pro->nbr_process - 1)
		kill(shell->my_pro->pid[shell->my_pro->child->id_child], SIGCONT);
	/*EL ULTIMO PASO MANDAMOS A EJECUTAR */
	exe_command(shell);

}

//MOTHER //

void	mother_process(t_shell *shell)
{
	int i;
	int status;
	
	i = -1;
    while (++i < shell->my_pro->nbr_process)
	{
		waitpid(-1, &status, 0);
		if (status != 0)
		{
			printf("Hijos %d con problemas\n", i);
			if (i != (shell->my_pro->nbr_process - 1))
				kill(shell->my_pro->pid[i + 1], SIGCONT);
		}
	}
	//unlink("here_doc.txt");
	//printf("Process OK\n");
}

//CREATE PROCESSES//

void create_processes(t_shell *shell)
{
	shell->my_pro->child->id_child = shell->my_pro->nbr_process;
    while (shell->my_pro->child->id_child--)
    {
        shell->my_pro->pid[shell->my_pro->child->id_child] = fork();
        if (shell->my_pro->pid[shell->my_pro->child->id_child] < 0)
			perror("Error");
		if (shell->my_pro->pid[shell->my_pro->child->id_child] != 0)
			kill(shell->my_pro->pid[shell->my_pro->child->id_child], SIGSTOP);
		else
			child_process(shell->my_pro->orders[shell->my_pro->child->id_child], shell);
    }
	kill(shell->my_pro->pid[0], SIGCONT);
	close_pipes(shell); //Hay que cerrar todas las pipes(bucle)
	mother_process(shell);
}

//ALLOC PROCESSES//

void alloc_processes(t_shell *shell)
{
    int i;
    int npipes;

    i = -1;
    shell->my_pro->orders = ft_split_2(shell->line, '|', &shell->my_pro->nbr_process); //split del input con pipes
    npipes = (shell->my_pro->nbr_process + 1);
    shell->my_pro->fd = (int **)malloc(npipes * sizeof(int *)); //alojo array de pipes
    if (!shell->my_pro->fd)
        exit(1);
    while (++i < npipes)
    {
        shell->my_pro->fd[i] = (int *)malloc(2 * sizeof(int)); //alojo cada pipe
        if (!shell->my_pro->fd[i])
            exit(1);
        if (pipe(shell->my_pro->fd[i]) < 0)      //creo la pipe
        {
            printf("Error pipe\n");
            exit(1);
        }
    }
    i = 0;
    shell->my_pro->pid = (int *)malloc(shell->my_pro->nbr_process * sizeof (int)); //alojo array de hijos
    if (shell->my_pro->pid < 0)
        exit(1);
//	freees?
//	printf("Alloc Processes OK\n");
}

//INPUT STRING//
int input(t_shell *shell)
{
    /*if (shell->line[0] == '|') //  ERROR completar. Ej: Que no nos hayan metido solo |, <<< ...
        exit (1);*/
	unlink("here_doc.txt");
    alloc_processes(shell);
    create_processes(shell);
    return (0);
}
