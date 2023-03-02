#include "file_manager.h"

#include <stdio.h>
#include <stdlib.h>
char* CGLoadFile(const char* file_path)
{
    FILE* file = fopen(file_path, "r");
    if (file == NULL)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* file_data = (char*)malloc(file_size + 1);
    if (file_data == NULL)
    {
        fclose(file);
        return NULL;
    }

    fgets(file_data, file_size, file);
    fclose(file);

    return file_data;
}

