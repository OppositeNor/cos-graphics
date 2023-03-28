#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdlib.h>

// get the "z" property of the render node object.
float* CGGetDepthPointer(CGRenderNode* node);

void CGCreateRenderList(CGWindow* window)
{
    CG_ERROR_CONDITION(window == NULL, "Cannot create render list on a NULL window");
    if (window->render_list != NULL)
        free(window->render_list);
    window->render_list = (CGRenderNode*)malloc(sizeof(CGRenderNode));
    window->render_list->next = NULL;
}

float* CGGetDepthPointer(CGRenderNode* node)
{
    switch (node->type)
    {
    case CG_RD_TYPE_TRIANGLE:
        return &((CGTriangle*)node->render_object)->z;
    case CG_RD_TYPE_QUADRANGLE:
        return &((CGQuadrangle*)node->render_object)->z;
    case CG_RD_TYPE_SPRITE:
        return &((CGSprite*)node->render_object)->z;
    default:
        CG_ERROR_COND_RETURN(CG_TRUE, 0, "Failed to get the z value from node: Cannot find render type: %d.", node->type);
    }
}

CGRenderNode* CGCreateRenderNode(void* render_object, enum CGObjectType type)
{
    CGRenderNode* node = (CGRenderNode*)malloc(sizeof(CGRenderNode));
    CG_ERROR_COND_RETURN(node == NULL, NULL, "Failed to allocate memory for node");
    node->render_object = render_object;
    node->type = type;
    node->next = NULL;
    return node;
}

void CGAddRenderNode(CGWindow* window, CGRenderNode* node)
{
    CG_ERROR_CONDITION(window == NULL, "Cannot create render list on a NULL window");
    if (node == NULL || window->render_list == NULL)
        return;
    CGRenderNode* p = window->render_list;
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
    p_node->render_object = p_node->next->render_object;
    p_node->type = p_node->next->type;
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