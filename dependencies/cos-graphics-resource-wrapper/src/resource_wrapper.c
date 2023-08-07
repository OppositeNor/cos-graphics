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
 * @brief Byte array.
 */
typedef struct {
    CGRWChar* data;         /*The data of the array.*/
    unsigned int size;  /*The size of the array.*/
}CGRWByteArray;

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

static void CGRWGetSubString(const CGRWChar* str, unsigned int start, unsigned int end, CGRWChar* sub_string);

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

static inline CGRW_BOOL CGRWIsLetter(CGRWChar c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-'; }

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
    CharToCGRWChar(argv[1], buff);
    CGRW_PRINT(CGSTR("Resource path: %ls\n"), buff);
    unsigned int cgrw_buffer_length = (CGRW_STRLEN(buff) + CGRW_STRLEN(cgrw_resource_file_name) + 2) * sizeof(CGRWChar);
    cgrw_resource_file_path = (CGRWChar*)malloc(cgrw_buffer_length);
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL, -1, CGSTR("Failed to allocate memory for resource file path."));
    CGRW_SPRINTF(cgrw_resource_file_path, cgrw_buffer_length, CGSTR("%ls%lc%ls"), buff, FILE_SPLITTER, cgrw_resource_file_name);
    CGRW_PRINT(CGSTR("Resource file path: %ls"), cgrw_resource_file_path);
    cgrw_buffer_length = (CGRW_STRLEN(buff) + CGRW_STRLEN(cgrw_resource_finder_name) + 2) * sizeof(CGRWChar);
    cgrw_resource_finder_path = (CGRWChar*)malloc(cgrw_buffer_length);
    CGRW_ERROR_COND_EXIT(cgrw_resource_finder_path == NULL, -1, CGSTR("Failed to allocate memory for resource finder path."));
    CGRW_SPRINTF(cgrw_resource_finder_path, cgrw_buffer_length, CGSTR("%ls%lc%ls"), buff, FILE_SPLITTER, cgrw_resource_finder_name);
    CGRW_PRINT(CGSTR("Resource finder path: %ls"), cgrw_resource_finder_path);
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
#ifdef CGRW_USE_WCHAR
    CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to open file at path: %ls."), file_path);
#else
    CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to open file at path: %s."), file_path);
#endif
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    CGRWByteArray file_data;
    file_data.data = (CGRWChar*)malloc((file_size + 1) * sizeof(CGRWChar));
    if (file_data.data == NULL)
    {
        fclose(file);
        CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Failed to allocate memory for file data."));
    }
    rewind(file);
    fread(file_data.data, sizeof(CGRWChar), file_size, file);
    file_data.data[file_size] = '\0';
    file_data.size = file_size;
    fclose(file);
    return file_data;
}

void CGRWAddResource(CGRWResourceData* res_data)
{
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL || cgrw_resource_finder_path == NULL, -1, CGSTR("Cannot add resource: Resource wrapper is not initialized."));
    CGRW_ERROR_COND_EXIT(res_data == NULL, -1, CGSTR("Cannot add resource: Resource data is NULL."));
#ifdef CGRW_USE_WCHAR
    CGRW_PRINT(CGSTR("Adding resource: %ls."), res_data->key);
#else
    CGRW_PRINT(CGSTR("Adding resource: %s."), res_data->key);
#endif
    CGRWByteArray data = CGRWLoadFile(res_data->path);
    // write in resource file
    FILE* file = CGRWFOpen(cgrw_resource_file_path, "rb+");
    if (file == NULL)
    {
        file = CGRWFOpen(cgrw_resource_file_path, "wb");
        CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to create resource file."));
    }
    fseek(file, 0, SEEK_END);
    unsigned int data_location = ftell(file);
    fwrite(data.data, 1, data.size, file);
    fclose(file);
    free(data.data);
    
    // write in resource finder
    file = CGRWFOpen(cgrw_resource_finder_path, "ab");
    
    fwrite(res_data->key, 1, sizeof(CGRWChar) * CGRW_STRLEN(res_data->key) + 1, file);
    fwrite(&data_location, sizeof(unsigned int), 1, file);
    fwrite(&data.size, sizeof(data.size), 1, file);
    fwrite(&res_data->type, sizeof(CGRWChar) * CGRW_STRLEN(res_data->type), 1, file);
    CGRWChar end = '\n';
    fwrite(&end, sizeof(end), 1, file);

    fclose(file);
}

