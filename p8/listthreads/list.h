#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

enum {
	MaxStr = 40
};

struct Creat {
	long long id;
	char especie[MaxStr];
	int altura;
	int peso;
	char color[MaxStr];
	struct Creat *siguiente;
	struct Creat *anterior;
	pthread_mutex_t mutex;
};

typedef struct Creat Creat;

Creat *createCreat(long long id, char *especie, int altura, int peso,
		   char *color);
//Creacion de criatura con los atributos dados, usted tendra que 
//encargarse de liberar la memoria de esta criatura 

Creat *copyCreat(Creat creat);

//Copia una criatura dado el valor de otra, usted tendra que encargarse de liberar la memoria

void printCreat(Creat * c);

void freeCreat(Creat * creat);

struct List {
	Creat *head;
	Creat *tail;
	pthread_mutex_t mutex;
};

typedef struct List List;

List *createList(void);

int isListEmpty(List * list);	

void insertFirst(List * list, Creat * creat);

void insertLast(List * list, Creat * creat);

void printList(List * list);

int listSize(List * list);

void destroyList(List * list);	//Destrución de la lista liberando la memoria propia y de las criaturas que la componen.

Creat *getHead(List * list);

Creat *getTail(List * list);

Creat *getCreat(List * list, long long id);

//Conseguir el puntero de una criatura pasando su identificadosr
//no elimina la criatura de la lista;
//En caso de querer eliminar la criatura de la lista usted
//debe hacerse una copia de elemento y despues utilizar el metodo deleteCreat(list*, Creat*)

int deleteCreat(List * list, Creat * creat);

//Elimina un animal de la lista pasando su puntero

Creat *extractFirst(List * list);

Creat *extractLast(List * list);
