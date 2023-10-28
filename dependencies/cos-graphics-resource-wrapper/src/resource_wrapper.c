#include "cg_resource_wrapper/resource_wrapper.h"
#include "cg_resource_wrapper/log.h"

#include <stdlib.h>
#include <string.h>

#ifdef CGRW_TG_WIN
    #include <windows.h>
    #ifdef CGRW_USE_WCHAR
        #define FILE_SPLITTER L'\\'
    #else
        #define FILE_SPLITTER '\\'
#endif
#else
    #include <unistd.h>
    #ifdef CGRW_USE_WCHAR
        #define FILE_SPLITTER L'/'
    #else
        #define FILE_SPLITTER '/'
#endif
#endif

/**
 * @brief The path of the resource. (uwu stands for unified wrapper unit)
 */
static const CGRWChar* cgrw_resource_file_name = CGSTR("resource.uwu");
static CGRWChar* cgrw_resource_file_path = NULL;

/**
 * @brief The path of the resource finder. (qaq stands for qualified access query)
 */
static const CGRWChar* cgrw_resource_finder_name = CGSTR("finder.qaq");
static CGRWChar* cgrw_resource_finder_path = NULL;

/**
 * @brief Load file data from disk.
 * 
 * @param file_path The path of the file to be loaded.
 * @return CGRWChar* The file data. Need to be freed manually.
 */
static CGRWByteArray CGRWLoadFile(const CGRWChar* file_path);

/**
 * @brief Read value of a variable.
 * 
 * @param str The string to read value from.
 * @param buff The buffer to store the value.
 */
static void CGRWReadValue(CGRWChar* str, CGRWChar* buff, unsigned int line_count);

/**
 * @brief Remove comments from a string.
 * 
 * @param str The string to remove comments from.
 * @param line_count Line counter.
 */
static void CGRWDeleteComments(CGRWChar* str);

/**
 * @brief Get the sub string from a string.
 * 
 * @param str The string to get the sub string from.
 * @param start The start index.
 * @param end The end index.
 * @param sub_string The buffer to store the sub string.
 */
static void CGRWGetSubString(const CGRWChar* str, unsigned int start, unsigned int end, CGRWChar** sub_string);

/**
 * @brief Get the data from a file.
 * 
 * @param file The file to get the data from.
 * @return CGRWChar* The data of the file. Need to be freed manually.
 */
static CGRWChar* CGRWGetFileData(FILE* file);

/**
 * @brief Skip spaces.
 * 
 * @param p The pointer to be moved.
 * @param line_count Line counter.
 */
static void CGRWSkipSpaces(CGRWChar** p, unsigned int* line_count);

/**
 * @brief Get the first word from a string and store it in a buffer.
 * 
 * @param str The string to get the word from.
 * @param buffer The buffer to store in.
 */
static void CGRWGetFirstWord(const CGRWChar* str, CGRWChar* buffer);

/**
 * @brief Move p to the next identifier.
 * 
 * @param p The p to be moved.
 * @param line_count The line count. If set to NULL, it will not be changed.
 * @param identifier The identifier for p to find.
 */
static void CGRWGoToNext(CGRWChar** p, unsigned int* line_count, CGRWChar identifier);

/**
 * @brief Phrase data.
 * 
 * @param file_data The data to be phrased.
 * @param data_head The head of the data list.
 */
static void CGRWPhraseData(const CGRWChar* file_data, CGRWResourceData* data_head);

/**
 * @brief Phrase data from chunk.
 * 
 * @param str The string to get data from.
 * @param line_count Line counter.
 * @param data Data reciever.
 */
static void CGRWPhraseChunk(const CGRWChar* str, unsigned int line_count, CGRWResourceData* data);

static inline CGRW_BOOL CGRWIsLetter(CGRWChar c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }

