#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<err.h>
#include <string.h>
#include "list.h"

Creat *
createCreat(long long id, char *especie, int altura, int peso, char *color)
{

	Creat *creat = (Creat *) malloc(sizeof(Creat));

	if (pthread_mutex_init(&creat->mutex, NULL) != 0) {
		warnx("cant init mutex");
		return NULL;
	}
	pthread_mutex_lock(&creat->mutex);

	creat->id = id;
	strcpy(creat->especie, especie);
	creat->altura = altura;
	creat->peso = peso;
	strcpy(creat->color, color);
	creat->siguiente = NULL;
	creat->anterior = NULL;
	
	pthread_mutex_unlock(&creat->mutex);
	return creat;
};

Creat *
copyCreat(Creat creat)
{
	pthread_mutex_lock(&creat.mutex);

	Creat *newCreat = (Creat *) malloc(sizeof(Creat));

	pthread_mutex_init(&newCreat->mutex, NULL);
	pthread_mutex_lock(&newCreat->mutex);

	newCreat->id = creat.id;
	strcpy(newCreat->especie, creat.especie);
	newCreat->altura = creat.altura;
	newCreat->peso = creat.peso;
	strcpy(newCreat->color, creat.color);
	newCreat->siguiente = NULL;
	newCreat->anterior = NULL;

	pthread_mutex_unlock(&newCreat->mutex);
	pthread_mutex_unlock(&creat.mutex);
	return newCreat;
};

void
printCreat(Creat * c)
{
	pthread_mutex_lock(&c->mutex);
	printf("------------------------\n");
	printf("Atributos de la criatura:\n");
	printf("Identificador: %lld\nEspecie: %s\n", c->id, c->especie);
	printf("Altura: %d\nPeso: %d\nColor: %s \n", c->altura, c->peso,
	       c->color);
	printf("------------------------\n");
	pthread_mutex_unlock(&c->mutex);
	return;

};

void
freeCreat(Creat * creat)
{
	pthread_mutex_lock(&creat->mutex);
	creat->siguiente = NULL;
	creat->anterior = NULL;
	pthread_mutex_unlock(&creat->mutex);
	pthread_mutex_destroy(&creat->mutex);
	free(creat);
	return;
};

List *
createList(void)
{
	List *list = (List *) malloc(sizeof(List));

	if (pthread_mutex_init(&list->mutex, NULL) != 0) {
		warnx("cant init mutex");
		return NULL;
	}
	pthread_mutex_lock(&list->mutex);

	list->head = NULL;
	list->tail = NULL;

	pthread_mutex_unlock(&list->mutex);

	return list;
};

int
isListEmpty(List * list)
{
	pthread_mutex_lock(&list->mutex);
	if (list->head == NULL) {
		pthread_mutex_unlock(&list->mutex);
		return 1;
	};
	pthread_mutex_unlock(&list->mutex);
	return 0;
};

static int 
islistEmpty(List * list){
	if (list->head == NULL) {
		return 1;
	};
	return 0;
}

void
insertFirst(List * list, Creat * creatPtr)
{
	pthread_mutex_lock(&list->mutex);

	if (!islistEmpty(list)) {
		list->head->anterior = creatPtr;
		creatPtr->siguiente = list->head;
	} else {
		list->tail = creatPtr;
	}
	list->head = creatPtr;

	pthread_mutex_unlock(&list->mutex);
};

void
insertLast(List * list, Creat * creatPtr)
{
	pthread_mutex_lock(&list->mutex);
	Creat *current;

	if (islistEmpty(list)) {
		//fprintf(stdout, "insertando en lista vacia");
		list->head = creatPtr;
		list->tail = creatPtr;
	} else {
		current = list->tail;
		current->siguiente = creatPtr;
		creatPtr->anterior = current;
		list->tail = creatPtr;
	}
	pthread_mutex_unlock(&list->mutex);
};

void
printList(List * list)
{
	pthread_mutex_lock(&list->mutex);
	if (islistEmpty(list)) {
		//printf("La lista esta vacia");
		pthread_mutex_unlock(&list->mutex);
		return;
	}

	for (Creat * current = list->head; current != NULL;
	     current = current->siguiente) {
		printCreat(current);
	}
	pthread_mutex_unlock(&list->mutex);
	return;
};

