#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

enum {
	COMMANDSIZE = 50,
	MAXARGS = 10
};

void
usage()
{
	fprintf(stderr,
		"Usage: ./pipeline 'command1 arguments'  'command2 arguments' 'command3 arguments'");
}

int
freeFileDescripts(int *fds[], int numberOfFds)
{
	for (int i = 0; i < numberOfFds; i++) {
		free(fds[i]);
	}
	return 0;
}

int
configExec(char *rest, char *program, char *args[], int arrPos)
{
	char *paths[2] = { "/bin/", "/usr/bin/" };
	char *delimiter = " ";
	char *token = (char *)malloc(sizeof(char) * 100);
	int x = 1;

	token = strtok_r(rest, delimiter, &rest);

	memset(program, 0, COMMANDSIZE);
	strcpy(program, paths[arrPos]);
	strcat(program, token);
	args[0] = program;
	args[MAXARGS - 1] = NULL;

	while ((token = strtok_r(rest, delimiter, &rest))) {
		args[x] = token;
		x++;
	}
	free(token);
	return 0;
}

int
FDescriptsConfig(int childNumber, int *fileDescriptors[])
{

	switch (childNumber) {
	case 0:
		close(fileDescriptors[1][0]);
		close(fileDescriptors[1][1]);
		close(fileDescriptors[0][0]);

		if (dup2(fileDescriptors[0][1], STDOUT_FILENO) == -1) {
			err(EXIT_FAILURE, "Error case 0 dup[0][1]: %s\n",
			    strerror(errno));
		}
		close(fileDescriptors[0][1]);
		break;

	case 1:
		close(fileDescriptors[0][1]);
		close(fileDescriptors[1][0]);

		if (dup2(fileDescriptors[0][0], STDIN_FILENO) == -1) {
			err(EXIT_FAILURE, "Error case1 dup[0][1]: %s\n",
			    strerror(errno));
		}
		if (dup2(fileDescriptors[1][1], STDOUT_FILENO) == -1) {
			err(EXIT_FAILURE, "Error case1 dup2[1][0]: %s\n",
			    strerror(errno));
		}
		close(fileDescriptors[0][0]);
		close(fileDescriptors[1][1]);

		break;

	case 2:
		close(fileDescriptors[0][0]);
		close(fileDescriptors[0][1]);
		close(fileDescriptors[1][1]);
		if (dup2(fileDescriptors[1][0], STDIN_FILENO) == -1) {
			err(EXIT_FAILURE, "Error case 2 dup[1][1]: %s\n",
			    strerror(errno));
		}
		close(fileDescriptors[1][0]);

		break;
	default:
		break;
	}
	return 0;
}

int
executeCommand(char *command, int childNumber, int *fileDescriptors[])
{

	char *rest = (char *)malloc(sizeof(char) * 100);
	char program[COMMANDSIZE] = { 0 };
	char *args[MAXARGS] = { 0 };
	pid_t childPid;

	childPid = fork();

	if (childPid == -1) {
		err(EXIT_FAILURE, "pipeline: error forking: %s\n",
		    strerror(errno));
	}
	if (childPid == 0) {
		FDescriptsConfig(childNumber, &fileDescriptors[0]);
		rest = command;

		for (int i = 0; i < 2; i++) {
			configExec(rest, &program[0], &args[0], i);
			execv(program, args);
			fprintf(stderr, "Failed to execute %s\n", program);

		}

		exit(EXIT_FAILURE);
	}

	if (childPid != 0) {
		for (int i = 0; i < childNumber; i++) {
			close(fileDescriptors[i][0]);
			close(fileDescriptors[i][1]);
		}
		free(rest);
	}
	return childPid;

}

int
main(int argc, char *argv[])
{
	pid_t childPid;
	pid_t lastPid;
	int wstatus;
	int exitStatus;

	if (argc != 4) {
		usage();
	}

	argc--;
	argv++;

	int *fds[2];

	for (int i = 0; i < argc - 1; i++) {
		int *fd = (int *)malloc(sizeof(int) * 2);

		if (pipe(fd) == -1) {
			err(EXIT_FAILURE, "defenv: error creating pipe: %s\n",
			    strerror(errno));
		}
		fds[i] = fd;
	}

	for (int i = 0; i < argc; i++, argv++) {
		childPid = executeCommand(argv[0], i, &fds[0]);
	}

	while ((lastPid = wait(&wstatus)) != -1) {
		if (lastPid == childPid) {	//chilpid is the last child because it is the last one to be executed
			exitStatus = wstatus;
		}

	};

	freeFileDescripts(&fds[0], argc - 1);

	if (WIFEXITED(exitStatus)) {
		exit(WEXITSTATUS(exitStatus));
	} else if (WIFSIGNALED(exitStatus)) {
		printf("killed by signal %d\n", WTERMSIG(exitStatus));
	}
}
