#include "cg_resource_wrapper/resource_wrapper.h"
#include "cg_resource_wrapper/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef CGRW_TG_WIN
    #include <windows.h>

    #define FILE_SPLITTER '\\'
#else
    #include <unistd.h>

    #define FILE_SPLITTER '/'
#endif

/**
 * @brief The path of the resource. (uwu stands for unified wrapper unit)
 */
static const char* cgrw_resource_file_name = "resource.uwu";
static char* cgrw_resource_file_path = NULL;

/**
 * @brief The path of the resource finder. (qaq stands for qualified access query)
 */
static const char* cgrw_resource_finder_name = "finder.qaq";
static char* cgrw_resource_finder_path = NULL;

/**
 * @brief Byte array.
 */
typedef struct {
    char* data;         /*The data of the array.*/
    unsigned int size;  /*The size of the array.*/
}CGRWByteArray;

/**
 * @brief Load file data from disk.
 * 
 * @param file_path The path of the file to be loaded.
 * @return char* The file data. Need to be freed manually.
 */
static CGRWByteArray CGRWLoadFile(const char* file_path);

/**
 * @brief Read value of a variable.
 * 
 * @param str The string to read value from.
 * @param buff The buffer to store the value.
 */
static void CGRWReadValue(char* str, char* buff, unsigned int line_count);

/**
 * @brief Remove comments from a string.
 * 
 * @param str The string to remove comments from.
 * @param line_count Line counter.
 */
static void CGRWDeleteComments(char* str);

static void CGRWGetSubString(const char* str, unsigned int start, unsigned int end, char* sub_string);

/**
 * @brief Skip spaces.
 * 
 * @param p The pointer to be moved.
 * @param line_count Line counter.
 */
static void CGRWSkipSpaces(char** p, unsigned int* line_count);

/**
 * @brief Get the first word from a string and store it in a buffer.
 * 
 * @param str The string to get the word from.
 * @param buffer The buffer to store in.
 */
static void CGRWGetFirstWord(const char* str, char* buffer);

/**
 * @brief Move p to the next identifier.
 * 
 * @param p The p to be moved.
 * @param line_count The line count. If set to NULL, it will not be changed.
 * @param identifier The identifier for p to find.
 */
static void CGRWGoToNext(char** p, unsigned int* line_count, char identifier);

/**
 * @brief Phrase data.
 * 
 * @param file_data The data to be phrased.
 * @param data_head The head of the data list.
 */
static void CGRWPhraseData(const char* file_data, CGRWResourceData* data_head);

/**
 * @brief Phrase data from chunk.
 * 
 * @param str The string to get data from.
 * @param line_count Line counter.
 * @param data Data reciever.
 */
static void CGRWPhraseChunk(const char* str, unsigned int line_count, CGRWResourceData* data);

static inline CGRW_BOOL CGRWIsLetter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-'; }

void CGRWInit(int argc, char* argv[])
{
    if (cgrw_resource_file_path != NULL || cgrw_resource_file_path != NULL)
    {
        CGRW_WARNING("Resource wrapper is already initialized.");
        return;
    }
    CGRW_ERROR_COND_EXIT(argc < 2, -1, "No resource path specified.");
    CGRW_PRINT("Resource path: %s\n", argv[1]);

    cgrw_resource_file_path = (char*)malloc((strlen(argv[1]) + strlen(cgrw_resource_file_name) + 2) * sizeof(char));
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL, -1, "Failed to allocate memory for resource file path.");
    sprintf(cgrw_resource_file_path, "%s%c%s", argv[1], FILE_SPLITTER, cgrw_resource_file_name);
    CGRW_PRINT("Resource file path: %s\n", cgrw_resource_file_path);

    cgrw_resource_finder_path = (char*)malloc((strlen(argv[1]) + strlen(cgrw_resource_finder_name) + 2) * sizeof(char));
    CGRW_ERROR_COND_EXIT(cgrw_resource_finder_path == NULL, -1, "Failed to allocate memory for resource finder path.");
    sprintf(cgrw_resource_finder_path, "%s%c%s", argv[1], FILE_SPLITTER, cgrw_resource_finder_name);
    CGRW_PRINT("Resource finder path: %s\n", cgrw_resource_finder_path);

    remove(cgrw_resource_file_path);
    remove(cgrw_resource_finder_path);
}

void CGRWTerminate()
{
    free(cgrw_resource_file_path);
    free(cgrw_resource_finder_path);
    cgrw_resource_file_path = NULL;
    cgrw_resource_finder_path = NULL;
    CGRW_PRINT("Resource wrapper terminated.\n");
}