void CGRWInit(int argc, char* argv[])
{
    if (cgrw_resource_file_path != NULL || cgrw_resource_file_path != NULL)
    {
        CGRW_WARNING(CGSTR("Resource wrapper is already initialized."));
        return;
    }
    CGRW_ERROR_COND_EXIT(argc < 2, -1, CGSTR("No resource path specified."));
#ifdef CGRW_USE_WCHAR
    CGRWChar buff[256];
    CharToCGRWChar(argv[1], buff, 256);
    CGRW_PRINT(CGSTR("Resource path: %s"), buff);
    unsigned int cgrw_buffer_length = (CGRW_STRLEN(buff) + CGRW_STRLEN(cgrw_resource_file_name) + 2) * sizeof(CGRWChar);
    cgrw_resource_file_path = (CGRWChar*)malloc(cgrw_buffer_length);
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL, -1, CGSTR("Failed to allocate memory for resource file path."));
    CGRW_SPRINTF(cgrw_resource_file_path, cgrw_buffer_length, CGSTR("%s%c%s"), buff, FILE_SPLITTER, cgrw_resource_file_name);
    CGRW_PRINT(CGSTR("Resource file path: %s"), cgrw_resource_file_path);
    cgrw_buffer_length = (CGRW_STRLEN(buff) + CGRW_STRLEN(cgrw_resource_finder_name) + 2) * sizeof(CGRWChar);
    cgrw_resource_finder_path = (CGRWChar*)malloc(cgrw_buffer_length);
    CGRW_ERROR_COND_EXIT(cgrw_resource_finder_path == NULL, -1, CGSTR("Failed to allocate memory for resource finder path."));
    CGRW_SPRINTF(cgrw_resource_finder_path, cgrw_buffer_length, CGSTR("%s%c%s"), buff, FILE_SPLITTER, cgrw_resource_finder_name);
    CGRW_PRINT(CGSTR("Resource finder path: %s"), cgrw_resource_finder_path);
#else
    CGRW_PRINT(CGSTR("Resource path: %s\n"), argv[1]);
    unsigned int cgrw_buffer_length = (strlen(argv[1]) + CGRW_STRLEN(cgrw_resource_file_name) + 2) * sizeof(CGRWChar);
    cgrw_resource_file_path = (CGRWChar*)malloc(cgrw_buffer_length);
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL, -1, CGSTR("Failed to allocate memory for resource file path."));
    CGRW_SPRINTF(cgrw_resource_file_path, cgrw_buffer_length, CGSTR("%s%c%s"), argv[1], FILE_SPLITTER, cgrw_resource_file_name);
    CGRW_PRINT(CGSTR("Resource file path: %s"), cgrw_resource_file_path);
    cgrw_buffer_length = (strlen(argv[1]) + CGRW_STRLEN(cgrw_resource_finder_name) + 2) * sizeof(CGRWChar);
    cgrw_resource_finder_path = (CGRWChar*)malloc(cgrw_buffer_length);
    CGRW_ERROR_COND_EXIT(cgrw_resource_finder_path == NULL, -1, CGSTR("Failed to allocate memory for resource finder path."));
    CGRW_SPRINTF(cgrw_resource_finder_path, cgrw_buffer_length, CGSTR("%s%c%s"), argv[1], FILE_SPLITTER, cgrw_resource_finder_name);
    CGRW_PRINT(CGSTR("Resource finder path: %s"), cgrw_resource_finder_path);
#endif
    CGRWRemove(cgrw_resource_file_path);
    CGRWRemove(cgrw_resource_finder_path);
}

void CGRWTerminate()
{
    free(cgrw_resource_file_path);
    free(cgrw_resource_finder_path);
    cgrw_resource_file_path = NULL;
    cgrw_resource_finder_path = NULL;
    
    CGRW_PRINT(CGSTR("Resource wrapper terminated."));
}

