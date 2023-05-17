#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

enum {
	CFILE = 1,
	HFILE = 2,
	OTHER = 3,
	BAD_DIR = 4,
	BAD_FILE = 5
};

void
printResults(char *path, int Cfiles, int Hfiles, int bytes)
{
	printf("%s  %d  %d  %d\n", path, Cfiles, Hfiles, bytes);

}

int
FileType(char *filename)
{
	int len = strlen(filename);

	if (len >= 3 && filename[len - 2] == '.' && filename[len - 1] == 'c') {
		return CFILE;
	}

	if (len >= 3 && filename[len - 2] == '.' && filename[len - 1] == 'h') {
		return HFILE;
	}
	return OTHER;
}

int
countBytes(char *filename, int *bytes)
{
	struct stat st = { 0 };

	if (stat(filename, &st) == -1) {
		printf("Error al obtener información del archivo %s\n",
		       filename);
		return BAD_FILE;
	}

	*bytes += st.st_size;

	return 1;
}

int
count(char *path, int *Cfiles, int *Hfiles, int *bytes)
{
	int status = 0;
	DIR *dir = opendir(path);
	struct dirent *ent;
	char filename[300];

	if (dir == NULL) {
		return BAD_DIR;
	}
	
	chdir(path);
	while ((ent = readdir(dir)) != NULL) {

		if (strcmp(ent->d_name, ".") == 0
		    || strcmp(ent->d_name, "..") == 0)
			continue;

		if (ent->d_type == DT_DIR) {
			strcpy(filename, ent->d_name);

			count(filename, Cfiles, Hfiles, bytes);
			chdir("..");

		} else {

			strcpy(filename, ent->d_name);
			switch (FileType(filename)) {
			case CFILE:
				*Cfiles += 1;
				countBytes(filename, bytes);
				break;
			case HFILE:
				*Hfiles += 1;
				countBytes(filename, bytes);
				break;
			default:
				break;
			}

		}
	}

	closedir(dir);
	return status;
};

int
main(int argc, char *argv[])
{
	int status = 0;
	char *path = (char *)malloc(sizeof(char) * 100);

	argc--;
	argv++;

	while (fgets(path, 100, stdin) != NULL) {	//Lee paths por la entrada estandar 
		int Cfiles = 0;
		int Hfiles = 0;
		int bytes = 0;

		path[strlen(path) - 1] = '\0';
		status = count(path, &Cfiles, &Hfiles, &bytes);
		if (status == BAD_DIR) {
			printf("Error: Directory: %s not found\n", path);
			continue;
		}
		printResults(path, Cfiles, Hfiles, bytes);
	}

	free(path);

	exit(status);
}
