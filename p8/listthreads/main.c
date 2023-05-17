#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<err.h>
#include "list.h"

enum {

	Nthreads = 100,

	Ncreats = 100,
};

int counter;

List *list;

void *
threadActions(void *arg)
{
	long long *xid = (long long *)arg;

	long long id = *xid;
	int criaturas = 0;

	for (int x = 0; x < Ncreats; x++) {
		
		Creat *creat = createCreat(id, "especie", 1, 1, "color");

		insertLast(list, creat);
		id++;

	}
	for (int x = 0; x < 40; x++) {

		Creat *creat = extractFirst(list);

		if (creat->id < *xid || creat->id > *xid + Ncreats) {
			criaturas++;
		}
		freeCreat(creat);

	}
	printf("Criaturas extraidas por hilo %lld de hilos no propios: %d\n",
	       *xid / 100, criaturas);

	free(xid);

	return NULL;
}

int
main(int argc, char *argv[])
{

	pthread_t thread[Nthreads];
	long long *id;
	int ids[Nthreads];

	for (int i = 0; i < Nthreads; i++) {
		ids[i] = i;
	}

	list = createList();

	for (int i = 0; i < Nthreads; i++) {
		id = malloc(sizeof(long long));
		*id = (Ncreats * i);

		if (pthread_create(&thread[i], NULL, threadActions, id) != 0) {

			warnx("error creating thread");

			return 1;

		}
	}

	for (int i = 0; i < Nthreads; i++) {

		if (pthread_join(thread[i], NULL) != 0) {

			warnx("error joining thread");

			return 1;
		}
	}

	pthread_mutex_destroy(&list->mutex);

	int listsize = listSize(list);

	fprintf(stderr, "list size: %d\n", listsize);

	for (int i = 0; i < listsize; i++) {
		Creat *creat = extractFirst(list);

		
		int hilo = creat->id / 100;

		if (ids[hilo] > creat->id) {
			printf("Criatura con id %lld en orden erroneo",
			       creat->id);
			freeCreat(creat);
			break;
		}
		ids[creat->id / 100] = creat->id;

		freeCreat(creat);
	}

	destroyList(list);

	return 0;
}