static CGRWByteArray CGRWLoadFile(const CGRWChar* file_path)
{
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL || cgrw_resource_finder_path == NULL, -1, CGSTR("Cannot load file: Resource wrapper is not initialized."));
    FILE* file = CGRWFOpen(file_path, "rb");
    CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to open file at path: %s."), file_path);
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    CGRWByteArray file_data;
    file_data.data = (CGRWByte*)malloc((file_size + 1) * sizeof(CGRWByte));
    if (file_data.data == NULL)
    {
        fclose(file);
        CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Failed to allocate memory for file data."));
    }
    rewind(file);
    fread(file_data.data, sizeof(CGRWByte), file_size, file);
    file_data.data[file_size] = '\0';
    file_data.size = file_size;
    fclose(file);
    return file_data;
}

void CGRWAddResource(CGRWResourceData* res_data)
{
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL || cgrw_resource_finder_path == NULL, -1, CGSTR("Cannot add resource: Resource wrapper is not initialized."));
    CGRW_ERROR_COND_EXIT(res_data == NULL, -1, CGSTR("Cannot add resource: Resource data is NULL."));
    CGRW_PRINT(CGSTR("Adding resource: %s."), res_data->key);
    FILE* file = NULL;
    CGRWByteArray data;
    if (!res_data->is_data_value)
        data = CGRWLoadFile(res_data->data.path);
    else
        data = res_data->data.value;
    CGRW_PRINT_VERBOSE(CGSTR("Resource data size: %d."), data.size);
    // write in resource file
    file = CGRWFOpen(cgrw_resource_file_path, "rb+");
    if (file == NULL)
    {
        file = CGRWFOpen(cgrw_resource_file_path, "wb");
        CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to create resource file."));
    }
    fseek(file, 0, SEEK_END);
    unsigned int data_location = ftell(file);
    fwrite(data.data, sizeof(CGRWByte), data.size, file);
    fclose(file);
    if (!res_data->is_data_value)
        free(data.data);
    
    // write in resource finder
    file = CGRWFOpen(cgrw_resource_finder_path, "ab");
    
    fwrite(res_data->key, 1, sizeof(CGRWChar) * (CGRW_STRLEN(res_data->key) + 1), file);
    fwrite(&data_location, sizeof(data_location), 1, file);
    fwrite(&data.size, sizeof(data.size), 1, file);
    fwrite(&res_data->type, 1, sizeof(CGRWChar) * (CGRW_STRLEN(res_data->type) + 1), file);
    CGRWChar end = '\n';
    fwrite(&end, sizeof(end), 1, file);

    fclose(file);
}

CGRWResourceData* CGRWPhraseUsedResource(const CGRWChar* file_path)
{
    CGRW_PRINT(CGSTR("Phrasing resource at path: %s"), file_path);
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL || cgrw_resource_finder_path == NULL, -1, CGSTR("Cannot phrase resource: Resource wrapper is not initialized."));
    CGRW_ERROR_COND_EXIT(file_path == NULL, -1, CGSTR("Cannot phrase resource: File path is NULL."));
    FILE* file = CGRWFOpen(file_path, "rb");
    CGRW_PRINT_VERBOSE(CGSTR("Opening file at path: %s"), file_path);
    CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to find resource file: %s."), file_path);
    CGRWChar* file_data = CGRWGetFileData(file);
    fclose(file);

    CGRWDeleteComments(file_data);
    CGRWResourceData* data_head = (CGRWResourceData*)malloc(sizeof(CGRWResourceData));
    CGRW_ERROR_COND_EXIT(data_head == NULL, -1, CGSTR("Failed to allocate memory for resource data."));
    CGRWPhraseData(file_data, data_head);
    free(file_data);

    CGRW_PRINT(CGSTR("Resource phrase finished."));
    CGRWResourceData* p = data_head;

    // check is resource valid
    CGRW_PRINT_VERBOSE(CGSTR("Checking is resource valid..."));

    for (unsigned int i = 0; p != NULL; p = p->next)
    {
        CGRW_PRINT_VERBOSE(CGSTR("Checking resource index %d..."), i);
        CGRW_ERROR_COND_EXIT(p->type == NULL, -1, CGSTR("Resource type is empty."));
        CGRW_ERROR_COND_EXIT(p->key == NULL, -1, CGSTR("Resource key is empty."));
        CGRW_ERROR_COND_EXIT(p->data.path == NULL, -1, CGSTR("Resource path is empty."));
    for (CGRWResourceData* q = p->next; q != NULL; q = q->next)
        {
            if (CGRW_STRCMP(p->key, q->key) == 0)
            {
                CGRW_PRINT_VERBOSE(CGSTR("Resource key: %s is duplicated."), p->key);
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Resource key: %s is duplicated."), p->key);
            }
        }
        CGRW_PRINT_VERBOSE(CGSTR("type: %s"), p->type);
        CGRW_PRINT_VERBOSE(CGSTR("key: %s"), p->key);
        if (p->is_data_value)
            CGRW_PRINT_VERBOSE(CGSTR("value: %s"), p->data.value.data);
        else
            CGRW_PRINT_VERBOSE(CGSTR("path: %s"), p->data.path);
        ++i;
    }
    CGRW_PRINT_VERBOSE(CGSTR("All resources are valid."));
    return data_head;
}

