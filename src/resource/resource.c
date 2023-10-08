#include "cos_graphics/resource.h"
#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include "cos_graphics/utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef CG_TG_WIN
    #include <windows.h>

    #define CG_FILE_SPLITTER (CGChar)'\\'

    static void CGGetExecDir(CGChar* buff, unsigned int size)
    {
#ifdef CG_USE_WCHAR
        char* temp = (char*)malloc(size * sizeof(char));
        CG_ERROR_COND_EXIT(temp == NULL, -1, CGSTR("Failed to allocate memory for temp buffer."));
        GetModuleFileName(NULL, temp, size - 1);
        char* p = temp + strlen(temp) - 1;
        for (; *p != '\\'; p--);
        *p = '\0';
        CharToCGChar(temp, buff, size);
        free(temp);
#else
        GetModuleFileName(NULL, buff, size - 1);
        CGChar* p = buff + CG_STRLEN(buff) - 1;
        for (; *p != '\\'; p--);
        *p = '\0';
#endif
    }
#else
    #include <unistd.h>

    #define CG_FILE_SPLITTER (CGChar)'/'

    static void CGGetExecDir(CGChar* buff, unsigned int size)
    {
#ifdef CG_USE_WCHAR
        char* temp = (char*)malloc(size * sizeof(char));
        CG_ERROR_COND_EXIT(temp == NULL, -1, CGSTR("Failed to allocate memory for temp buffer."));
        readlink("/proc/self/exe", temp, size - 1);
        char* p = temp + size - 1;
        for (; *p != '/' && p > temp; p--);
        *p = '\0';
        CharToCGChar(temp, buff, size);
        free(temp);
#else
        readlink("/proc/self/exe", buff, size - 1);
        char* p = buff + size - 1;
        for (; *p != '/' && p > buff; p--);
        *p = '\0';
#endif
    }
#endif

/**
 * @brief Open a file with the given path and mode (equivalent to fopen but with CGChar)
 * 
 * @param file_path The path of the file.
 * @param mode The mode of the file.
 * @return FILE* The file pointer.
 */
static inline FILE* CGFOpen(const CGChar* file_path, const char* mode);

/**
 * @brief The path of the resource. (uwu stands for unified wrapper unit)
 */
static const CGChar* cg_resource_file_name = CGSTR("resource.uwu");
static CGChar* cg_resource_file_path = NULL;

/**
 * @brief The path of the resource finder. (qaq stands for qualified access query)
 */
static const CGChar* cg_resource_finder_name = CGSTR("finder.qaq");
static CGChar* cg_resource_finder_path = NULL;


#define CG_EXTRACT_MEM_RES_NODE_DATA(node) ((CGMemRes*)(node->data))

typedef CGLinkedListNode CGMemResNode;

typedef struct
{
    void* data;
    void (*deleter)(void*);
}CGMemRes;

typedef struct CGTextureResource{
    unsigned int texture_id;
    unsigned int reference_count;
    CGChar* key;
    struct CGTextureResource* next;
}CGTextureResource;

typedef struct CGReusableResource{
    unsigned int size;
    void* data;
    void (*deleter)(void*);
    CGChar* key;
    struct CGReusableResource* next;
}CGReusableResource;

static CGTextureResource* cg_texture_res_head = NULL;
static CGReusableResource* cg_reusable_res_head = NULL;

enum CGMemResType
{
    CG_MEM_RES_TYPE_HEAD = 0,

    CG_MEM_RES_TYPE_DATA
};

/**
 * @brief Linked list head for memory resources.
 */
static CGMemResNode* mem_res_head = NULL;

/**
 * @brief The release queue. The resource registered will be freed at the end of the frame.
 */
static CGMemResNode* cg_release_queue_head = NULL;

/**
 * @brief Clear the resource in the resource list.
 */
static void CGClearResource();

/**
 * @brief Clear the resource in the resource queue.
 */
static void CGClearQueueResource();

CG_BOOL CGResourceSystemInitialized()
{
    return mem_res_head != NULL;
}

