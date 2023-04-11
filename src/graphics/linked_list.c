#ifdef __cplusplus
extern "C" {
#endif
#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdlib.h>

CGLinkedListNode* CGCreateLinkedListNode(void* data, int type)
{
    CGLinkedListNode* node = (CGLinkedListNode*)malloc(sizeof(CGLinkedListNode));
    CG_ERROR_COND_RETURN(node == NULL, NULL, "Failed to allocate memory for linked list node");
    node->data = data;
    node->identifier = type;
    node->next = NULL;
    return node;
}

void CGRemoveLinkedListNode(CGLinkedListNode** node)
{
    if (node == NULL || *node == NULL)
        return;
    if ((*node)->next == NULL)
    {
        free(*node);
        *node = NULL;
        return;
    }
    CGLinkedListNode* p_node = *node;
    p_node->data = p_node->next->data;
    p_node->identifier = p_node->next->identifier;
    p_node->assigned_z = p_node->next->assigned_z;
    CGLinkedListNode* temp = p_node->next->next;
    free(p_node->next);
    p_node->next = temp;
}

#ifdef __cplusplus
}
#endif