CGRWChar* CGRWLoadResource(const CGRWChar* resource_key)
{
    FILE* file = CGRWFOpen(cgrw_resource_finder_path, "rb");
#ifdef CGRW_USE_WCHAR
    CGRW_PRINT(CGSTR("Loading resource: %ls."), resource_key);
#else
    CGRW_PRINT(CGSTR("Loading resource: %s."), resource_key);
#endif
    CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to open resource finder file."));
    CGRWChar buff[256];
    unsigned int i = 0;
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    for(;;)
    {
        fread(&buff[i], sizeof(CGRWChar), 1, file);
        if (ftell(file) >= file_size)
        {
            fclose(file);
#ifdef CGRW_USE_WCHAR
            CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Failed to find resource with key: %ls."), resource_key);
#else
            CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Failed to find resource with key: %s."), resource_key);
#endif
        }
        if (buff[i] != '\0')
        {
            ++i;
            continue;
        }
        if (CGRW_STRCMP(buff, resource_key) == 0)
        {
            unsigned int data_location;
            fread(&data_location, sizeof(unsigned int), 1, file);
            unsigned int data_size;
            fread(&data_size, sizeof(unsigned int), 1, file);
            fclose(file);
            file = CGRWFOpen(cgrw_resource_file_path, "rb");
            CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to open resource file."));
            fseek(file, data_location, SEEK_SET);
            CGRWChar* data = (CGRWChar*)malloc(data_size * sizeof(CGRWChar) + 1);
            CGRW_ERROR_COND_EXIT(data == NULL, -1, CGSTR("Failed to allocate memory for resource data."));
            fread(data, sizeof(CGRWChar), data_size, file);
            data[data_size] = '\0';
            fclose(file);
            return data;
        }
        for (;;)
        {
            CGRWChar p;
            fread(&p, sizeof(CGRWChar), 1, file);
            if (p == '\n')
                break;
            if (ftell(file) == file_size)
            {
                fclose(file);
#ifdef CGRW_USE_WCHAR
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Failed to find resource with key: %ls."), resource_key);
#else
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Failed to find resource with key: %s."), resource_key);
#endif
            }
        }
        i = 0;
    }

    fclose(file);
#ifdef CGRW_USE_WCHAR
    CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Failed to find resource with key: %ls."), resource_key);
#else
    CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Failed to find resource with key: %s."), resource_key);
#endif
}

