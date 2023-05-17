#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

enum {
	BUFSIZE = 1024
};

int
isNumber(char *str)
{
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] < '0' || str[i] > '9') {
			return 0;
		}
	}
	return 1;
}

int
closeFiles(int file1, int file2)
{
	if (file1 != STDIN_FILENO && close(file1) == -1) {
		perror("Error closing input file");
		exit(EXIT_FAILURE);
	}
	if (file2 != STDOUT_FILENO && close(file2) == -1) {
		perror("Error closing output file");
		exit(EXIT_FAILURE);
	}
	return 0;
}

int
copyBytes(int file1, int file2, long bytesToCopy)
{
	ssize_t bytes_read, bytes_written;
	char buffer[BUFSIZE];

	if (bytesToCopy == 0) {	// Si se lee de la entrada estandar y no se especifican bytes se copiaran bytes hasta encontrar ctrl+D
		while ((bytes_read = read(file1, buffer, BUFSIZE)) > 0) {
			bytes_written = write(file2, buffer, bytes_read);
			if (bytes_written == -1) {
				perror("Error writing to output file");
				exit(EXIT_FAILURE);
			}
		}
	} else {
		while (bytesToCopy > 0 && (bytes_read = read(file1, buffer, BUFSIZE)) > 0) {	// Si se especifican bytes a copiar se copian hasta que se cumplan
			if (bytesToCopy < bytes_read) {
				bytes_read = bytesToCopy;	// Si se leen mas bytes de los especificados para copiar se copian
				// solo los necesarios de la lectura para completar los pedidos
			}

			bytes_written = write(file2, buffer, bytes_read);
			if (bytes_written == -1) {
				perror("Error writing to output file");
				exit(EXIT_FAILURE);
			}
			bytesToCopy -= bytes_written;

		}
	}

	if (bytes_read == -1) {
		perror("Error reading from input file");
		exit(EXIT_FAILURE);
	}

	closeFiles(file1, file2);
	return 0;
}

int
main(int argc, char *argv[])
{
	int file1, file2;

	long bytesToCopy = 0;

	if (argc < 3 || argc > 4) {
		fprintf(stderr, "Usage: file1 file2 [bytes to copy]\n");
		exit(EXIT_FAILURE);
	}
	argv++;
	argc--;

	switch (*argv[0]) {
	case '-':
		file1 = STDIN_FILENO;
		break;
	default:
		file1 = open(argv[0], O_RDONLY);
		if (file1 == -1) {
			perror("open file1");
			exit(EXIT_FAILURE);
		}
		break;
	}

	argv++;
	argc--;

	switch (*argv[0]) {
	case '-':
		file2 = STDOUT_FILENO;
		break;
	default:
		file2 = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0666);	// Crea el archivo si no existe y si existe se trunca 
		if (file2 == -1) {
			perror("open file2");
			exit(EXIT_FAILURE);
		}
		break;
	}

	argv++;
	argc--;

	if (argc == 0) {
		if (file1 != STDIN_FILENO) {
			bytesToCopy = lseek(file1, 0L, SEEK_END);	// Mueve el puntero de archivo al final del archivo
			lseek(file1, 0L, SEEK_SET);	// Mueve el puntero de archivo al inicio del archivo
		}

	} else {
		if (!isNumber(argv[0]) || *argv[0] == '0') {	// Si el argumento no es un numero o es 0
.			fprintf(stderr,
				"Bytes to copy must be a integer greater than 0\n");
			fprintf(stderr, "Usage: file1 file2 [bytes to copy]");
			exit(EXIT_FAILURE);
		}
		bytesToCopy = atoi(argv[0]);

	}

	copyBytes(file1, file2, bytesToCopy);	// Copia los bytes especificados de file1 a file2

	exit(EXIT_FAILURE);
}
