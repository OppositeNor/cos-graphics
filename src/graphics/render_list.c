#include "graphics.h"
#include "cos_graphics/log.h"
#include <stdlib.h>

void CGCreateRenderList(CGWindow* window)
{
    CG_ERROR_CONDITION(window == NULL, "Cannot create render list on a NULL window");
    if (window->render_list != NULL)
        free(window->render_list);
    window->render_list = (CGRenderNode*)malloc(sizeof(CGRenderNode));
    window->render_list->next = NULL;
}

float CGGetDepthFromNode(CGRenderNode* node)
{
    switch (node->type)
    {
    case CG_RD_TYPE_TRIANGLE:
        return ((CGTriangle*)node->render_object)->z;
    case CG_RD_TYPE_QUADRANGLE:
        return ((CGQuadrangle*)node->render_object)->z;
    default:
        CG_ERROR_COND_RETURN(CG_TRUE, 0, "Failed to get the z value from node: Node render type not found.");
    }
}

void CGAddRenderNode(CGWindow* window, CGRenderNode* node)
{
    CG_ERROR_CONDITION(window == NULL, "Cannot create render list on a NULL window");
    if (node == NULL || window->render_list == NULL)
        return;
    CGRenderNode* p = window->render_list;
    while (p->next != NULL)
    {
        if (CGGetDepthFromNode(node) > CGGetDepthFromNode(p))
        {
            CGRenderNode* temp = p->next;
            p->next = node;
            node->next = temp;
            return;
        }
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
    CGRenderNode* temp = p_node->next->next;
    free(p_node->next);
    p_node->next = temp;
}