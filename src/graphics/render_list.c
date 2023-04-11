#ifdef __cplusplus
extern "C" {
#endif
#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdlib.h>

// get the "z" property of the render node object.
float* CGGetDepthPointer(const CGRenderNode* node);

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

void CGAddLinkedListNode(CGRenderNode* list_head, CGRenderNode* node)
{
    if (node == NULL || list_head == NULL)
        return;
    CGRenderNode* p = list_head;
    while (p->next != NULL)
    {
        if (*CGGetDepthPointer(node) > *CGGetDepthPointer(p->next))
        {
            CGRenderNode* temp = p->next;
            p->next = node;
            node->next = temp;
            return;
        }
        p = p->next;
    }
    CGRenderNode* temp = p->next;
    p->next = node;
    node->next = temp;
    
}

void CGDeleteRenderNode(CGRenderNode** node)
{
    if (node == NULL || *node == NULL)
        return;
    if ((*node)->next == NULL)
    {
        free(*node);
        *node = NULL;
        return;
    }
    CGRenderNode* p_node = *node;
    p_node->data = p_node->next->data;
    p_node->identifier = p_node->next->identifier;
    p_node->assigned_z = p_node->next->assigned_z;
    CGRenderNode* temp = p_node->next->next;
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