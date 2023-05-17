#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "listcreat.h"

Creat *
createCreat(long long id, char *especie, int altura, int peso, char *color)
{
	Creat *creat = (Creat *) malloc(sizeof(Creat));

	creat->id = id;
	strcpy(creat->especie, especie);
	creat->altura = altura;
	creat->peso = peso;
	strcpy(creat->color, color);
	creat->siguiente = NULL;
	creat->anterior = NULL;
	return creat;
};

Creat *
copyCreat(Creat creat)
{
	Creat *newCreat = (Creat *) malloc(sizeof(Creat));

	newCreat->id = creat.id;
	strcpy(newCreat->especie, creat.especie);
	newCreat->altura = creat.altura;
	newCreat->peso = creat.peso;
	strcpy(newCreat->color, creat.color);
	newCreat->siguiente = NULL;
	newCreat->anterior = NULL;
	return newCreat;
};

void
printCreat(Creat * c)
{
	printf("------------------------\n");
	printf("Atributos de la criatura:\n");
	printf("Identificador: %lld\nEspecie: %s\n", c->id, c->especie);
	printf("Altura: %d\nPeso: %d\nColor: %s \n", c->altura, c->peso,
	       c->color);
	printf("------------------------\n");
};

void
freeCreat(Creat * creat)
{
	creat->siguiente = NULL;
	creat->anterior = NULL;
	free(creat);
};

List *
createList(void)
{
	List *list = (List *) malloc(sizeof(List));

	list->head = NULL;
	list->tail = NULL;

	return list;
};

int
isListEmpty(List * list)
{
	if (list->head == NULL) {
		return 1;
	};
	return 0;
};

void
insertFirst(List * list, Creat * creatPtr)
{

	//*creatPtr = *creat;

	if (!isListEmpty(list)) {
		//printf("list no vacia");
		list->head->anterior = creatPtr;
		creatPtr->siguiente = list->head;
	}
	if (isListEmpty(list)) {
		list->tail = creatPtr;
	}
	list->head = creatPtr;
	//printCreat(&list->head->creat);
};

void
insertLast(List * list, Creat * creatPtr)
{
	Creat *current;

	if (isListEmpty(list)) {
		list->head = creatPtr;
	} else {
		current = list->head;

		while (current->siguiente) {
			current = current->siguiente;
		}
		current->siguiente = creatPtr;
		creatPtr->anterior = current;
		list->tail = creatPtr;

	}
};

void
printList(List * list)
{
	if (isListEmpty(list)) {
		printf("La lista esta vacia");
		return;
	}

	for (Creat * current = list->head; current != NULL;
	     current = current->siguiente) {
		printCreat(current);
	}
	return;
};

int
listSize(List * list)
{
	int i = 0;
	Creat *current;

	current = list->head;
	while (current) {
		current = current->siguiente;
		i++;
	}
	return i;
};

void
destroyList(List * list)
{
	Creat *current;

	if (isListEmpty(list)) {
		printf("lista vacia\n");
		return;
	}
	while (!isListEmpty(list)) {
		//printf("entra a destructor");
		//printf("%lld\n", list->head->id);
		current = list->head;
		list->head = list->head->siguiente;
		freeCreat(current);
	}
	free(list);
}

Creat *
getHead(List * list)
{
	printf("head of list: \n");
	return list->head;
};

Creat *
getTail(List * list)
{
	printf("tail of list: \n");
	return list->tail;
};

Creat *
getCreat(List * list, long long id)
{
	Creat *creat = NULL;
	Creat *current;

	if (isListEmpty(list)) {
		printf("Lista vacia");
		return NULL;
	} else {
		current = list->head;
		for (current = list->head; current != NULL;
		     current = current->siguiente)
			if (current->id == id) {
				if (creat) {
					printf
					    ("Existe mas de una criatura con este ID\n");
					return NULL;
				}
				creat = current;
			}
	}
	return creat;

}

int
deleteCreat(List * list, Creat * creat)
{
	Creat *current;
	Creat *nextCreat;
	Creat *previousCreat;

	if (isListEmpty(list)) {
		printf("Lista vacia");
		return 0;
	}

	for (current = list->head; current != creat;
	     current = current->siguiente) {
		if (current == NULL) {
			printf("No existe la criatura en la lista\n");
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
	return 1;
}

Queue *
createQueue(void)
{
	Queue *queue = (Queue *) malloc(sizeof(Queue));

	queue->list = createList();
	return queue;
}

void
enqueue(Queue * queue, Creat * creat)
{
	insertLast(queue->list, creat);
}

Creat *
dequeue(Queue * queue)
{
	Creat *creatPtr = (Creat *) malloc(sizeof(Creat));

	*creatPtr = *getHead(queue->list);
	deleteCreat(queue->list, queue->list->head);
	return creatPtr;
}

int
isQueueEmpty(Queue * queue)
{
	return isListEmpty(queue->list);
}

void
printQueue(Queue * queue)
{
	printf("La cola contiene las siguientes criaturas: \n");
	printList(queue->list);
	return;
}

void
destroyQueue(Queue * queue)
{
	destroyList(queue->list);
	free(queue);
	return;
}
