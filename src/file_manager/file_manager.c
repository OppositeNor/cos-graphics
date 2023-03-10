#include "cos_graphics/file_manager.h"
#include "cos_graphics/log.h"

#include <stdio.h>
#include <stdlib.h>
char* CGLoadFile(const char* file_path)
{
    FILE* file = fopen(file_path, "r");
    if (file == NULL)
    {
        CG_ERROR("Failed to open file at path: %s.", file_path);
        return NULL;
    }
    unsigned int file_size = 0;
    while(fgetc(file) != EOF){
        ++file_size;
    }
    ++file_size;
    char* file_data = (char*)malloc(file_size * sizeof(char));
    if (file_data == NULL)
    {
        CG_ERROR("Failed to allocate memory for file data.");
        return NULL;
    }
    rewind(file);
    for (int i = 0; i < file_size; ++i)
    {
        file_data[i] = fgetc(file);
    }
    fclose(file);
    return file_data;
}

