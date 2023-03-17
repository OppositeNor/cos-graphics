#include "cos_graphics/resource.h"
#include "cos_graphics/log.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

CGImage* CGCreateImage(int width, int height, int channels, unsigned char* data)
{
    CGImage* image = (CGImage*)malloc(sizeof(CGImage));
    if (image == NULL)
    {
        CG_ERROR("Failed to allocate memory for image object.");
        return NULL;
    }
    image->width = width;
    image->height = height;
    image->channels = channels;
    int image_size = width * height * channels;
    image->data = (unsigned char*)malloc(sizeof(unsigned char) * image_size);
    if (image->data == NULL)
    {
        CG_ERROR("Failed to allocate memory for image data.");
        return NULL;
    }
    memcpy(image->data, data, image_size);
    return image;
}

CGImage* CGLoadImage(const char* file_path)
{
    
}