void CGInitResourceSystem()
{
    CGChar buff[256];
    CGGetExecDir(buff, 256);
    unsigned int temp_size = sizeof(CGChar) * (CG_STRLEN(buff) + CG_STRLEN(cg_resource_file_name) + 2);
    cg_resource_file_path = (CGChar*)malloc(temp_size);
    CG_ERROR_CONDITION(cg_resource_file_path == NULL, CGSTR("Failed to allocate memory for resource file path."));
#ifdef CG_USE_WCHAR
    CG_SPRINTF(cg_resource_file_path, temp_size, CGSTR("%ls%lc%ls"), buff, CG_FILE_SPLITTER, cg_resource_file_name);
#else
    CG_SPRINTF(cg_resource_file_path, temp_size, CGSTR("%s%c%s"), buff, CG_FILE_SPLITTER, cg_resource_file_name);
#endif
    temp_size = sizeof(CGChar) * (CG_STRLEN(buff) + CG_STRLEN(cg_resource_finder_name) + 2);
    cg_resource_finder_path = (CGChar*)malloc(temp_size);
    if (cg_resource_finder_path == NULL)
    {
        free(cg_resource_file_path);
        cg_resource_file_path = NULL;
        CG_ERROR_CONDITION(CG_TRUE, CGSTR("Failed to allocate memory for resource finder path."));
    }
#ifdef CG_USE_WCHAR
    CG_SPRINTF(cg_resource_finder_path, temp_size, CGSTR("%ls%lc%ls"), buff, CG_FILE_SPLITTER, cg_resource_finder_name);
#else
    CG_SPRINTF(cg_resource_finder_path, temp_size, CGSTR("%s%c%s"), buff, CG_FILE_SPLITTER, cg_resource_finder_name);
#endif
    
    mem_res_head = CGCreateLinkedListNode(NULL, CG_MEM_RES_TYPE_HEAD);
    cg_release_queue_head = CGCreateLinkedListNode(NULL, CG_MEM_RES_TYPE_HEAD);
    cg_texture_res_head = (CGTextureResource*)malloc(sizeof(CGTextureResource));
    CG_ERROR_CONDITION(cg_texture_res_head == NULL, CGSTR("Failed to allocate memory for texture resource head."));
    cg_reusable_res_head = (CGReusableResource*)malloc(sizeof(CGReusableResource));
    if (cg_reusable_res_head == NULL)
    {
        free(cg_texture_res_head);
        cg_texture_res_head = NULL;
        CG_ERROR_CONDITION(CG_TRUE, CGSTR("Failed to allocate memory for reusable resource head."));
    }
    cg_texture_res_head->key = NULL;
    cg_texture_res_head->texture_id = 0;
    cg_texture_res_head->next = NULL;

    cg_reusable_res_head->key = NULL;
    cg_reusable_res_head->data = NULL;
    cg_reusable_res_head->size = 0;
    cg_reusable_res_head->next = NULL;
}


/// Disk functions ///
CGByte* CGLoadFile(const CGChar* file_path)
{
    FILE* file = CGFOpen(file_path, "rb");
    if (file == NULL)
    {
#ifdef CG_USE_WCHAR
        CG_ERROR(CGSTR("Failed to open file at path: %ls."), file_path);
#else
        CG_ERROR(CGSTR("Failed to open file at path: %s."), file_path);
#endif
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    ++file_size;

    CGByte* file_data = (CGByte*)malloc(file_size * sizeof(CGByte));
    if (file_data == NULL)
    {
        fclose(file);
        CG_ERROR(CGSTR("Failed to allocate memory for file data."));
        return NULL;
    }
    rewind(file);
    fread(file_data, sizeof(CGByte), file_size - 1, file);
    fclose(file);
    return file_data;
}

CGImage* CGCreateImage(int width, int height, int channels, CGUByte* data)
{
    CGImage* image = (CGImage*)malloc(sizeof(CGImage));

    // this check is required in release mode
    if (image == NULL)
    {
        CG_ERROR(CGSTR("Failed to allocate memory for image object."));
        return NULL;
    }
    image->width = width;
    image->height = height;
    image->channels = channels;
    int image_size = width * height * channels;
    if (data != NULL)
    {
        image->data = (CGUByte*)malloc(sizeof(CGUByte) * image_size);
        if (image->data == NULL)
        {
            free(image);
            CG_ERROR_COND_RETURN(CG_TRUE, NULL, CGSTR("Failed to allocate memory for image data."));
        }
        memcpy(image->data, data, image_size);
    }
    else
        image->data = NULL;
    CGRegisterResource(image, CG_DELETER(CGDeleteImage));
    return image;
}

CGImage* CGLoadImage(const CGChar* file_path)
{
    CGImage* image = CGCreateImage(0, 0, 0, NULL);
#ifdef CG_USE_WCHAR
    {
        char file_path_buff[256];
        CGCharToChar(file_path, file_path_buff, 256);
        image->data = stbi_load(file_path_buff, &image->width, &image->height, &image->channels, 0);
    }
#else
    image->data = stbi_load(file_path, &image->width, &image->height, &image->channels, 0);
#endif
    if (image->data == NULL)
    {
        free(image);
#ifdef CG_USE_WCHAR
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, CGSTR("Failed to load image from path: %ls"), file_path);
#else
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, CGSTR("Failed to load image from path: %s"), file_path);
#endif
    }
    return image;
}

