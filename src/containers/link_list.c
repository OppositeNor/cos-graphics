/**
 * @file link_list.c
 * @author OppositeNor
 * @brief Link list container
 * @version 0.1
 * @date 2023-02-22
 * 
 * 
 */

#include "link_list.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
LinkListNode* LLCreateList(int num, ...)
{
    if (num == 0)
        return NULL;
    va_list args;
    va_start(args, num);

    LinkListNode* head = va_arg(args, LinkListNode*);
    LinkListNode* p = head;
    for (int i = 0; i < num; ++i)
    {
        p->next = va_arg(args, LinkListNode*);
        p = p->next;
    }
    va_end(args);
    return head;
}

void LLDeleteList(LinkListNode* head)
{
    LinkListNode* p = head;
    while (p!= NULL)
    {
        LinkListNode* next = p->next;
        LLFreeNode(p);
        p = next;
    }
}

LinkListNode* LLCreateNode(void* data, unsigned int size)
{
    LinkListNode* node = (LinkListNode*)malloc(sizeof(LinkListNode));
    if (node == NULL)
        return NULL;
    
    node->data = (void*)malloc(size);
    if (node->data == NULL)
    {
        free(node);
        return NULL;
    }
    memcpy(node->data, data, size);

    node->size = size;
    node->next = NULL;
    return node;
}

void LLFreeNode(LinkListNode* node)
{
    if (node == NULL)
        return;
    if (node->data != NULL && node->size != 0)
        free(node->data);
    free(node);
    node = NULL;
}

LinkListNode* LLPop(LinkListNode* head)
{
    if (head == NULL)
        return head;
    LinkListNode* p = head;
    while (p->next != NULL) p++;
    LinkListNode* result = p->next;
    p->next = NULL;
    return result;
}

void LLAppendNode(LinkListNode* head, LinkListNode* node)
{
    if (head == NULL)
        return;
    LinkListNode* p = head;
    while (p->next != NULL)
        p = p->next;
    p->next = node;
}

void LLGetLinkListNodeData(LinkListNode* node, void* data)
{
    if (node == NULL || node->data == NULL || node->size == 0)
        return;
    memcpy(data, node->data, node->size);
}

LinkListNode* LLCopyNode(LinkListNode* from)
{
    LinkListNode* result = (LinkListNode*)malloc(sizeof(LinkListNode));
    LLGetLinkListNodeData(from, result->data);
    result->size = from->size;
    return result;
}

void LLDeleteNode(LinkListNode* head, unsigned int index)
{
    if (head == NULL)
        return;
    if (index == 0)
    {
        LLFreeNode(head);
        return;
    }
    LinkListNode* p = head;
    for (int i = 0; i < index - 1; ++i)
    {
        p = p->next;
        if (p == NULL)
            return;
    }
    LinkListNode* temp = p->next;
    p->next = temp->next;
    LLFreeNode(temp);
}


LinkListNode* LLGetNode(LinkListNode* head, unsigned int index)
{
    if (head == NULL)
        return NULL;
    if (index == 0)
        return head;
    LinkListNode* p = head;
    for (int i = 0; i < index; ++i)
    {
        p = p->next;
        if (p == NULL)
            return NULL;
    }
    return p;
}