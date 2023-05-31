#include "cos_graphics/resource.h"
#include "cos_graphics/log.h"
#include "cos_graphics/linked_list.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define CG_EXTRACT_MEM_RES_NODE_DATA(node) ((CGMemRes*)(node->data))

typedef CGLinkedListNode CGMemResNode;

typedef struct
{
    void* data;
    void (*deleter)(void*);
}CGMemRes;

enum CGMemResType
{
    CG_MEM_RES_TYPE_HEAD = 0,

    CG_MEM_RES_TYPE_DATA
};

/**
 * @brief Linked list head for memory resources.
 */
static CGMemResNode *mem_res_head = NULL;

CG_BOOL CGResourceSystemInitialized()
{
    return mem_res_head != NULL;
}

void CGInitResourceSystem()
{
    mem_res_head = CGCreateLinkedListNode(NULL, CG_MEM_RES_TYPE_HEAD);
}


/// Disk functions ///
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
    file_data[file_size - 1] = '\0';
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
    CGRegisterResource(image, CG_DELETER(CGDeleteImage));
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

void CGSetFloatArrayValue(unsigned int count, float* array, ...)
{
    va_list args;
    va_start(args, array);
    for (int i = 0; i < count; ++i)
    {
        array[i] = (float)va_arg(args, double);
    }
    va_end(args);
}

/// Memory functions ///

CGMemRes* CGCreateMemRes(void* data, void (*deleter)(void*))
{
    CGMemRes* mem_res = (CGMemRes*)malloc(sizeof(CGMemRes));
    CG_ERROR_COND_RETURN(mem_res == NULL, NULL, "Failed to allocate memory for memory resource.");
    mem_res->data = data;
    mem_res->deleter = deleter;
    return mem_res;
}

void CGRegisterResource(void* data, void (*deleter)(void*))
{
    CG_ERROR_CONDITION(mem_res_head == NULL, "Memory resource system not initialized.");
    CGMemRes* resource = CGCreateMemRes(data, deleter);
    CG_ERROR_CONDITION(resource == NULL, "Failed to create memory resource.");
    CGMemResNode* p = mem_res_head;
    while (p->next != NULL)
    {
        CG_ERROR_CONDITION(p->data == data, "Resource already registered.");
        p = p->next;
    }
    p->next = CGCreateLinkedListNode(resource, CG_MEM_RES_TYPE_DATA);
}


void CGFreeResource(void* resource)
{
    CG_ERROR_CONDITION(mem_res_head == NULL, "Memory resource system not initialized.");
    CGMemResNode* p = mem_res_head;
    while (p->next != NULL)
    {
        CGMemRes* extract = CG_EXTRACT_MEM_RES_NODE_DATA(p->next);
        if (extract->data == resource)
        {
            extract->deleter(extract->data);
            CGRemoveLinkedListNode(&(p->next));
            return;
        }
        p = p->next;
    }
    CG_ERROR_CONDITION(CG_TRUE, "Resource not found.");
}

void CGPrintMemoryList()
{
    CGPrintList(mem_res_head);
}

void CGClearResource()
{
    CGMemResNode* p = mem_res_head;
    while (p->next != NULL)
    {
        CGMemResNode* temp = p->next;
        p->next = p->next->next;
        CGMemRes* mem_res = (CGMemRes*)temp->data;
        mem_res->deleter(mem_res->data);
        free(mem_res);
        free(temp);
    }
}

void CGTerminateResourceSystem()
{
    CGClearResource();
    free(mem_res_head);
    mem_res_head = NULL;
}