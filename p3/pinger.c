#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <err.h>

enum {
	MaxUrls = 200,
	MaxUrlsize = 100
};

struct Identifiyer {
	pid_t pid;
	char url[MaxUrlsize];
};

typedef struct Identifiyer Identifiyer;

void
waitChildren(Identifiyer * identifiyersArr)
{
	int wstatus;

	pid_t pid_hijo = wait(&wstatus);

	for (int i = 0; i < MaxUrls; i++) {
		if (pid_hijo == identifiyersArr[i].pid) {
			printf("\nNo existe: %s\n", identifiyersArr[i].url);
		}
	}

	if (WIFEXITED(wstatus)) {
		printf("Child exited, status=%d\n", WEXITSTATUS(wstatus));
		if (WEXITSTATUS(wstatus) != 0) {
			exit(EXIT_FAILURE);
		}

	} else if (WIFSIGNALED(wstatus)) {
		printf("killed by signal %d\n", WTERMSIG(wstatus));

	}
}

void
childActions(char *time_response, char *url, int posicion)
{
	char PATH[MaxUrlsize] = { "/usr/bin/ping" };

	execl(PATH, PATH, "-c", "2", "-W", time_response, "-q", url, NULL);
	err(EXIT_FAILURE, "defenv: program failed %s", PATH);

}

void
dofork(int argc, char *argv[], char *time_response)
{

	pid_t pid;

	Identifiyer identifiyersArr[MaxUrls];

	memset(identifiyersArr, 0, sizeof(identifiyersArr));

	for (int i = 2; i < argc; i++) {
		pid = fork();

		switch (pid) {
		case -1:
			err(EXIT_FAILURE, "defenv: error forking: %s\n",
			    strerror(errno));
			break;
		case 0:
			// Proceso hijo: ejecuta el programa
			childActions(time_response, argv[i], i);
		default:
			Identifiyer identifiyer;

			identifiyer.pid = pid;
			strcpy(identifiyer.url, argv[i]);
			identifiyersArr[i - 2] = identifiyer;

			break;
		}
	}

	waitChildren(identifiyersArr);

}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Needed more arguments\n");
		printf("Syntax: ./pinger response_time url1 url2....\n");
		exit(EXIT_FAILURE);
	}

	if (atoi(argv[1]) == 0) {
		printf("Error: response_time must be a number\n");
		printf("Syntax: ./pinger response_time url1 url2....\n");
		exit(EXIT_FAILURE);
	}
	if (argc > MaxUrls + 2) {
		printf("Error: Max number of urls is %d\n", MaxUrls);
		exit(EXIT_FAILURE);
	}

	char time_response[10];

	strcpy(time_response, argv[1]);
	dofork(argc, argv, time_response);

	exit(EXIT_FAILURE);
}