CGImage* CGLoadImageFromResource(const CGChar* file_rk)
{
    CGImage* image = CGCreateImage(0, 0, 0, NULL);
    int buff_size;
    CGByte* img_buff = CGLoadResource(file_rk, &buff_size, NULL);
    if (img_buff == NULL)
    {
        free(image);
#ifdef CG_USE_WCHAR
        CG_ERROR(CGSTR("Failed to load image from key: %ls"), file_rk);
#else
        CG_ERROR(CGSTR("Failed to load image from key: %s"), file_rk);
#endif
        return NULL;
    }
    image->data = stbi_load_from_memory(img_buff, buff_size, &image->width, &image->height, &image->channels, 0);
    free(img_buff);
    if (image->data == NULL)
    {
        free(image);
#ifdef CG_USE_WCHAR
        CG_ERROR(CGSTR("Failed to load image from key: %ls"), file_rk);
#else
        CG_ERROR(CGSTR("Failed to load image from key: %s"), file_rk);
#endif
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
        float f = (float)va_arg(args, double);
        array[i] = f;
    }
    va_end(args);
}

/// Memory functions ///

CGMemRes* CGCreateMemRes(void* data, void (*deleter)(void*))
{
    CGMemRes* mem_res = (CGMemRes*)malloc(sizeof(CGMemRes));
    CG_ERROR_COND_RETURN(mem_res == NULL, NULL, CGSTR("Failed to allocate memory for memory resource."));
    mem_res->data = data;
    mem_res->deleter = deleter;
    return mem_res;
}

void CGRegisterResource(void* data, void (*deleter)(void*))
{
    CG_ERROR_CONDITION(mem_res_head == NULL, CGSTR("Memory resource system not initialized."));
    CGMemRes* resource = CGCreateMemRes(data, deleter);
    CG_ERROR_CONDITION(resource == NULL, CGSTR("Failed to create memory resource."));
    CGMemResNode* p = mem_res_head;
    while (p->next != NULL)
    {
        CG_ERROR_CONDITION(p->data == data, CGSTR("Resource already registered."));
        p = p->next;
    }
    p->next = CGCreateLinkedListNode(resource, CG_MEM_RES_TYPE_DATA);
}


void CGFree(void* resource)
{
    CG_ERROR_CONDITION(mem_res_head == NULL, CGSTR("Memory resource system not initialized."));
    CGMemResNode* p = mem_res_head;
    while (p->next != NULL)
    {
        CGMemRes* extract = CG_EXTRACT_MEM_RES_NODE_DATA(p->next);
        if (extract->data == resource)
        {
            extract->deleter(extract->data);
            CGRemoveLinkedListNode(&(p->next));
            free(extract);
            return;
        }
        p = p->next;
    }
    CG_WARNING(CGSTR("Resource at %p is not registered. Freeing the resource directly..."), resource);
    free(resource);
}

void CGPrintMemoryList()
{
    CGPrintList(mem_res_head);
}

static void CGClearResource()
{
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
    {
        CGReusableResource* p = cg_reusable_res_head;
        while (p->next != NULL)
        {
            CGReusableResource* temp = p->next;
            p->next = p->next->next;
            temp->deleter(temp->data);
            free(temp->key);
            free(temp);
        }
    }
}

// release queue //

