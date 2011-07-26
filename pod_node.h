#ifndef INCLUDE_POD_NODE_H
#define INCLUDE_POD_NODE_H

#include <stddef.h>



// A node is a place on a list.  It simplifies doubly linked lists because the
// header is just another node.  Everything in Pod is a node, potentially a
// part of a list or map.  Header nodes should be initialized to point to
// themselves.  Otherwise, initialize them to NULL.
//
// For example:
//      pod_node header;
//
//      header.previous = &header;
//      header.next = &header;


struct pod_node;
typedef struct pod_node pod_node;

struct pod_node {
    pod_node *previous;
    pod_node *next;
};


    // Other pod_node related functions

// extern pod_node *pod_node_remove(pod_node *node);
extern inline pod_node *pod_node_remove(pod_node *node) 
{
    pod_node *next;

    next = node->next;
    node->previous->next = node->next;
    node->next->previous = node->previous;
    node->previous = node->next = NULL;

    return next;
}
 


#endif /*INCLUDE_POD_NODE_H */
