#include "cg_resource_wrapper/resource_wrapper.h"
#include "cg_resource_wrapper/log.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    CGRWInit(argc, argv);
    CGRWResourceData* head = CGRWPhraseUsedResource(CGSTR("./resource.cgures"));
    CGRWResourceData* temp = head;
    for (int i = 2; i < argc; i++)
    {
#ifdef CGRW_USE_WCHAR
        CGRWChar buff[256];
        CharToCGRWChar(argv[1], buff);
        CGRWResourceData* res_data = CGRWPhraseUsedResource(buff);
#else
        CGRWResourceData* res_data = CGRWPhraseUsedResource(argv[i]);
#endif
        if (res_data != NULL)
        {
            for (; temp->next != NULL; temp = temp->next);
            temp->next = res_data;
        }
    }
    temp = head;
    while (temp != NULL)
    {
#ifdef CGRW_USE_WCHAR
        CGRW_PRINT_VERBOSE(CGSTR("type: %ls, key: %ls, path: %ls"), temp->type, temp->key, temp->path);
#else
        CGRW_PRINT_VERBOSE(CGSTR("type: %s, key: %s, path: %s"), temp->type, temp->key, temp->path);
#endif
        CGRWAddResource(temp);
        CGRWResourceData* temp2 = temp;
        temp = temp->next;
        free(temp2);
    }
    CGRWTerminate();
    return 0;
}