void CGQueueFree(void* data, void (*deleter)(void*))
{
    CG_ERROR_CONDITION(cg_release_queue_head == NULL, CGSTR("Memory resource system not initialized."));
    CGMemRes* resource = CGCreateMemRes(data, deleter);
    CG_ERROR_CONDITION(resource == NULL, CGSTR("Failed to create memory resource."));
    CGMemResNode* p = cg_release_queue_head;
    while (p->next != NULL)
    {
        p = p->next;
    }
    p->next = CGCreateLinkedListNode(resource, CG_MEM_RES_TYPE_DATA);
}

static void CGClearQueueResource()
{
    CG_ERROR_CONDITION(cg_release_queue_head == NULL, CGSTR("Memory resource system not initialized."));
    CGMemResNode* p = cg_release_queue_head;
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
    CGClearQueueResource();
    free(mem_res_head);
    mem_res_head = NULL;
    free(cg_resource_file_path);
    cg_resource_file_path = NULL;
    free(cg_resource_finder_path);
    cg_resource_finder_path = NULL;
    free(cg_texture_res_head);
    cg_texture_res_head = NULL;
    free(cg_reusable_res_head);
    cg_reusable_res_head = NULL;
    free(cg_release_queue_head);
    cg_release_queue_head = NULL;
}

void CGResourceSystemUpdate()
{
    CGClearQueueResource();
}

CGByte* CGLoadResource(const CGChar* resource_key, int* size, CGChar* type)
{
    #ifdef CG_USE_WCHAR
    CG_PRINT_VERBOSE(CGSTR("Loading resource with key: %ls"), resource_key);
    CG_ERROR_COND_RETURN(mem_res_head == NULL, NULL, CGSTR("Memory resource system not initialized."));
    FILE* file = CGFOpen(cg_resource_finder_path, "rb");
    CG_ERROR_COND_RETURN(file == NULL, NULL, CGSTR("Failed to open resource finder file at path: %ls."), cg_resource_finder_path);
    #else
    CG_PRINT_VERBOSE(CGSTR("Loading resource with key: %s"), resource_key);
    CG_ERROR_COND_RETURN(mem_res_head == NULL, NULL, CGSTR("Memory resource system not initialized."));
    FILE* file = CGFOpen(cg_resource_finder_path, "rb");
    CG_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to open resource finder file at path: %s."), cg_resource_finder_path);
    #endif
    CGChar buff[256];
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    rewind(file);
    for(unsigned int i = 0;;)
    {
        if (ftell(file) >= file_size - 1)
        {
            fclose(file);
            #ifdef CG_USE_WCHAR
            CG_ERROR_COND_EXIT(CG_TRUE, -1, CGSTR("Failed to find resource with key: %ls."), resource_key);
            #else
            CG_ERROR_COND_EXIT(CG_TRUE, -1, CGSTR("Failed to find resource with key: %s."), resource_key);
            #endif
        }

        CGFRead(&buff[i], sizeof(CGChar), 1, file);

        if (buff[i] != (CGChar)'\0')
        {
            ++i;
            continue;
        }
        
        if (CG_STRCMP(buff, resource_key) == 0)
        {
            unsigned int data_location;
            CGFRead(&data_location, sizeof(unsigned int), 1, file);
            unsigned int data_size;
            CGFRead(&data_size, sizeof(unsigned int), 1, file);
            if (size != NULL)
                *size = data_size;
            if (type != NULL)
            {
                for (int i = 0; type[i] != '\0'; ++i)
                    CGFRead(&type[i], sizeof(CGByte), 1, file);
            }
            fclose(file);
            file = CGFOpen(cg_resource_file_path, "rb");
            CG_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to open resource file."));
            fseek(file, data_location, SEEK_SET);
            CGByte* data = (CGByte*)malloc((data_size + 1) * sizeof(CGByte));
            CG_ERROR_COND_EXIT(data == NULL, -1, CGSTR("Failed to allocate memory for resource data."));
            CGFRead(data, sizeof(CGByte), data_size, file);
            data[data_size] = '\0';
            fclose(file);
            #ifdef CG_USE_WCHAR
            CG_PRINT_VERBOSE(CGSTR("Resource with key: %ls is successfully loaded."), resource_key);
            #else
            CG_PRINT_VERBOSE(CGSTR("Resource with key: %s is successfully loaded."), resource_key);
            #endif
            return data;
        }
        fseek(file, 2 * sizeof(unsigned int), SEEK_CUR);
        for (;;)
        {
            if (ftell(file) >= file_size - 1)
            {
                fclose(file);
                #ifdef CG_USE_WCHAR
                CG_ERROR_COND_EXIT(CG_TRUE, -1, CGSTR("Failed to find resource with key: %ls."), resource_key);
                #else
                CG_ERROR_COND_EXIT(CG_TRUE, -1, CGSTR("Failed to find resource with key: %s."), resource_key);
                #endif
            }
            static CGChar p = 0;
            CGFRead(&p, sizeof(CGChar), 1, file);
            if (p == (CGChar)'\n')
                break;
        }
        i = 0;
    }

    fclose(file);
    #ifdef CG_USE_WCHAR
    CG_ERROR_COND_EXIT(CG_TRUE, -1, CGSTR("Failed to find resource with key: %ls."), resource_key);
    #else
    CG_ERROR_COND_EXIT(CG_TRUE, -1, CGSTR("Failed to find resource with key: %s."), resource_key);
    #endif
}

