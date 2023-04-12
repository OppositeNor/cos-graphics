
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