static CGRWByteArray CGRWLoadFile(const char* file_path)
{
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL || cgrw_resource_finder_path == NULL, -1, "Cannot load file: Resource wrapper is not initialized.");
    FILE* file = fopen(file_path, "rb");
    CGRW_ERROR_COND_EXIT(file == NULL, -1, "Failed to open file at path: %s.", file_path);
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    CGRWByteArray file_data;
    file_data.data = (char*)malloc((file_size + 1) * sizeof(char));
    if (file_data.data == NULL)
    {
        fclose(file);
        CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Failed to allocate memory for file data.");
    }
    rewind(file);
    fread(file_data.data, 1, file_size, file);
    file_data.data[file_size] = '\0';
    file_data.size = file_size;
    fclose(file);
    return file_data;
}

void CGRWAddResource(CGRWResourceData* res_data)
{
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL || cgrw_resource_finder_path == NULL, -1, "Cannot add resource: Resource wrapper is not initialized.");
    CGRW_ERROR_COND_EXIT(res_data == NULL, -1, "Cannot add resource: Resource data is NULL.");
    CGRW_PRINT("Adding resource: %s.", res_data->key);
    CGRWByteArray data = CGRWLoadFile(res_data->path);
    // write in resource file
    FILE* file = fopen(cgrw_resource_file_path, "rb+");
    if (file == NULL)
    {
        file = fopen(cgrw_resource_file_path, "wb");
        CGRW_ERROR_COND_EXIT(file == NULL, -1, "Failed to create resource file.");
    }
    fseek(file, 0, SEEK_END);
    unsigned int data_location = ftell(file);
    fwrite(data.data, 1, data.size, file);
    fclose(file);
    free(data.data);

    // write in resource finder
    file = fopen(cgrw_resource_finder_path, "ab");
    
    fwrite(res_data->key, 1, strlen(res_data->key) + 1, file);
    fwrite(&data_location, sizeof(unsigned int), 1, file);
    fwrite(&data.size, sizeof(data.size), 1, file);
    fwrite(&res_data->type, strlen(res_data->type), 1, file);
    char end = '\n';
    fwrite(&end, sizeof(end), 1, file);

    fclose(file);
}

char* CGRWLoadResource(const char* resource_key)
{
    FILE* file = fopen(cgrw_resource_finder_path, "rb");
    CGRW_PRINT("Loading resource: %s.", resource_key);
    CGRW_ERROR_COND_EXIT(file == NULL, -1, "Failed to open resource finder file.");
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
            CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Failed to find resource with key: %s.", resource_key);
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
            fclose(file);
            file = fopen(cgrw_resource_file_path, "rb");
            CGRW_ERROR_COND_EXIT(file == NULL, -1, "Failed to open resource file.");
            fseek(file, data_location, SEEK_SET);
            char* data = (char*)malloc(data_size * sizeof(char) + 1);
            CGRW_ERROR_COND_EXIT(data == NULL, -1, "Failed to allocate memory for resource data.");
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
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Failed to find resource with key: %s.", resource_key);
            }
        }
        i = 0;
    }

    fclose(file);
    CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Failed to find resource with key: %s.", resource_key);
}

CGRWResourceData* CGRWPhraseUsedResource(const char* file_path)
{
    CGRW_PRINT("Phrasing resource...");
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL || cgrw_resource_finder_path == NULL, -1, "Cannot phrase resource: Resource wrapper is not initialized.");
    CGRW_ERROR_COND_EXIT(file_path == NULL, -1, "Cannot phrase resource: File path is NULL.");
    FILE* file = fopen(file_path, "r");
    CGRW_PRINT_VERBOSE("Opening file at path: %s", file_path);
    CGRW_ERROR_COND_EXIT(file == NULL, -1, "Failed to find resource file: %s.", file_path);
    unsigned int file_size = 0;
    while (fgetc(file) != EOF)
        ++file_size;
    rewind(file);
    char* file_data = (char*)malloc((file_size + 1) * sizeof(char));
    CGRW_ERROR_COND_EXIT(file_data == NULL, -1, "Failed to allocate memory for file data.");
    fread(file_data, 1, file_size, file);
    file_data[file_size] = '\0';
    CGRW_PRINT_VERBOSE("File loaded with size: %d", file_size);
    fclose(file);

    CGRWDeleteComments(file_data);

    CGRWResourceData* data_head = (CGRWResourceData*)malloc(sizeof(CGRWResourceData));

    CGRWPhraseData(file_data, data_head);
    free(file_data);

    CGRWResourceData* p = data_head->next;

    // check is resource valid
    CGRW_PRINT_VERBOSE("Checking is resource valid...");
    for (unsigned int i = 0; p != NULL; p = p->next)
    {
        CGRW_PRINT_VERBOSE("Checking resource index %d...", i);
        if (p->type[0] == '\0')
        {
            CGRW_PRINT_VERBOSE("Resource type is empty.");
            CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Resource type is empty.");
        }
        if (p->key[0] == '\0')
        {
            CGRW_PRINT_VERBOSE("Resource key is empty.");
            CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Resource key is empty.");
        }
        if (p->path[0] == '\0')
        {
            CGRW_PRINT_VERBOSE("Resource path is empty.");
            CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Resource path is empty.");
        }
        for (CGRWResourceData* q = p->next; q != NULL; q = q->next)
        {
            if (strcmp(p->key, q->key) == 0)
            {
                CGRW_PRINT_VERBOSE("Resource key: %s is duplicated.", p->key);
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Resource key: %s is duplicated.", p->key);
            }
        }
        CGRW_PRINT_VERBOSE("type: %s", p->type);
        CGRW_PRINT_VERBOSE("key: %s", p->key);
        CGRW_PRINT_VERBOSE("path: %s\n", p->path);
        ++i;
    }
    return data_head;
}

