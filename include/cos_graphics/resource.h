#ifndef _CG_RESOURCE_H_
#define _CG_RESOURCE_H_

#ifdef __cplusplus
#include "resource_ext.h"
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
 * @return CG_BOOL CG_TRUE if the resource system is initialized, CG_FALSE otherwise.
 */
CG_BOOL CGResourceSystemInitialized();

/**
 * @brief Initialize resource system.
 */
void CGInitResourceSystem();

/// Disk functions ///

/**
 * @brief Load file from disk
 * 
 * @param file_path file path
 * @return char* file data
 */
char* CGLoadFile(const char* file_path);

typedef struct{
    int width, height, channels;
    unsigned char* data;
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
 * @return CGImage* The image data created.
 */
CGImage* CGLoadImage(const char* file_path);

/**
 * @brief Load image from resource.
 * 
 * @param file_rk The resource key of the image.
 * @return CGImage* The image data created.
 */
CGImage* CGLoadImageFromResource(const char* file_rk);

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
 * @return char* The data of the resource.
 */
char* CGLoadResource(const char* resource_key, int* size, char* type);

#ifdef __cplusplus
}
#endif

#endif  //_CG_RESOURCE_H_