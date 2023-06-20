#include "cg_resource_wrapper/resource_wrapper.h"
#include "cg_resource_wrapper/log.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    CGRWInit(argc, argv);
    CGRWResourceData* head = CGRWPhraseUsedResource("./resource.cgures");
    CGRWResourceData* temp = head->next;
    while (temp != NULL)
    {
        CGRW_PRINT("type: %s, key: %s, path: %s", temp->type, temp->key, temp->path);
        CGRWAddResource(temp);
        CGRWResourceData* temp2 = temp;
        temp = temp->next;
        free(temp2);
    }
    free(head);
    CGRWTerminate();
    return 0;
}