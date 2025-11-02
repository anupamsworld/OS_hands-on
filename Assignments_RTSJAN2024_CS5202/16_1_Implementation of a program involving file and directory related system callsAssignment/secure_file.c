/*
WARNING!!!
Be careful about the path.
This program manipulates permission on file(s).
It may cause malfunction of existing software or may changes system configuration
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
void secure_files(const char *dir_path, const char *extension) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    if ((dir = opendir(dir_path)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", dir_path, entry->d_name);

        if (stat(file_path, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISREG(file_stat.st_mode) && strstr(entry->d_name, extension) != NULL) {
            // File is a regular file and has the specified extension
            // Remove read permissions from group and others
            if (chmod(file_path, file_stat.st_mode & ~(S_IRGRP | S_IROTH)) == -1) {
                perror("chmod");
            } else {
                printf("Permissions secured for file: %s\n", file_path);
            }
        } else if (S_ISDIR(file_stat.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // If directory and not .(current directory) or ..(parent directory)
            char subdir_path[PATH_MAX];
            sprintf(subdir_path, "%s/%s", dir_path, entry->d_name);
            secure_files(subdir_path, extension); // Recursively secure files in subdirectory
        }
    }

    closedir(dir);
}
int main(int argc, char *argv[]){
	/* Validation of number of arguments */
	if (argc < 3) {
		printf("\nRequired number of arguments are not mentioned.\nAborting.\n");
		return 1;
	}
	
	char *dir_path=argv[1];
	char *extension=argv[2];
	
	secure_files(dir_path, extension);
	
	return 0;
}