static void CGRWPhraseData(const char* file_data, CGRWResourceData* data_head)
{
    unsigned int line_count = 1;
    // head should be kept empty
    CGRW_ERROR_COND_EXIT(data_head == NULL, -1, "Cannot phrase resource with data_head valued \"NULL\".");
    data_head->type[0] = '\0';
    data_head->key[0] = '\0';
    data_head->path[0] = '\0';
    data_head->next = NULL;
    
    CGRWResourceData* data = data_head;
    
    for (char* p = (char*)file_data;;++p)
    {
        CGRWSkipSpaces(&p, &line_count);
        switch(*p)
        {
        case '\0':
            return;
        case ';':
            break;
        case '\"':
            CGRWGoToNext(&p, &line_count, '\"');
            break;
        case '[':
            {
                CGRW_PRINT_VERBOSE("Found resource at line: %d", line_count);
                if (data != data_head || data->type[0] != '\0' || data->key[0] != '\0' || data->path[0] != '\0')
                {
                    CGRWResourceData* t_data = (CGRWResourceData*)malloc(sizeof(CGRWResourceData));
                    CGRW_ERROR_COND_EXIT(t_data == NULL, -1, "Failed to allocate memory for resource data.");
                    data->next = t_data;
                    data = t_data;
                    data->key[0] = '\0';
                    data->path[0] = '\0';
                    data->type[0] = '\0';
                    data->next = NULL;
                }
                char* t_p = p;
                CGRWGoToNext(&p, &line_count, ']');
                CGRW_ERROR_COND_EXIT(t_p == p, -1, "Data type cannot be empty.", line_count);
                CGRWGetSubString(t_p, 1, p - t_p - 1, data->type);
                CGRW_PRINT_VERBOSE("Resource type: \"%s\".", data->type);
            }break;
        case '{':
            CGRW_ERROR_COND_EXIT(data == NULL, -1, "Invalid resource file format at line: %d.", line_count);
            CGRWPhraseChunk(p, line_count, data);
            break;

        default:
            CGRWGoToNext(&p, &line_count, ';');
            break;
        }
    }
}

static void CGRWPhraseChunk(const char* str, unsigned int line_count, CGRWResourceData* data)
{
    CGRW_PRINT_VERBOSE("Reading resource chunk...");
    CGRW_ERROR_COND_EXIT(str == NULL, -1, "Cannot phrase chunk from string pointer \"NULL\".");
    CGRW_ERROR_COND_EXIT(data == NULL, -1, "Cannot phrase chunk with resource data pointer \"NULL\".");

    char* p = (char*)(str + 1);
    for (;;++p)
    {
        CGRWSkipSpaces(&p, &line_count);
        switch (*p)
        {
        case '\0':
            CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Invalid resource file format at line: %d.", line_count);
        default:{
            if (*p == '}')
            {
                ++p;
                CGRWSkipSpaces(&p, &line_count);
                CGRW_ERROR_COND_EXIT(*p != ';', -1, "Invalid resource file format at line: %d, expected character: \';\', but get \'%c\' instead.", line_count, *p);
                return;
            }
            CGRW_ERROR_COND_EXIT(!CGRWIsLetter(*p) && *p != '\"' && *p != ';', -1, "Invalid resource file format at line: %d, unexpected character: \'%c\'.", line_count, *p);
            char buff[256];
            CGRWGetFirstWord(p, buff);
            p += strlen(buff);
            CGRWSkipSpaces(&p, &line_count);
            if (*p != '=')
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, "Invalid resource file format at line: %d, expected character: \'=\', but get \'%c\' instead.", line_count, *p);
            ++p;
            CGRWSkipSpaces(&p, &line_count);
            char* t_p = p;
            CGRWGoToNext(&t_p, &line_count, '"');
            if (strcmp(buff, "key") == 0)
                CGRWGetSubString(p, 0, t_p - p, data->key);
            else if (strcmp(buff, "path") == 0)
                CGRWGetSubString(p, 0, t_p - p, data->path);
            p = t_p + 1;
            CGRWSkipSpaces(&p, &line_count);
            CGRW_ERROR_COND_EXIT(*p != ';', -1, "Invalid resource file format at line: %d, expected character: \';\', but get \'%c\' instead.", line_count, *p);
            }break;
        };
    }
}