static void CGRWPhraseData(const CGRWChar* file_data, CGRWResourceData* data_head)
{
    CGRW_PRINT_VERBOSE(CGSTR("Phrasing data..."));
    unsigned int line_count = 1;
    
    CGRW_ERROR_COND_EXIT(data_head == NULL, -1, CGSTR("Cannot phrase resource with data_head valued \"NULL\"."));
    data_head->type = NULL;
    data_head->key = NULL;
    data_head->data.path = NULL;
    data_head->next = NULL;
    
    CGRWResourceData* data = data_head;
    for (CGRWChar* p = (CGRWChar*)file_data;;++p)
    {
        CGRWSkipSpaces(&p, &line_count);
        switch(*p)
        {
        case (CGRWChar)'\0':
            CGRW_PRINT_VERBOSE(CGSTR("Data successfully phrased."));
            return;
        case (CGRWChar)';':
            break;
        case (CGRWChar)'\"':
            CGRWGoToNext(&p, &line_count, '\"');
            break;
        case (CGRWChar)'[':
            {
                CGRW_PRINT_VERBOSE(CGSTR("Found resource at line: %d"), line_count);
                if (data != data_head || data->type != NULL || data->key != NULL || data->data.path != NULL)
                {
                    CGRWResourceData* t_data = (CGRWResourceData*)malloc(sizeof(CGRWResourceData));
                    CGRW_ERROR_COND_EXIT(t_data == NULL, -1, CGSTR("Failed to allocate memory for resource data."));
                    data->next = t_data;
                    data = t_data;
                    data->is_data_value = CGRW_FALSE;
                    data->key = NULL;
                    data->data.path = NULL;
                    data->type = NULL;
                    data->next = NULL;
                }
                CGRWChar* t_p = p;
                CGRWGoToNext(&p, &line_count, ']');
                CGRW_ERROR_COND_EXIT(t_p == p, -1, CGSTR("Data type cannot be empty."), line_count);
                CGRWGetSubString(t_p, 1, p - t_p - 1, &data->type);
                CGRW_PRINT_VERBOSE(CGSTR("Resource type: \"%s\"."), data->type);
            }break;
        case (CGRWChar)'{':
            CGRW_ERROR_COND_EXIT(data == NULL, -1, CGSTR("Invalid resource file format at line: %d."), line_count);
            CGRWPhraseChunk(p, line_count, data);
            CGRWGoToNext(&p, &line_count, '}');
            break;

        default:
            CGRWGoToNext(&p, &line_count, ';');
            break;
        }
    }
}