void CGRegisterTextureResource(const CGChar* key, unsigned int texture_id)
{
    CG_ERROR_CONDITION(cg_texture_res_head == NULL, CGSTR("Texture resource system not initialized."));
    CGTextureResource* p = cg_texture_res_head->next;
    if (p != NULL)
    {
        for (; p->next != NULL; p = p->next)
        {
            if (CG_STRCMP(p->key, key) == 0)
            {
                #ifdef CG_USE_WCHAR
                CG_WARNING(CGSTR("Texture resource with key: %ls already registered."), key);
                #else
                CG_WARNING(CGSTR("Texture resource with key: %s already registered."), key);
                #endif
                return;
            }
            else if (p->next->texture_id == texture_id)
            {
                #ifdef CG_USE_WCHAR
                CG_WARNING(CGSTR("Texture resource with texture id: %d already registered. The key is: \"%ls\"."), texture_id, p->next->key);
                #else
                CG_WARNING(CGSTR("Texture resource with texture id: %d already registered. The key is: \"%s\"."), texture_id, p->next->key);
                #endif
                return;
            }
        }
    }
    CGTextureResource* result = (CGTextureResource*)malloc(sizeof(CGTextureResource));
    CG_ERROR_CONDITION(result == NULL, CGSTR("Failed to allocate memory for texture resource."));
    result->key = (CGChar*)malloc(sizeof(CGChar) * (CG_STRLEN(key) + 1));
    if (result->key == NULL)
    {
        free(result);
        CG_ERROR_CONDITION(CG_TRUE, CGSTR("Failed to allocate memory for texture resource key."));
    }
    CG_STRCPY(result->key, key);
    result->texture_id = texture_id;
    result->reference_count = 0;
    if (p != NULL)
        p->next = result;
    else
        cg_texture_res_head->next = result;
    result->next = NULL;
}

unsigned int CGGetTextureResource(const CGChar* file_rk)
{
    CGTextureResource* p = cg_texture_res_head->next;
    for (; p != NULL; p = p->next)
    {
        if (CG_STRCMP(p->key, file_rk) == 0)
        {
            ++p->reference_count;
            return p->texture_id;
        }
    }

    CGTextureResource* result = (CGTextureResource*)malloc(sizeof(CGTextureResource));
    CG_ERROR_COND_RETURN(result == NULL, 0, CGSTR("Failed to allocate memory for texture resource."));
    CGImage* temp_image = CGLoadImageFromResource(file_rk);
    if (temp_image == NULL)
    {
        free(result);
        CG_ERROR_COND_RETURN(CG_TRUE, 0, CGSTR("Failed to load image."));
    }
    result->key = (CGChar*)malloc(sizeof(CGChar) * (CG_STRLEN(file_rk) + 1));
    if (result->key == NULL)
    {
        free(result);
        CGFree(temp_image);
        CG_ERROR_COND_RETURN(CG_TRUE, 0, CGSTR("Failed to allocate memory for texture resource key."));
    }
    CG_STRCPY(result->key, file_rk);
    result->texture_id = CGCreateTexture(temp_image);
    if (result->texture_id == 0)
    {
        free(result->key);
        free(result);
        CGFree(temp_image);
        CG_ERROR_COND_RETURN(CG_TRUE, 0, CGSTR("Failed to create texture."));
    }
    result->reference_count = 0;
    if (p != NULL)
        p->next = result;
    else
        cg_texture_res_head->next = result;
    result->next = NULL;
    return result->texture_id;
}

