#include "cos_graphics/resource.h"
#include "cos_graphics/log.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// set array value
void CGSetFloatArrayValue(unsigned int count, float* array, float first, ...);

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

    // this check is required in release mode
    if (image == NULL)
    {
        CG_ERROR("Failed to allocate memory for image object.");
        return NULL;
    }
    image->width = width;
    image->height = height;
    image->channels = channels;
    int image_size = width * height * channels;
    if (data != NULL)
    {
        image->data = (unsigned char*)malloc(sizeof(unsigned char) * image_size);
        CG_ERROR_COND_RETURN(image->data == NULL, NULL, "Failed to allocate memory for image data.");
        memcpy(image->data, data, image_size);
    }
    else
        image->data = NULL;
    return image;
}

CGImage* CGLoadImage(const char* file_path)
{
    CGImage* image = CGCreateImage(0, 0, 0, NULL);
    image->data = stbi_load(file_path, &image->width, &image->height, &image->channels, 0);
    if (image->data == NULL)
    {
        free(image);
        CG_ERROR("Failed to load image from path: %s", file_path);
        return NULL;
    }
    return image;
}

void CGDeleteImage(CGImage* image)
{
    free(image->data);
    free(image);
}

void CGSetFloatArrayValue(unsigned int count, float* array, float first, ...)
{
    va_list args;
    va_start(args, first);
    array[0] = first;
    for (int i = 0; i < count; i ++)
    {
        array[i] = (float)va_arg(args, double);
    }
    va_end(args);
}