static void CGRWPhraseChunk(const CGRWChar* str, unsigned int line_count, CGRWResourceData* data)
{
    CGRW_PRINT_VERBOSE(CGSTR("Reading resource chunk..."));
    CGRW_ERROR_COND_EXIT(str == NULL, -1, CGSTR("Cannot phrase chunk from string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(data == NULL, -1, CGSTR("Cannot phrase chunk with resource data pointer \"NULL\"."));

    CGRWChar* p = (CGRWChar*)(str + 1);
    for (;;++p)
    {
        CGRWSkipSpaces(&p, &line_count);
        switch (*p)
        {
        case '\0':
            CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Invalid resource file format at line: %d."), line_count);
        default:{
            if (*p == '}')
            {
                ++p;
                CGRWSkipSpaces(&p, &line_count);
                CGRW_ERROR_COND_EXIT(*p != ';', -1, CGSTR("Invalid resource file format at line: %d, expected character: \';\', but get \'%c\' instead."), line_count, *p);
                return;
            }
            CGRW_ERROR_COND_EXIT(!CGRWIsLetter(*p) && *p != '\"' && *p != ';', -1, CGSTR("Invalid resource file format at line: %d, unexpected character: \'%c\'."), line_count, *p);
            CGRWChar buff[256];
            CGRWGetFirstWord(p, buff);
            p += CGRW_STRLEN(buff);
            CGRWSkipSpaces(&p, &line_count);
            if (*p != '=')
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Invalid resource file format at line: %d, expected character: \'=\', but get \'%c\' instead."), line_count, *p);
            ++p;
            CGRWSkipSpaces(&p, &line_count);
            CGRWChar* t_p = p;
            CGRWGoToNext(&t_p, &line_count, '"');
            if (CGRW_STRCMP(buff, CGSTR("key")) == 0)
            {
                if (data->key != NULL)
                    free(data->key);
                CGRWGetSubString(p, 0, t_p - p, &data->key);
                CGRW_PRINT_VERBOSE(CGSTR("key: \"%s\""), data->key);
            }
            else if (CGRW_STRCMP(buff, CGSTR("path")) == 0)
            {
                data->is_data_value = CGRW_FALSE;
                if (data->data.path != NULL)
                    free(data->data.path);
                CGRWGetSubString(p, 0, t_p - p, &data->data.path);
                CGRW_PRINT_VERBOSE(CGSTR("path: \"%s\""), data->data.path);
            }
            else if (CGRW_STRCMP(buff, CGSTR("value")) == 0)
            {
                // currently value only accepts string.
                data->is_data_value = CGRW_TRUE;
                if (data->data.path != NULL)
                    free(data->data.path);
                data->data.value.data = (CGRWByte*)malloc((t_p - p + 1) * sizeof(CGRWChar));
                CGRW_ERROR_COND_EXIT(data->data.value.data == NULL, -1, CGSTR("Failed to allocate memory for resource value."));
                data->data.value.size = (t_p - p + 1) * sizeof(CGRWChar);
                CGRWChar* value_data = (CGRWChar*)data->data.value.data;
                CGRWGetSubString(p, 0, t_p - p, &value_data);
                CGRW_PRINT_VERBOSE(CGSTR("value: \"%s\""), data->data.value.data);
            }
            p = t_p + 1;
            CGRWSkipSpaces(&p, &line_count);
            CGRW_ERROR_COND_EXIT(*p != ';', -1, CGSTR("Invalid resource file format at line: %d, expected character: \';\', but get \'%c\' instead."), line_count, *p);
            }break;
        };
    }
}

