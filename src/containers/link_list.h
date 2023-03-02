/**
 * @file link_list.h
 * @author OppositeNor
 * @brief Link list container
 * @date 2023-02-22
 * 
 * 
 */

#ifndef _LINK_LIST_H_
#define _LINK_LIST_H_

/**
 * @brief List node
 */
typedef struct LinkListNode{
    /**
     * @brief Data of the list
     */
    void* data;

    /**
     * @brief Data size
     */
    unsigned int size;
    
    /**
     * @brief Pointer that points to the next node in the list
     */
    struct LinkListNode* next;
} LinkListNode;

/**
 * @brief Create a list
 * 
 * @param num The number of nodes
 * @param ... The list of the nodes in the created list
 * @return LinkListNode* The head node of the created list
 */
LinkListNode* LLCreateList(int num, ...);
/**
 * @brief Delete the list
 * 
 * @param head the head node of the list
 */
void LLDeleteList(LinkListNode* head);
/**
 * @brief Create a node
 * 
 * @param data node data
 * @param size data size. use sizeof(data).
 * @return LinkListNode* the allocated memory of the new node
 */
LinkListNode* LLCreateNode(void* data, unsigned int size);
/**
 * @brief Free the node resource
 * @details NOTE: DONOT free the node while the node is still in the list. If you want to 
 * delete a node that's still in the list, use LDeleteNode instead.
 * @param node 
 */
void LLFreeNode(LinkListNode* node);
/**
 * @brief Get the last node of a list. Remove the last node from the list.
 * Note that this is a heap object. You should call LFreeNode to free resource for the node.
 * @param head The head node of the list
 * @return LinkListNode* the last node of the list
 */
LinkListNode* LLPop(LinkListNode* head);
/**
 * @brief Add a node to the end of the list.
 * 
 * @param head The head of the list.
 * @param node node added to the list.
 */
void LLAppendNode(LinkListNode* head, LinkListNode* node);
/**
 * @brief Copy the content of the node.
 * 
 * @param from The node where to copy from
 * @return LinkListNode* the copied node
 */
LinkListNode* LLCopyNode(LinkListNode* from);
/**
 * @brief Get the data from a node.
 * 
 * @param node The node where to get the data from
 * @param data The data gotten.
 */
void LLGetLinkListNodeData(LinkListNode* node, void* data);
/**
 * @brief Delete a node at index from the list.
 * 
 * @param head The head node of the list
 * @param index The index of the node to be deleted
 */
void LLDeleteNode(LinkListNode* head, unsigned int index);
/**
 * @brief Get node at index
 * 
 * @param head The head node of the list
 * @param index The index of the node to be returned
 * @return LinkListNode* 
 */
LinkListNode* LLGetNode(LinkListNode* head, unsigned int index);

#endif  // _LINK_LIST_H_