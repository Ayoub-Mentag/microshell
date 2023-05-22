#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct s_info {
    char **cmd;
    struct s_info *next;
}   t_info;

int ft_strlen(char *s)
{
    int i = 0;
    while (s && s[i])
        i++;
    return (i);
}

int ft_strcmp(char *s1, char *s2)
{
    int i = 0;
    while (s1[i] && s2[i] && s1[i] == s2[i])
        i++;
    return (s1[i] - s2[i]);
}

char *ft_strdup(char *s)
{
    int i = 0;
    int len = ft_strlen(s);
    char *result= malloc(len + 1);
    if (!result)
        return (NULL);
    while (i < len)
    {
        result[i] = s[i];
        i++;
    }
    result[i] = 0;
    return (result);
}

char **get_cmd(char **av, int i)
{
    int j = 0;
    char **cmd = malloc((i + 1) * sizeof(char *));
    if (!cmd)
        return (NULL);
    while (j < i)
    {
        cmd[j] = ft_strdup(av[j]);
        j++;
    }
    cmd[j] = NULL;
    return (cmd);
}

char  **parsing(char **av)
{
    char **cmd;
    int j = 0;

    while (av[j] && (ft_strcmp(av[j], ";") && ft_strcmp(av[j], "|")))
        j++;
    cmd = get_cmd(av, j);
    return (cmd);
}

void    free_head(t_info **head)
{
    int i = 0;

    while ((*head)->cmd[i])
    {
        free((*head)->cmd[i]);
        i++;
    }
    free((*head)->cmd);
}


void    ft_putstrfd(int fd, char *s)
{
    if (!s)
        return ;
    write(fd, s, ft_strlen(s));
}

void    execute(char **cmd,  char **env, int *fd, int old)
{
    int id = fork();
    // printf("%s %s %s %d %d %d\n", cmd[0], cmd[1], cmd[2], fd[0], fd[1], old);
    if (id == 0)
    {
        close(fd[0]);
        if (old != -1)
        {
            // printf("reading %s %d\n",cmd[0], old);
            dup2(old, STDIN_FILENO);
            close(old);
        }
        if (fd[1] != -1)
        {
            // printf("writing %s %d\n",cmd[0], fd[1]);
            dup2(fd[1], STDOUT_FILENO);
        }
        close(fd[1]);
        execve(cmd[0], cmd, env);
        ft_putstrfd(2, "error: cannot execute ");
        ft_putstrfd(2, cmd[0]);
        ft_putstrfd(2, "\n");
    }
    else
    {
        close(fd[1]);
        close(old);
    }
}

void    execute_cd(char **cmd)
{
    if (!cmd[0] || cmd[1])
        return (ft_putstrfd(2, "error: cd: bad arguments\n"));
    if (chdir(cmd[0]))
        return (ft_putstrfd(2, "error: cd: cannot change directory to "), ft_putstrfd(2, cmd[0]), ft_putstrfd(2, "\n"));
}

void    free_cmd(char **cmd)
{
    int i = 0;
    char *tmp = cmd[i];
    while (cmd[i])
    {
        free(tmp);
        i++;
        tmp = cmd[i];
    }
    free(cmd);
}

int main(int ac, char **av, char **env)
{

    int fd[2];
    fd[0] = -1;
    fd[1] = -1;
    int old;
    if (ac == 1)
        return (1);
    int i = 1;
    while (av[i])
    {
        char **cmd = parsing(av + i);
        // printf("%s %s %s\n", cmd[0], cmd[1], cmd[2]);
        //skip the cmds till | or ;
        while (av[i] && (ft_strcmp(av[i], ";") && ft_strcmp(av[i], "|")))
            i++;
        old = fd[0];
        if (av[i] && ft_strcmp(av[i], "|") == 0)
        {
            pipe(fd);
        }
        else
        {
            fd[0] = -1;
            fd[1] = -1;
        }

        if (cmd[0])
        {
            if (ft_strcmp(cmd[0], "cd") == 0)
                execute_cd(cmd + 1);
            else
            {
                execute(cmd, env, fd, old);
                if (!av[i] || ft_strcmp(av[i], ";") == 0)
                {
                    while (waitpid(-1, NULL, 0) > 0);
                    close(fd[0]);
                }
            }
        }
        // //skip ; or |
        if (av[i])
            i++;
        // free_cmd(cmd);
    }
    return (0);
}
