#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>

enum {
	MAXARGS = 10,
	BUFSIZE = 100,
	COMMANDSIZE = 50
};

void
usage(void)
{
	fprintf(stderr, "Usage: filterout regex command [args...]\n");
}

void
childActions(int argc, char *argv[])
{
	char command[COMMANDSIZE] = { 0 };
	char *paths[2] = { "/bin/", "/usr/bin/" };
	char *args[MAXARGS] = { 0 };

	for (int x = 0; x < 2; x++) {
		memset(command, 0, sizeof(command));
		strcpy(command, paths[x]);
		strcat(command, argv[0]);

		args[0] = command;
		args[MAXARGS - 1] = NULL;

		for (int i = 1; i < argc; i++) {
			args[i] = argv[i];
		}

		execv(command, args);
		fprintf(stderr, "Error: command  %s not found in %s: \n",
			argv[0], paths[x]);

		if (x == 1) {
			err(EXIT_FAILURE, "defenv: error executing %s: %s\n",
			    command, strerror(errno));
		}
	}
}

int
grep_fork(pid_t * grep_pid, int *fd, char *regex)
{
	*grep_pid = fork();

	if (*grep_pid == -1) {
		err(EXIT_FAILURE, "defenv: error forking: %s\n",
		    strerror(errno));
	}
	if (*grep_pid == 0) {
		// Proceso hijo: ejecuta el commanda
		close(fd[1]);
		if (dup2(fd[0], STDIN_FILENO) == -1) {
			err(EXIT_FAILURE, "Error dup2: %s\n", strerror(errno));
		}
		close(fd[0]);
		execl("/usr/bin/grep", "/usr/bin/grep", regex, NULL);

		err(EXIT_FAILURE, "Error executing Grep%s\n", strerror(errno));

	}

	return *grep_pid;
}

int
command_fork(pid_t * command_pid, int *fd, int argc, char *argv[])
{

	//Fork to execute the comando
	*command_pid = fork();

	if (*command_pid == -1) {
		err(EXIT_FAILURE, "defenv: error forking: %s\n",
		    strerror(errno));
	}
	if (*command_pid == 0) {
		// Proceso hijo: ejecuta el comando
		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO) == -1) {
			err(EXIT_FAILURE, "Error dup2: %s\n", strerror(errno));
		}
		close(fd[1]);
		childActions(argc, argv);
	}

	return *command_pid;

}

int
main(int argc, char *argv[])
{
	int fd[2];
	pid_t pid_command, pid_grep;
	char *regex = (char *)malloc(sizeof(char) * 100);

	if (argc < 3) {
		usage();
		exit(EXIT_FAILURE);
	}
	argc--;
	argv++;

	strcpy(regex, argv[0]);

	argc--;
	argv++;

	if (pipe(fd) == -1) {
		err(EXIT_FAILURE, "defenv: error creating pipe: %s\n",
		    strerror(errno));
	}

	command_fork(&pid_command, &fd[0], argc, argv);

	grep_fork(&pid_grep, &fd[0], regex);

	close(fd[0]);
	close(fd[1]);

	free(regex);
	// Proceso padre: espera a que termine el commanda
	int wstatus_grep, wstatus_command;

	waitpid(pid_command, &wstatus_command, WUNTRACED | WCONTINUED);
	waitpid(pid_grep, &wstatus_grep, WUNTRACED | WCONTINUED);

	if (WIFEXITED(wstatus_command) && WEXITSTATUS(wstatus_grep) == 0) {
		exit(EXIT_SUCCESS);
	} else if (WIFSIGNALED(wstatus_command || WIFSIGNALED(wstatus_grep))) {
		printf("killed by signal %d\n", WTERMSIG(wstatus_command)
		       || WTERMSIG(wstatus_grep));
	}
	exit(EXIT_FAILURE);
}