CGRWResourceData* CGRWPhraseUsedResource(const CGRWChar* file_path)
{
#ifdef CGRW_USE_WCHAR
    CGRW_PRINT(CGSTR("Phrasing resource at path: %ls"), file_path);
#else
    CGRW_PRINT(CGSTR("Phrasing resource at path: %s"), file_path);
#endif
    CGRW_ERROR_COND_EXIT(cgrw_resource_file_path == NULL || cgrw_resource_finder_path == NULL, -1, CGSTR("Cannot phrase resource: Resource wrapper is not initialized."));
    CGRW_ERROR_COND_EXIT(file_path == NULL, -1, CGSTR("Cannot phrase resource: File path is NULL."));
    FILE* file = CGRWFOpen(file_path, "r");
    CGRW_PRINT_VERBOSE(CGSTR("Opening file at path: %s"), file_path);
#ifdef CGRW_USE_WCHAR
    CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to find resource file: %ls."), file_path);
#else
    CGRW_ERROR_COND_EXIT(file == NULL, -1, CGSTR("Failed to find resource file: %s."), file_path);
#endif
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file) / sizeof(CGRWChar);
    rewind(file);
    CGRWChar* file_data = (CGRWChar*)malloc((file_size + 1) * sizeof(CGRWChar));
    CGRW_ERROR_COND_EXIT(file_data == NULL, -1, CGSTR("Failed to allocate memory for file data."));
    fread(file_data, sizeof(CGRWChar), file_size, file);
    file_data[file_size] = '\0';
    CGRW_PRINT_VERBOSE(CGSTR("File loaded with size: %d"), file_size);
    fclose(file);

    CGRWDeleteComments(file_data);

    CGRWResourceData* data_head = (CGRWResourceData*)malloc(sizeof(CGRWResourceData));

    CGRWPhraseData(file_data, data_head);
    free(file_data);

    CGRW_PRINT(CGSTR("Resource phrase finished."));
    CGRWResourceData* p = data_head;

    // check is resource valid
    CGRW_PRINT_VERBOSE(CGSTR("Checking is resource valid..."));

    for (unsigned int i = 0; p != NULL; p = p->next)
    {
        CGRW_PRINT_VERBOSE(CGSTR("Checking resource index %d..."), i);
        CGRW_ERROR_COND_EXIT(p->type[0] == '\0', -1, CGSTR("Resource type is empty."));
        CGRW_ERROR_COND_EXIT(p->key[0] == '\0', -1, CGSTR("Resource key is empty."));
        CGRW_ERROR_COND_EXIT(p->path[0] == '\0', -1, CGSTR("Resource path is empty."));
        for (CGRWResourceData* q = p->next; q != NULL; q = q->next)
        {
            if (CGRW_STRCMP(p->key, q->key) == 0)
            {
#ifdef CGRW_USE_WCHAR
                CGRW_PRINT_VERBOSE(CGSTR("Resource key: %ls is duplicated."), p->key);
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Resource key: %ls is duplicated."), p->key);
#else
                CGRW_PRINT_VERBOSE(CGSTR("Resource key: %s is duplicated."), p->key);
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Resource key: %s is duplicated."), p->key);
#endif
            }
        }
#ifdef CGRW_USE_WCHAR
        CGRW_PRINT_VERBOSE(CGSTR("type: %ls"), p->type);
        CGRW_PRINT_VERBOSE(CGSTR("key: %ls"), p->key);
        CGRW_PRINT_VERBOSE(CGSTR("path: %ls"), p->path);
#else
        CGRW_PRINT_VERBOSE(CGSTR("type: %s"), p->type);
        CGRW_PRINT_VERBOSE(CGSTR("key: %s"), p->key);
        CGRW_PRINT_VERBOSE(CGSTR("path: %s"), p->path);
#endif
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
    data_head->type[0] = '\0';
    data_head->key[0] = '\0';
    data_head->path[0] = '\0';
    data_head->next = NULL;
    
    CGRWResourceData* data = data_head;
    
    for (CGRWChar* p = (CGRWChar*)file_data;;++p)
    {
        CGRWSkipSpaces(&p, &line_count);
        switch(*p)
        {
        case '\0':
            CGRW_PRINT_VERBOSE(CGSTR("Data successfully phrased."));
            return;
        case ';':
            break;
        case '\"':
            CGRWGoToNext(&p, &line_count, '\"');
            break;
        case '[':
            {
                CGRW_PRINT_VERBOSE(CGSTR("Found resource at line: %d"), line_count);
                if (data != data_head || data->type[0] != '\0' || data->key[0] != '\0' || data->path[0] != '\0')
                {
                    CGRWResourceData* t_data = (CGRWResourceData*)malloc(sizeof(CGRWResourceData));
                    CGRW_ERROR_COND_EXIT(t_data == NULL, -1, CGSTR("Failed to allocate memory for resource data."));
                    data->next = t_data;
                    data = t_data;
                    data->key[0] = '\0';
                    data->path[0] = '\0';
                    data->type[0] = '\0';
                    data->next = NULL;
                }
                CGRWChar* t_p = p;
                CGRWGoToNext(&p, &line_count, ']');
                CGRW_ERROR_COND_EXIT(t_p == p, -1, CGSTR("Data type cannot be empty."), line_count);
                CGRWGetSubString(t_p, 1, p - t_p - 1, data->type);
#ifdef CGRW_USE_WCHAR
                CGRW_PRINT_VERBOSE(CGSTR("Resource type: \"%ls\"."), data->type);
#else
                CGRW_PRINT_VERBOSE(CGSTR("Resource type: \"%s\"."), data->type);
#endif
            }break;
        case '{':
            CGRW_ERROR_COND_EXIT(data == NULL, -1, CGSTR("Invalid resource file format at line: %d."), line_count);
            CGRWPhraseChunk(p, line_count, data);
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
#ifdef CGRW_USE_WCHAR
                CGRW_ERROR_COND_EXIT(*p != ';', -1, CGSTR("Invalid resource file format at line: %d, expected character: \';\', but get \'%lc\' instead."), line_count, *p);
#else
                CGRW_ERROR_COND_EXIT(*p != ';', -1, CGSTR("Invalid resource file format at line: %d, expected character: \';\', but get \'%c\' instead."), line_count, *p);
#endif
                return;
            }
#ifdef CGRW_USE_WCHAR
            CGRW_ERROR_COND_EXIT(!CGRWIsLetter(*p) && *p != '\"' && *p != ';', -1, CGSTR("Invalid resource file format at line: %d, unexpected character: \'%lc\'."), line_count, *p);
#else
            CGRW_ERROR_COND_EXIT(!CGRWIsLetter(*p) && *p != '\"' && *p != ';', -1, CGSTR("Invalid resource file format at line: %d, unexpected character: \'%c\'."), line_count, *p);
#endif
            CGRWChar buff[256];
            CGRWGetFirstWord(p, buff);
            p += CGRW_STRLEN(buff);
            CGRWSkipSpaces(&p, &line_count);
            if (*p != '=')
#ifdef CGRW_USE_WCHAR
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Invalid resource file format at line: %d, expected character: \'=\', but get \'%lc\' instead."), line_count, *p);
#else
                CGRW_ERROR_COND_EXIT(CGRW_TRUE, -1, CGSTR("Invalid resource file format at line: %d, expected character: \'=\', but get \'%c\' instead."), line_count, *p);
#endif
            ++p;
            CGRWSkipSpaces(&p, &line_count);
            CGRWChar* t_p = p;
            CGRWGoToNext(&t_p, &line_count, '"');
            if (CGRW_STRCMP(buff, CGSTR("key")) == 0)
            {
                CGRWGetSubString(p, 0, t_p - p, data->key);
#ifdef CGRW_USE_WCHAR
                CGRW_PRINT_VERBOSE(CGSTR("key: \"%ls\""), data->key);
#else
                CGRW_PRINT_VERBOSE(CGSTR("key: \"%s\""), data->key);
#endif
            }
            else if (CGRW_STRCMP(buff, CGSTR("path")) == 0)
            {
                CGRWGetSubString(p, 0, t_p - p, data->path);
#ifdef CGRW_USE_WCHAR
                CGRW_PRINT_VERBOSE(CGSTR("path: \"%ls\""), data->path);
#else
                CGRW_PRINT_VERBOSE(CGSTR("path: \"%s\""), data->path);
#endif
            }
            p = t_p + 1;
            CGRWSkipSpaces(&p, &line_count);
#ifdef CGRW_USE_WCHAR
            CGRW_ERROR_COND_EXIT(*p != ';', -1, CGSTR("Invalid resource file format at line: %d, expected character: \';\', but get \'%lc\' instead."), line_count, *p);
#else
            CGRW_ERROR_COND_EXIT(*p != ';', -1, CGSTR("Invalid resource file format at line: %d, expected character: \';\', but get \'%c\' instead."), line_count, *p);
#endif
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
        case ' ':
        case 0x000D:
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
    CGRWGetSubString(p, 0, t_p - p, buff);
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
        case '\0':
            CGRW_PRINT_VERBOSE(CGSTR("Comments removed"));
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
}

static void CGRWGetSubString(const CGRWChar* str, unsigned int start, unsigned int end, CGRWChar* sub_string)
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
    for (unsigned int i = 0; i <= end - start; ++i)
        sub_string[i] = str[start + i];
    sub_string[end - start + 1] = '\0';
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
#ifdef CGRW_USE_WCHAR
        CGRW_ERROR_COND_EXIT(**p == '\0', -1, CGSTR("Line: %d, Segment is expected to be closed with: \"%lc\"."), temp_line_count, identifier);
#else
        CGRW_ERROR_COND_EXIT(**p == '\0', -1, CGSTR("Line: %d, Segment is expected to be closed with: \"%c\"."), temp_line_count, identifier);
#endif
        if (**p == identifier)
            return;
        if (**p == '\n' && line_count != NULL)
            ++(*line_count);
    }
}

void CGRWCharToChar(const CGRWChar* str, char* buffer)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, CGSTR("Cannot convert CGRWChar to char with string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(buffer == NULL, -1, CGSTR("Cannot convert CGRWChar to char with buffer pointer \"NULL\"."));
    unsigned int i = 0;
    for (; str[i] != (CGRWChar)'\0'; ++i)
        buffer[i] = (char)str[i];
    buffer[i] = '\0';
}

void CharToCGRWChar(const char* str, CGRWChar* buffer)
{
    CGRW_ERROR_COND_EXIT(str == NULL, -1, CGSTR("Cannot convert char to CGRWChar with string pointer \"NULL\"."));
    CGRW_ERROR_COND_EXIT(buffer == NULL, -1, CGSTR("Cannot convert char to CGRWChar with buffer pointer \"NULL\"."));
    unsigned int i = 0;
    for (; str[i] != '\0'; ++i)
        buffer[i] = (CGRWChar)str[i];
    buffer[i] = (CGRWChar)'\0';
}