#include "cos_graphics/resource.h"
#include "cos_graphics/log.h"
#include "cos_graphics/linked_list.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef CG_TG_WIN
    #include <windows.h>

    #define CG_FILE_SPLITTER '\\'

    static void CGGetExecDir(char* buff, unsigned int size)
    {
        GetModuleFileName(NULL, buff, size - 1);
        char* p = buff + strlen(buff) - 1;
        for (; *p != '\\'; p--);
        *p = '\0';
    }
#else
    #include <unistd.h>

    #define CG_FILE_SPLITTER '/'

    static void CGGetExecDir(char* buff, unsigned int size)
    {
        char* p = buff;
        readlink("/proc/self/exe", buff, size - 1);
        char* p = buff + size - 1;
        for (; *p != '/'; p--);
        *p = '\0';
    }
#endif
/**
 * @brief The path of the resource. (uwu stands for unified wrapper unit)
 */
static const char* cg_resource_file_name = "resource.uwu";
static char* cg_resource_file_path = NULL;

/**
 * @brief The path of the resource finder. (qaq stands for qualified access query)
 */
static const char* cg_resource_finder_name = "finder.qaq";
static char* cg_resource_finder_path = NULL;


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
    char buff[256];
    CGGetExecDir(buff, 256);
    cg_resource_file_path = (char*)malloc(sizeof(char) * (strlen(buff) + strlen(cg_resource_file_name) + 2));
    CG_ERROR_CONDITION(cg_resource_file_path == NULL, "Failed to allocate memory for resource file path.");
    sprintf(cg_resource_file_path, "%s%c%s", buff, CG_FILE_SPLITTER, cg_resource_file_name);

    cg_resource_finder_path = (char*)malloc(sizeof(char) * (strlen(buff) + strlen(cg_resource_finder_name) + 2));
    if (cg_resource_finder_path == NULL)
    {
        free(cg_resource_file_path);
        cg_resource_file_path = NULL;
        CG_ERROR_CONDITION(CG_TRUE, "Failed to allocate memory for resource finder path.");
    }
    sprintf(cg_resource_finder_path, "%s%c%s", buff, CG_FILE_SPLITTER, cg_resource_finder_name);
    
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
        fclose(file);
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, "Failed to allocate memory for file data.");
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

CGImage* CGLoadImageFromResource(const char* file_rk)
{
    CGImage* image = CGCreateImage(0, 0, 0, NULL);
    int buff_size;
    char* img_buff = CGLoadResource(file_rk, &buff_size, NULL);
    if (img_buff == NULL)
    {
        free(image);
        CG_ERROR("Failed to load image from key: %s", file_rk);
        return NULL;
    }
    image->data = stbi_load_from_memory(img_buff, buff_size, &image->width, &image->height, &image->channels, 0);
    free(img_buff);
    if (image->data == NULL)
    {
        free(image);
        CG_ERROR("Failed to load image from key: %s", file_rk);
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
    free(cg_resource_finder_path);
    cg_resource_finder_path = NULL;
    free(cg_resource_file_path);
    cg_resource_file_path = NULL;
}

char* CGLoadResource(const char* resource_key, int* size, char* type)
{
    CG_ERROR_COND_RETURN(mem_res_head == NULL, NULL, "Memory resource system not initialized.");
    FILE* file = fopen(cg_resource_finder_path, "rb");
    CG_ERROR_COND_EXIT(file == NULL, -1, "Failed to open resource finder file.");
    char buff[256];
    unsigned int i = 0;
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    for(;;)
    {
        fread(&buff[i], 1, 1, file);
        if (ftell(file) >= file_size)
        {
            fclose(file);
            CG_ERROR_COND_EXIT(CG_TRUE, -1, "Failed to find resource with key: %s.", resource_key);
        }
        if (buff[i] != '\0')
        {
            ++i;
            continue;
        }
        
        if (strcmp(buff, resource_key) == 0)
        {
            unsigned int data_location;
            fread(&data_location, sizeof(unsigned int), 1, file);
            unsigned int data_size;
            fread(&data_size, sizeof(unsigned int), 1, file);
            if (size != NULL)
                *size = data_size;
            if (type != NULL)
            {
                for (int i = 0; type[i] != '\0'; ++i)
                    fread(&type[i], 1, 1, file);
            }
            fclose(file);
            file = fopen(cg_resource_file_path, "rb");
            CG_ERROR_COND_EXIT(file == NULL, -1, "Failed to open resource file.");
            fseek(file, data_location, SEEK_SET);
            char* data = (char*)malloc(data_size * sizeof(char) + 1);
            CG_ERROR_COND_EXIT(data == NULL, -1, "Failed to allocate memory for resource data.");
            fread(data, 1, data_size, file);
            data[data_size] = '\0';
            fclose(file);
            return data;
        }
        for (;;)
        {
            char p;
            fread(&p, 1, 1, file);
            if (p == '\n')
                break;
            if (ftell(file) == file_size)
            {
                fclose(file);
                CG_ERROR_COND_EXIT(CG_TRUE, -1, "Failed to find resource with key: %s.", resource_key);
            }
        }
        i = 0;
    }

    fclose(file);
    CG_ERROR_COND_EXIT(CG_TRUE, -1, "Failed to find resource with key: %s.", resource_key);
}