unsigned int CGCopyTextureResource(unsigned int texture_id)
{
    CG_ERROR_COND_RETURN(cg_texture_res_head == NULL, 0, CGSTR("Texture resource system not initialized."));

    for (CGTextureResource* p = cg_texture_res_head; p->next != NULL; p = p->next)
    {
        if (p->next->texture_id == texture_id)
        {
            ++(p->next->reference_count);
            return p->next->texture_id;
        }
    }
    CG_ERROR_COND_RETURN(CG_TRUE, 0, CGSTR("Texture resource not found."));
    return 0;
}

void CGFreeTextureResource(unsigned int texture_id)
{
    CG_ERROR_CONDITION(cg_texture_res_head == NULL, CGSTR("Texture resource system not initialized."));

    for (CGTextureResource* p = cg_texture_res_head; p->next != NULL; p = p->next)
    {
        if (p->next->texture_id == texture_id)
        {
            --p->next->reference_count;
            if (p->next->reference_count == 0)
            {
                CGTextureResource* temp = p->next;
                p->next = p->next->next;
                CGDeleteTexture(temp->texture_id);
                free(temp->key);
                free(temp);
            }
            return;
        }
    }
    CGDeleteTexture(texture_id);
}

void CGClearTextureResource()
{
    while (cg_texture_res_head->next != NULL)
    {
        CGTextureResource* temp = cg_texture_res_head->next;
        cg_texture_res_head->next = cg_texture_res_head->next->next;
        CGDeleteTexture(temp->texture_id);
        free(temp->key);
        free(temp);
    }

}

CGUByte* CGLoadReusableResource(const CGChar* key, void (*deleter)(void*), unsigned int* resource_size)
{
    CGReusableResource* p = cg_reusable_res_head;
    for (; p->next != NULL; p = p->next)
    {
        if (CG_STRCMP(p->next->key, key) == 0)
        {
            if (resource_size != NULL)
                *resource_size = p->next->size;
            return p->next->data;
        }
    }
    CGReusableResource* resource = (CGReusableResource*)malloc(sizeof(CGReusableResource));
    CG_ERROR_COND_RETURN(resource == NULL, NULL, CGSTR("Failed to allocate memory for reusable resource."));
    resource->key = (CGChar*)malloc(sizeof(CGChar) * (CG_STRLEN(key) + 1));
    if (resource->key == NULL)
    {
        free(resource);
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, CGSTR("Failed to allocate memory for reusable resource key."));
    }
    CG_STRCPY(resource->key, key);
    resource->data = CGLoadResource(key, &resource->size, NULL);
    if (resource->data == NULL)
    {
        free(resource->key);
        free(resource);
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, CGSTR("Failed to load resource."));
    }
    resource->next = NULL;
    resource->deleter = deleter;
    p->next = resource;
    if (resource_size != NULL)
        *resource_size = resource->size;
    return resource->data;
}

void CGFreeReusableResource(const CGChar* key)
{
    CGReusableResource* p = cg_reusable_res_head;
    for (; p->next != NULL; p = p->next)
    {
        if (CG_STRCMP(p->next->key, key) == 0)
        {
            CGReusableResource* temp = p->next;
            p->next = p->next->next;
            temp->deleter(temp->data);
            free(temp->key);
            free(temp);
            return;
        }
    }
    CG_WARNING(CGSTR("The resource key is not found in the reusable resource."));
}

#ifdef CG_USE_WCHAR
    static inline FILE* CGFOpen(const CGChar* file_path, const char* mode)
    {
        char buffer[256];
        CGCharToChar(file_path, buffer, 256);
        return fopen(buffer, mode);
    }
#else
    static inline FILE* CGFOpen(const CGChar* file_path, const char* mode)
    {
        return fopen(file_path, mode);
    }
#endif