static void CGRWSkipSpaces(CGRWChar** p, unsigned int* line_count)
{
    CGRW_ERROR_COND_EXIT(p == NULL, -1, CGSTR("Cannot skip spaces with string pointer pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(*p == NULL, -1, CGSTR("Cannot skip spaces with string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(line_count == NULL, -1, CGSTR("Cannot skip spaces with line count pointer \"NULL\"."));

    for (;;++(*p))
    {
        switch (**p)
        {
        case (CGRWChar)0x0d:
        case (CGRWChar)' ':
        case (CGRWChar)'\t':
            break;
        case (CGRWChar)0x0a:
            ++(*line_count);
            break;
        default:
            return;
        }
    }
}

static void CGRWReadValue(CGRWChar* str, CGRWChar* buff, unsigned int line_count)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, CGSTR("Cannot read value from string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(buff == NULL, -1, CGSTR("Cannot read value with buff pointer \"NULL\"."));

    CGRWChar* p = str;
    CGRWSkipSpaces(&p, &line_count);
    CGRW_ERROR_COND_EXIT(*p != '=', -1, CGSTR("Invalid resource file format at line: %d."), line_count);
    CGRWSkipSpaces(&p, &line_count);
    CGRW_ERROR_COND_EXIT(*p != '\"', -1, CGSTR("Invalid resource file format at line: %d."), line_count);
    CGRWChar* t_p = p;
    CGRWGoToNext(&t_p, &line_count, '\"');
    CGRWGetSubString(p, 0, t_p - p, &buff);
}

static void CGRWDeleteComments(CGRWChar* str)
{
    unsigned int line_count;
    CGRW_PRINT_VERBOSE(CGSTR("Removing comments..."));
    CGRWChar* p = str;
    for (;;++p)
    {
        switch(*p)
        {
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
        if (*p == '\0')
        {
            CGRW_PRINT_VERBOSE(CGSTR("Comments removed."));
            return;
        }
    }
}

static void CGRWGetSubString(const CGRWChar* str, unsigned int start, unsigned int end, CGRWChar** sub_string)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, CGSTR("Cannot get substring from string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(start >= CGRW_STRLEN(str), -1, CGSTR("Invalid start string index."));
    CGRW_ERROR_COND_EXIT(end >= CGRW_STRLEN(str), -1, CGSTR("Invalid end string index."));
    while (str[start] == ' ' || str[start] == '\t')     ++start;
    while (str[end] == ' ' || str[end] == '\t')         --end;
    if (str[start] == '\"')
        ++start;
    if (str[end] == '\"')
        --end;
    CGRW_ERROR_COND_EXIT(start > end, -1, CGSTR("Start string index cannot be greater than end string index."));
    CGRW_ERROR_COND_EXIT(sub_string == NULL, -1, CGSTR("Cannot get substring with substring pointer \"NULL\"."));
    if (*sub_string == NULL)
    {
        *sub_string = (CGRWChar*)malloc((end - start + 2) * sizeof(CGRWChar));
        CGRW_ERROR_COND_EXIT(*sub_string == NULL, -1, CGSTR("Failed to allocate memory for substring."));
    }
    memcpy(*sub_string, str + start, (end - start + 1) * sizeof(CGRWChar));
    (*sub_string)[end - start + 1] = '\0';
}

static CGRWChar *CGRWGetFileData(FILE *file)
{
#if defined CGRW_TG_WIN || !defined CGRW_USE_UTF16LE
    fseek(file, 0, SEEK_END);
    unsigned int file_size = (ftell(file) / sizeof(CGRWChar));
    rewind(file);

    unsigned int bom_skip = 0;
    // skip BOM
#ifdef CGRW_USE_UTF16LE
    fseek(file, 2, SEEK_SET);
    bom_skip = 1;
#else
    if (fgetc(file) == 0xEF)
    {
        fseek(file, 3, SEEK_SET);
        bom_skip = 3;
    }
    else
        rewind(file);
#endif
    CGRWChar* file_data = (CGRWChar*)malloc((file_size - bom_skip + 1) * sizeof(CGRWChar));
    CGRW_ERROR_COND_EXIT(file_data == NULL, -1, CGSTR("Failed to allocate memory for file data."));
    fread(file_data, sizeof(CGRWChar), file_size - bom_skip, file);
    file_data[file_size - bom_skip] = '\0';
    CGRW_PRINT_VERBOSE(CGSTR("File loaded with size: %d"), file_size);
    return file_data;
#else
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    rewind(file);
    unsigned short *file_data_16 = (unsigned short*)malloc((file_size) * sizeof(char));
    CGRW_ERROR_COND_EXIT(file_data_16 == NULL, -1, CGSTR("Failed to allocate memory for file data."));
    fseek(file, 2, SEEK_SET); // skip BOM
    fread(file_data_16, sizeof(char), file_size, file);

    file_size /= 2;
    CGRWChar* file_data = (CGRWChar*)malloc((file_size + 1) * sizeof(CGRWChar));
    CGRW_ERROR_COND_EXIT(file_data == NULL, -1, CGSTR("Failed to allocate memory for file data."));
    for (unsigned int i = 0; i < file_size; ++i)
    {
        file_data[i] = (CGRWChar)file_data_16[i];
    }
    file_data[file_size] = '\0';
    free(file_data_16);
    CGRW_PRINT_VERBOSE(CGSTR("File loaded with size: %d"), file_size);
    return file_data;
#endif
}

static void CGRWGetFirstWord(const CGRWChar* str, CGRWChar* sub_string)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, CGSTR("Cannot get first word from string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(sub_string == NULL, -1, CGSTR("Cannot get first word with substring pointer \"NULL\"."));
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
    *sub_string = '\0';
}

