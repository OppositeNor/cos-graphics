#ifndef _CG_RESOURCE_H_
#define _CG_RESOURCE_H_

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
 * @brief Load image from disk.
 * 
 * @param file_path Image file path.
 * @return CGImage* The image file created.
 */
CGImage* CGLoadImage(const char* file_path);

/**
 * @brief Delete image object.
 * 
 * @param image Image object to be deleted.
 */
void CGDeleteImage(CGImage* image);

#endif  //_CG_RESOURCE_H_