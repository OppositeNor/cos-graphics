#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

typedef struct CGLinkedListNode{
    int identifier;
    void* data;
    struct CGLinkedListNode* next;
}CGLinkedListNode;

/**
 * @brief Create a linked list node object
 * 
 * @param data linked list data
 * @param type object type
 * @return CGRenderNode* the render object instance
 */
CGLinkedListNode* CGCreateLinkedListNode(void* data, int type);

/**
 * @brief Delete a linked list node
 * 
 * @param node The node to be deleted
 */
void CGRemoveLinkedListNode(CGLinkedListNode** node);

/**
 * @brief Remove a linked list node by data.
 * 
 * @param head The head of the list.
 * @param data The data of the node to be removed.
 */
void CGRemoveLinkedListNodeByData(CGLinkedListNode** head, void* data);

/**
 * @brief Add a node to the end of the list
 * 
 * @param head The head of the list.
 * @param node The node to be added.
 */
void CGAppendListNode(CGLinkedListNode* head, CGLinkedListNode* node);

/**
 * @brief Find a node by data.
 * 
 * @param head The head of the list.
 * @param data The data of the node to be found.
 */
CGLinkedListNode* CGFindLinkedListNodeByData(CGLinkedListNode* head, void* data);

/**
 * @brief Delete list.
 * 
 * @param head The head of the list.
 */
void CGDeleteList(CGLinkedListNode* head);

/**
 * @brief Print list.
 * 
 * @param head The head of the list
 */
void CGPrintList(CGLinkedListNode* head);

#endif // __LINKED_LIST_H__