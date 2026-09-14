/*
  source_archive_lifetime.c -- verify derived sources are invalidated with their archive
  Copyright (C) 2026

  This file is part of libzip, a library to manipulate ZIP archives.
*/

#include "config.h"

#include <stdio.h>

#include "zip.h"


int main(void) {
    char byte;
    int error_code;
    zip_error_t error;
    zip_source_t *source;
    zip_t *archive;

    error_code = 0;
    archive = zip_open("testdeflated.zip", 0, &error_code);
    if (archive == NULL) {
        zip_error_t open_error;

        zip_error_init_with_code(&open_error, error_code);
        fprintf(stderr, "can't open test archive: %s\n", zip_error_strerror(&open_error));
        zip_error_fini(&open_error);
        return 1;
    }

    zip_error_init(&error);
    source = zip_source_zip_file_create(archive, 0, 0, 1, 32, NULL, &error);
    if (source == NULL) {
        fprintf(stderr, "can't create partial source: %s\n", zip_error_strerror(&error));
        zip_error_fini(&error);
        zip_discard(archive);
        return 1;
    }
    if (zip_source_open(source) < 0) {
        fprintf(stderr, "can't open partial source: %s\n", zip_error_strerror(zip_source_error(source)));
        zip_source_free(source);
        zip_error_fini(&error);
        zip_discard(archive);
        return 1;
    }

    zip_discard(archive);
    if (zip_source_read(source, &byte, 1) != -1 || zip_error_code_zip(zip_source_error(source)) != ZIP_ER_ZIPCLOSED) {
        fprintf(stderr, "partial source was not invalidated when its archive was closed\n");
        zip_source_free(source);
        zip_error_fini(&error);
        return 1;
    }

    zip_source_free(source);
    zip_error_fini(&error);
    return 0;
}