static void CGRWGoToNext(CGRWChar** p, unsigned int* line_count, CGRWChar identifier)
{
    CGRW_ERROR_COND_EXIT(p == NULL, -1, CGSTR("Cannot go to next with string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(*p == NULL, -1, CGSTR("Cannot go to next with string pointer \"NULL\"."));
    unsigned int temp_line_count = *line_count;
    for (;;)
    {
        ++(*p);
        CGRW_ERROR_COND_EXIT(**p == '\0', -1, CGSTR("Line: %d, Segment is expected to be closed with: \"%c\"."), temp_line_count, identifier);
        if (**p == identifier)
            return;
        if (**p == '\"')
            CGRWGoToNext(p, line_count, '\"');
        if (**p == '\n' && line_count != NULL)
            ++(*line_count);
    }
}

void CGRWCharToChar(const CGRWChar* str, char* buffer, unsigned int buffer_size)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, CGSTR("Cannot convert CGRWChar to char with string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(buffer == NULL, -1, CGSTR("Cannot convert CGRWChar to char with buffer pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(buffer_size == 0, -1, CGSTR("Cannot convert CGRWChar to char with buffer size \"0\"."));
    CGRW_ERROR_COND_EXIT(buffer_size < CGRW_STRLEN(str), -1, CGSTR("Cannot convert CGRWChar to char with buffer size \"%d\" and string length \"%d\"."), buffer_size, CGRW_STRLEN(str));
    unsigned int i = 0;
    for (; str[i] != (CGRWChar)'\0' && i < buffer_size; ++i)
        buffer[i] = (char)str[i];
    buffer[i] = '\0';
}

void CharToCGRWChar(const char* str, CGRWChar* buffer, unsigned int buffer_size)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, CGSTR("Cannot convert char to CGRWChar with string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(buffer == NULL, -1, CGSTR("Cannot convert char to CGRWChar with buffer pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(buffer_size == 0, -1, CGSTR("Cannot convert CGRWChar to char with buffer size \"0\"."));
    CGRW_ERROR_COND_EXIT(buffer_size < strlen(str), -1, CGSTR("Cannot convert CGRWChar to char with buffer size \"%d\" and string length \"%d\"."), buffer_size, strlen(str));
    unsigned int i = 0;
    for (; str[i] != '\0' && i < buffer_size; ++i)
        buffer[i] = (CGRWChar)str[i];
    buffer[i] = (CGRWChar)'\0';
}