static void CGRWSkipSpaces(char** p, unsigned int* line_count)
{
    CGRW_ERROR_COND_EXIT(p == NULL, -1, "Cannot skip spaces with string pointer pointer \"NULL\".");
    CGRW_ERROR_COND_EXIT(*p == NULL, -1, "Cannot skip spaces with string pointer \"NULL\".");
    CGRW_ERROR_COND_EXIT(line_count == NULL, -1, "Cannot skip spaces with line count pointer \"NULL\".");

    for (;;++(*p))
    {
        switch (**p)
        {
        case ' ':
        case '\t':
            break;
        case '\n':
            ++(*line_count);
            break;
        default:
            return;
        }
    }
}

static void CGRWReadValue(char* str, char* buff, unsigned int line_count)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, "Cannot read value from string pointer \"NULL\".");
    CGRW_ERROR_COND_EXIT(buff == NULL, -1, "Cannot read value with buff pointer \"NULL\".");

    char* p = str;
    CGRWSkipSpaces(&p, &line_count);
    CGRW_ERROR_COND_EXIT(*p != '=', -1, "Invalid resource file format at line: %d.", line_count);
    CGRWSkipSpaces(&p, &line_count);
    CGRW_ERROR_COND_EXIT(*p != '\"', -1, "Invalid resource file format at line: %d.", line_count);
    char* t_p = p;
    CGRWGoToNext(&t_p, &line_count, '\"');
    CGRWGetSubString(p, 0, t_p - p, buff);
}

static void CGRWDeleteComments(char* str)
{
    unsigned int line_count;
    CGRW_PRINT_VERBOSE("Removing comments...");
    char* p = str;
    for (;;++p)
    {
        switch(*p)
        {
        case '\0':
            return;
        case '\n':
            ++line_count;
            break;
        case '\"':
            CGRWGoToNext(&p, &line_count, '\"');
            break;
        case '#':
            while (*p != '\n' && *p != '\0')
            {
                *p = ' ';
                ++p;
            }break;
        default:
            break;
        }
    }
    CGRW_PRINT_VERBOSE("Comments removed");
}

static void CGRWGetSubString(const char* str, unsigned int start, unsigned int end, char* sub_string)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, "Cannot get substring from string pointer \"NULL\".");
    CGRW_ERROR_COND_EXIT(start >= strlen(str), -1, "Invalid start string index.");
    CGRW_ERROR_COND_EXIT(end >= strlen(str), -1, "Invalid end string index.");
    while (str[start] == ' ' || str[start] == '\t')     ++start;
    while (str[end] == ' ' || str[end] == '\t')         --end;
    if (str[start] == '\"')
        ++start;
    if (str[end] == '\"')
        --end;
    CGRW_ERROR_COND_EXIT(start > end, -1, "Start string index cannot be greater than end string index.");
    CGRW_ERROR_COND_EXIT(sub_string == NULL, -1, "Cannot get substring with substring pointer \"NULL\".");

    
    memcpy(sub_string, str + start, end - start + 1);
    sub_string[end - start + 1] = '\0';
}

static void CGRWGetFirstWord(const char* str, char* sub_string)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, "Cannot get first word from string pointer \"NULL\".");
    CGRW_ERROR_COND_EXIT(sub_string == NULL, -1, "Cannot get first word with substring pointer \"NULL\".");
    unsigned int i = 0;
    for( ; !CGRWIsLetter(str[i]); ++i)
    {
        if (str[i] == '\0')
        {
            sub_string[0] = '\0';
            return;
        }
        ++i;
    }
    for( ; CGRWIsLetter(str[i]); ++i)
    {
        *sub_string = str[i];
        ++sub_string;
    }
}

static void CGRWGoToNext(char** p, unsigned int* line_count, char identifier)
{
    CGRW_ERROR_COND_EXIT(p == NULL, -1, "Cannot go to next with string pointer \"NULL\".");
    CGRW_ERROR_COND_EXIT(*p == NULL, -1, "Cannot go to next with string pointer \"NULL\".");
    
    unsigned int temp_line_count = *line_count;
    for (;;)
    {
        ++(*p);
        CGRW_ERROR_COND_EXIT(**p == '\0', -1, "Error, line: %d, Segment is expected to be closed with: \"%c\".", temp_line_count, identifier);
        if (**p == identifier)
            return;
        if (**p == '\n' && line_count != NULL)
            ++(*line_count);
    }
}