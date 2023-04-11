#ifdef __cplusplus
extern "C" {
#endif
#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdlib.h>

// get the "z" property of the render node object.
float* CGGetDepthPointer(const CGRenderNode* node);

CGLinkedListNode* CGCreateLinkedListNode(void* data, int type)
{
    CGLinkedListNode* node = (CGLinkedListNode*)malloc(sizeof(CGLinkedListNode));
    CG_ERROR_COND_RETURN(node == NULL, NULL, "Failed to allocate memory for linked list node");
    node->data = data;
    node->identifier = type;
    node->next = NULL;
    return node;
}

void CGCreateRenderList(CGWindow* window)
{
    CG_ERROR_CONDITION(window == NULL, "Cannot create render list on a NULL window");
    if (window->render_list != NULL)
        free(window->render_list);
    window->render_list = (CGRenderNode*)malloc(sizeof(CGRenderNode));
    window->render_list->next = NULL;
}

float* CGGetDepthPointer(const CGRenderNode* node)
{
    switch (node->identifier)
    {
    case CG_RD_TYPE_TRIANGLE:
        return &((CGTriangle*)node->data)->z;
    case CG_RD_TYPE_QUADRANGLE:
        return &((CGQuadrangle*)node->data)->z;
    case CG_RD_TYPE_SPRITE:
        return &((CGSprite*)node->data)->z;
    default:
        CG_ERROR_COND_RETURN(CG_TRUE, 0, "Failed to get the z value from node: Cannot find render type: %d.", node->identifier);
    }
}

void CGAddRenderListNode(CGLinkedListNode* list_head, CGLinkedListNode* node)
{
    if (node == NULL || list_head == NULL)
        return;
    CGLinkedListNode* p = list_head;
    while (p->next != NULL)
    {
        if (*CGGetDepthPointer(node) > *CGGetDepthPointer(p->next))
        {
            CGLinkedListNode* temp = p->next;
            p->next = node;
            node->next = temp;
            return;
        }
        p = p->next;
    }
    CGLinkedListNode* temp = p->next;
    p->next = node;
    node->next = temp;
    
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

void CGReorganizeRenderList(CGWindow* window)
{
    CG_ERROR_CONDITION(window == NULL, "Cannot reorganize list with a NULL window");
    if(window->render_list == NULL)
        return;
    CGRenderNode* p = window->render_list->next;
    float assign_z = CG_RENDER_FAR;
    while(p != NULL)
    {
        assign_z -= 0.1;
        p->assigned_z = assign_z;
        p = p->next;
    }
}

#ifdef __cplusplus
}
#endif