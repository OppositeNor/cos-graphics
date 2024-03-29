#ifdef __cplusplus
extern "C" {
#endif
#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdlib.h>

CGLinkedListNode* CGCreateLinkedListNode(void* data, int type)
{
    CGLinkedListNode* node = (CGLinkedListNode*)malloc(sizeof(CGLinkedListNode));
    CG_ERROR_COND_RETURN(node == NULL, NULL, CGSTR("Failed to allocate memory for linked list node"));
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
    CGLinkedListNode* temp = p_node->next->next;
    free(p_node->next);
    p_node->next = temp;
}

void CGRemoveLinkedListNodeByData(CGLinkedListNode** head, void* data)
{
    CG_ERROR_CONDITION(head == NULL || *head == NULL, CGSTR("Failed to remove data from list: The head of the list is null."));
    CGLinkedListNode* p = *head;
    if (p->data == data)
    {
        *head = p->next;
        free(p);
        return;
    }
    for (; p->next != NULL; p = p->next)
    {
        if (p->next->data == data)
        {
            CGLinkedListNode* temp = p->next;
            p->next = p->next->next;
            free(temp);
            return;
        }
    }
    CG_WARNING(CGSTR("Failed to remove data from list: The data is not in the list."));
}

void CGAppendListNode(CGLinkedListNode* head, CGLinkedListNode* node)
{
    CGLinkedListNode* p = head;
    for (; p->next != NULL; p = p->next);
    p->next = node;
}

CGLinkedListNode* CGFindLinkedListNodeByData(CGLinkedListNode* head, void* data)
{
    for (CGLinkedListNode* p = head; p != NULL; p = p->next)
    {
        if (p->data == data)
            return p;
    }
    CG_WARNING(CGSTR("Failed to find data from list: The data is not in the list. Returning NULL..."));
    return NULL;
}

void CGDeleteList(CGLinkedListNode* head)
{
    if (head == NULL)
        return;
    CGLinkedListNode* p_node = head;
    while (p_node != NULL)
    {
        CGLinkedListNode* temp = p_node->next;
        free(p_node);
        p_node = temp;
    }
}

void CGPrintList(CGLinkedListNode* head)
{
    if (head == NULL)
        return;
    CG_PRINT(CGSTR("Print list with head %p: "), head);
    CGLinkedListNode* p_node = head;
    unsigned int i = 0;
    while (p_node != NULL)
    {
        CG_PRINT(CGSTR("Node: %d, Identifier: %d, Data location: %p"), i, p_node->identifier, p_node->data);
        p_node = p_node->next;
        ++i;
    }
    CG_PRINT(CGSTR("Print list end. Total %d nodes.\n"), i);
}

#ifdef __cplusplus
}
#endif