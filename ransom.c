
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char DIRNAME[] = "./test/";

int main(void) {
    DIR *dir;
    FILE *fd;
    struct dirent *entry;
    char *buf, *filename;
    size_t read, length;
    long int size;

    if ((dir = opendir(DIRNAME)) == NULL) {
        fprintf(stderr, "error opening directory [%s]\n", DIRNAME);
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_REG) {
            continue;
        }

        length = strlen(DIRNAME);

        if ((filename = malloc(length + strlen(entry->d_name) + 1)) == NULL) {
            fprintf(stderr, "memory allocation failed for [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        strncpy(filename, DIRNAME, length);
        strncpy(filename + length, entry->d_name, strlen(entry->d_name));

        filename[length + strlen(entry->d_name)] = 0;

        if ((fd = fopen(filename, "rb")) == NULL) {
            fprintf(stderr, "error opening file [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        fseek(fd, 0, SEEK_SET);

        if ((buf = malloc(size + 1)) == NULL) {
            fprintf(stderr, "memory allocation failed for buffer\n");
            exit(EXIT_FAILURE);
        }

        if ((read = fread(buf, 1, size, fd)) != size) {
            fprintf(stderr, "file read error for [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        if (fclose(fd) == EOF) {
            fprintf(stderr, "error closing file [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        for (size_t i = 0; i < size; ++i) {
            buf[i] ^= 0x42;
        }

        if ((fd = fopen(filename, "wb")) == NULL) {
            fprintf(stderr, "error opening file [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        if (fwrite(buf, 1, size, fd) < size) {
            fprintf(stderr, "error writing to file [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        if (fclose(fd) == EOF) {
            fprintf(stderr, "error closing file [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        free(filename);
        free(buf);
    }

    if (closedir(dir) == -1) {
        fprintf(stderr, "error closing directory [%s]\n", DIRNAME);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
