#ifndef _CG_RESOURCE_H_
#define _CG_RESOURCE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"

/**
 * @brief Convert a function to a type that is accepted by the resource system.
 * 
 */
#define CG_DELETER(function) ((void(*)(void*))function)

/**
 * @brief Check if the resource system is initialized.
 * 
 * @returns CG_TRUE if the resource system is initialized.
 * @returns CG_FALSE if the resource system is not initialized.
 */
CG_BOOL CGResourceSystemInitialized();


/**
 * @brief Clear all texture resources. This must be called before terminating OpenGL.
 */
void CGClearTextureResource();

/**
 * @brief Initialize resource system.
 */
void CGInitResourceSystem();

/// Disk functions ///

/**
 * @brief Load binary file from disk.
 * 
 * @param file_path file path.
 * 
 * @returns file data.
 * @returns NULL if failed to load file.
 */
CGByte* CGLoadFile(const CGChar* file_path);

/**
 * @brief Register texture resource to the resource system. After registering, 
 * you can access it with @ref CGGetTextureResource by using the key you entered
 * as the first parameter (file_rk).
 * @param key The key of the resource.
 * @param texture_id The OpenGL texture id.
 */
void CGRegisterTextureResource(const CGChar* key, unsigned int texture_id);

/**
 * @brief Get texture resource.
 * @details This function uses references counting to manage the resource. You should call CGFreeTextureResource 
 * to destruct everytime you call this function. Every time you call this function with the same file_rk, this function
 * will return the same texture id.
 * @param file_rk The resource key of the texture.
 * 
 * @returns The OpenGL texture id.
 * @returns 0 if the file_rk is not valid.
 */
unsigned int CGGetTextureResource(const CGChar* file_rk);

/**
 * @brief Copy texture resource.
 * @details This function basically returns the same value as the texture_id been passed; however, it will increase
 * the references count of the resource by 1. Thus you should call CGFreeTextureResource to destruct everytime you call this
 * function.
 * @param texture_id The OpenGL texture id to be copied.
 * 
 * @returns The copied OpenGL texture id.
 * @returns 0 if the texture_id is not valid.
 */
unsigned int CGCopyTextureResource(unsigned int texture_id);
/**
 * @brief Free texture resource.
 * @detailes This function uses references counting to manage the resource. You should call this function to destruct
 * everytime you call CGGetTextureResource, and the reference will automatically deduct by 1. The resource will be 
 * freed when the references count is 0.
 * @param texture_id The OpenGL texture id to be deleted.
 */
void CGFreeTextureResource(unsigned int texture_id);

typedef struct{
    int width, height, channels;
    CGUByte* data;
}CGImage;

/**
 * @brief Create a CGImage object
 * 
 * @param width Width of the image.
 * @param height Height of the image.
 * @param channels Channel count of the image.
 * @param data The data of the image. The data will be copied to the image.
 * @return CGImage* The created CGImage object.
 */
CGImage* CGCreateImage(int width, int height, int channels, unsigned char* data);

/**
 * @brief Load image from path.
 * 
 * @param file_path The path of the image.
 * @returns The image data created.
 * @returns NULL if failed to load image.
 */
CGImage* CGLoadImage(const CGChar* file_path);

/**
 * @brief Load image from resource.
 * 
 * @param file_rk The resource key of the image.
 * @returns The image data created.
 * @returns NULL if failed to load image.
 */
CGImage* CGLoadImageFromResource(const CGChar* file_rk);

/**
 * @brief Delete image object.
 * 
 * @param image Image object to be deleted.
 */
void CGDeleteImage(CGImage* image);

/**
 * @brief Set float value to array
 * 
 * @param count count of values of the array
 * @param array the array to be set
 * @param ... the values to be set to the array
 */
void CGSetFloatArrayValue(unsigned int count, float* array, ...);

/// Memory functions ///

/**
 * @brief Register a resource.
 * 
 * @param data The data of the resource.
 * @param deleter The deleter of the resource. The data will be released by calling deleter(data)
 */
void CGRegisterResource(void* data, void (*deleter)(void*));

/**
 * @brief Free the resource that's been created.
 * 
 * @param resource The resource to be freed.
 */
void CGFreeResource(void* resource);

/**
 * @brief Print the memory list.
 * 
 */
void CGPrintMemoryList();

/**
 * @brief Clear all resources.
 * 
 */
void CGClearResource();

/**
 * @brief Terminate resource system.
 * 
 */
void CGTerminateResourceSystem();

/**
 * @brief Load resource from resource file.
 * 
 * @param resource_key The key of the resource.
 * @param size This buffer will be set to the size of the resource. If you don't need this, you can set it to NULL.
 * @param type This buffer will be set to the type of the resource. If you don't need this, you can set it to NULL.
 * @return CGByte* The resource data. NULL if failed to load resource.
 */
CGByte* CGLoadResource(const CGChar* resource_key, int* size, CGChar* type);

#ifdef __cplusplus
}
#endif

#endif  //_CG_RESOURCE_H_