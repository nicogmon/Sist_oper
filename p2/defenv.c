#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>

enum {
	Maxargs = 5,
	MaxlengthArgs = 4000
};

void
childActions(int argNum, char *envArgs[])
{
	int current_arg = 1;
	int j = 0;
	char *PATH = NULL;
	char *args[Maxargs];
	char *envName;
	char *envValue;

	memset(args, 0, sizeof(args));

	//While to find the first argument without '=' 
	//setting the previous arguments as environment variables
	while (strchr(envArgs[current_arg], '=') != NULL) {

		char *ptr = strchr(envArgs[current_arg], '=');
		int len = ptr - envArgs[current_arg];

		if (len > MaxlengthArgs)
			errx(EXIT_FAILURE,
			     "defenv: error: argument too long: %s",
			     envArgs[current_arg]);

		if (ptr == envArgs[current_arg] || *(ptr + 1) == '\0')
			errx(EXIT_FAILURE,
			     "defenv: error: mising name or value for enviroment variable");

		*ptr = '\0';
		envName = envArgs[current_arg];
		envValue = ptr + 1;

		//printf("envName: %s, envValue: %s\n", envName, envValue);

		setenv(envName, envValue, 1);

		current_arg++;
		if (current_arg == argNum) {
			printf("No path\n");
			printf
			    ("Syntax: ./defenv [env1=val1] [env2=val2] ... [envN=valN] path/program [args]\n");
			exit(EXIT_FAILURE);
		}
	}

	//PATH = argv[current_arg] because current_arg is set at the first argument whithout '=' is the path
	PATH = envArgs[current_arg];

	//Set the last argument to NULL needed by execv
	args[Maxargs - 1] = NULL;

	//Set the rest of the arguments
	while (current_arg < argNum) {
		args[j] = envArgs[current_arg];
		current_arg++;
		j++;
	}

	execv(PATH, args);
	err(EXIT_FAILURE, "defenv: program failed %s", PATH);

}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("No arguments\n");
		printf
		    ("Syntax: ./defenv [env1=val1] [env2=val2] ... [envN=valN] path/program [args]\n");
		exit(EXIT_FAILURE);
	}
	//Fork to execute the program
	pid_t pid = fork();

	if (pid == -1) {
		err(EXIT_FAILURE, "defenv: error forking: %s\n",
		    strerror(errno));
	}
	if (pid == 0) {
		// Proceso hijo: ejecuta el programa
		// estableciendo las variables de entorno
		childActions(argc, argv);

	} else {
		// Proceso padre: espera a que termine el programa
		int wstatus;

		waitpid(pid, &wstatus, WUNTRACED | WCONTINUED);

		if (WIFEXITED(wstatus)) {
			printf("Child exited, status=%d\n",
			       WEXITSTATUS(wstatus));
		} else if (WIFSIGNALED(wstatus)) {
			printf("killed by signal %d\n", WTERMSIG(wstatus));
		}
	}

	exit(EXIT_SUCCESS);
}
