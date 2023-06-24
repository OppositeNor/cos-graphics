#include "cg_resource_wrapper/resource_wrapper.h"
#include "cg_resource_wrapper/log.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    CGRWInit(argc, argv);
    CGRWResourceData* head = CGRWPhraseUsedResource("./resource.cgures");
    CGRWResourceData* temp = head;
    for (int i = 2; i < argc; i++)
    {
        CGRWResourceData* res_data = CGRWPhraseUsedResource(argv[i]);
        if (res_data != NULL)
        {
            for (; temp->next != NULL; temp = temp->next);
            temp->next = res_data;
        }
    }
    temp = head;
    while (temp != NULL)
    {
        CGRW_PRINT_VERBOSE("type: %s, key: %s, path: %s", temp->type, temp->key, temp->path);
        CGRWAddResource(temp);
        CGRWResourceData* temp2 = temp;
        temp = temp->next;
        free(temp2);
    }
    CGRWTerminate();
    return 0;
}