int
listSize(List * list)
{
	int i = 0;
	Creat *current;

	pthread_mutex_lock(&list->mutex);
	current = list->head;
	while (current) {
		current = current->siguiente;
		i++;
	}
	pthread_mutex_unlock(&list->mutex);
	return i;
};

void
destroyList(List * list)
{

	Creat *current;

	pthread_mutex_lock(&list->mutex);
	if (islistEmpty(list)) {
		//printf("lista vacia\n");
		pthread_mutex_unlock(&list->mutex);
		free(list);
		return;
	}
	while (!islistEmpty(list)) {

		//printf("%lld\n", list->head->id);
		current = list->head;
		list->head = list->head->siguiente;
		freeCreat(current);
	}
	pthread_mutex_unlock(&list->mutex);

	pthread_mutex_destroy(&list->mutex);
	free(list);
}

Creat *
getHead(List * list)
{
	pthread_mutex_lock(&list->mutex);
	printf("head of list: \n");
	pthread_mutex_unlock(&list->mutex);
	return list->head;
};

Creat *
getTail(List * list)
{
	pthread_mutex_lock(&list->mutex);
	printf("tail of list: \n");
	pthread_mutex_unlock(&list->mutex);
	return list->tail;
};

Creat *
getCreat(List * list, long long id)
{
	pthread_mutex_lock(&list->mutex);
	Creat *creat = NULL;
	Creat *current;

	if (islistEmpty(list)) {
		//printf("Lista vacia");
		pthread_mutex_unlock(&list->mutex);
		return NULL;
	} else {
		current = list->head;
		for (current = list->head; current != NULL;
		     current = current->siguiente)
			if (current->id == id) {
				if (creat) {
					printf
					    ("Existe mas de una criatura con este ID\n");
					pthread_mutex_unlock(&list->mutex);
					return NULL;
				}
				creat = current;
			}
	}
	pthread_mutex_unlock(&list->mutex);
	return creat;

}

int
deleteCreat(List * list, Creat * creat)
{
	pthread_mutex_lock(&list->mutex);
	Creat *current;
	Creat *nextCreat;
	Creat *previousCreat;

	if (islistEmpty(list)) {
		//printf("Lista vacia");
		pthread_mutex_unlock(&list->mutex);
		return 0;
	}

	for (current = list->head; current != creat;
	     current = current->siguiente) {
		if (current == NULL) {
			printf("No existe la criatura en la lista\n");
			pthread_mutex_unlock(&list->mutex);
			return 0;
		}
	}
	if (current == list->head) {
		nextCreat = current->siguiente;
		nextCreat->anterior = NULL;
		list->head = nextCreat;
	} else if (current == list->tail) {
		previousCreat = current->anterior;
		previousCreat->siguiente = NULL;
		list->tail = previousCreat;
	} else {
		nextCreat = current->siguiente;
		previousCreat = current->anterior;
		nextCreat->anterior = previousCreat;
		previousCreat->siguiente = nextCreat;
	}
	long long id = current->id;

	freeCreat(current);
	printf("Criatura con identificador %lld eleminida de la colleccion\n",
	       id);
	pthread_mutex_unlock(&list->mutex);
	return 1;
}

Creat *
extractFirst(List * list)
{
	pthread_mutex_lock(&list->mutex);

	Creat *current;
	Creat *nextCreat;

	if (islistEmpty(list)) {
		//printf("Lista vacia\n");
		pthread_mutex_unlock(&list->mutex);
		return NULL;
	}
	current = list->head;
	if (current->siguiente == NULL) {
		list->head = NULL;
		list->tail = NULL;

		pthread_mutex_unlock(&list->mutex);
		return current;
	}
	nextCreat = current->siguiente;
	nextCreat->anterior = NULL;
	list->head = nextCreat;

	pthread_mutex_unlock(&list->mutex);

	return current;
}

Creat *
extractLast(List * list)
{
	pthread_mutex_lock(&list->mutex);

	Creat *current;
	Creat *previousCreat;

	if (islistEmpty(list)) {
		//printf("Lista vacia");
		pthread_mutex_unlock(&list->mutex);
		return NULL;
	}
	current = list->tail;
	previousCreat = current->anterior;
	previousCreat->siguiente = NULL;
	list->tail = previousCreat;

	pthread_mutex_unlock(&list->mutex);

	return current;
}
