#include <stdlib.h>
#include <stdio.h>
#include "listcreat.h"
enum {
	MaxCreatures = 10
};

//inicalizacion de datos de las criaturas
Creat creatures[] = {
	{1, "Dog", 450, 5000, "Golden"},
	{2, "Cat", 250, 3000, "White"},
	{3, "Mouse", 25, 100, "Gray"},
	{4, "Horse", 1500, 10000, "Black"},
	{5, "Cow", 1250, 50000, "Red"},
	{6, "Pig", 500, 30000, "Pink"},
	{7, "Turtle", 750, 1560, "Green"},
	{8, "Bird", 100, 200, "Yellow"},
	{9, "Fish", 100, 100, "Blue"},
	{10, "Bear", 250, 3000, "Brown"}
};

List *
listCreation()
{				//creacion de lista creando las criaturas necesarias e insertandolas en la lista.

	List *list = createList();

	for (int i = 0; i < sizeof(creatures) / sizeof(Creat); i++) {
		Creat *creatPtr = (Creat *) malloc(sizeof(Creat));

		*creatPtr = creatures[i];
		insertLast(list, creatPtr);
	};
	return list;
};

Queue *
queueCreation()
{				//creacion de la cola creando las criaturas necesarias e insertandolas en la cola.
	Queue *queue = createQueue();

	for (int i = 0; i < sizeof(creatures) / sizeof(Creat); i++) {
		Creat *creatPtr = (Creat *) malloc(sizeof(Creat));

		*creatPtr = creatures[i];
		enqueue(queue, creatPtr);
	};
	return queue;
};

//La memoria de las criaturas se liberara al llamar a las funciones destroyList y destroyQueue de listcreat.c
//liberando tambien la memoria de la lista y cola respectivamente.
int
main(int argc, char *argv[])
{

	Creat *current = NULL;
	Creat *next = NULL;
	int i = 1;
	List *list = listCreation();

	printf("La lista contiene las siguientes criaturas : \n");
	printList(list);

	for (current = list->head; current != NULL; current = next, i++) {
		next = current->siguiente;
		if (i % 2 == 0) {
			deleteCreat(list, current);
		}

	};

	printf("La lista contiene las siguientes criaturas : \n");
	printList(list);

	destroyList(list);

	printf("FIN ACCIONES LISTA\n\n");

	Queue *queue = queueCreation();

	printQueue(queue);

	for (int x = 0; x < 5; x++) {
		printf("Criatura %d:\n", x + 1);
		Creat *c = dequeue(queue);

		printCreat(c);
		free(c);
	}

	printf("\n\n");
	printQueue(queue);

	destroyQueue(queue);
}
