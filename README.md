# Red-Black Tree in C - threaded.
Red-Black Tree in C/C++, threaded.
Red Black Tree in C \(and C++), threaded, generic and with support 
for multible trees on same node \(as a secondary 'index').

To build the library, in src directory, type:
```
    make
```
This build the librbt.a file.

## Tutorial/how to:

### Initial work

* Define your own node, add variables for the rbt_ functions \(2 pointers and a char per tree). Eg:
```c
#include <stddef.h> // for size_t, and offsetof
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rbt.h"

typedef struct  {
    void *left;       // used by rbt_ functions
    void *right;      // used by rbt_ functions
    char  color;      // used by rbt_ functions
    char  key[40];    // primary unique key
    char  data[40];   // data
} myNode;

void myNode_freeNode( myNode  * r );

myNode * myNode_newNode( const char * key, const char * data );

extern RBTDEF * myNode_DEF;
```

* Create 4 supporting functions (allocation, deallocation and 2 compare functions), Eg:

```c
myNode * myNode_newNode( const char * key, const char * data ) {
    myNode  * r ;
    r = malloc(sizeof(myNode));
    if(r==NULL)
        return NULL;
    snprintf( r->key,  sizeof(r->key),  "%s", key  );
    snprintf( r->data, sizeof(r->data), "%s", data );
    return r;
}

void myNode_freeNode( myNode  * r ) {
    free( r );
}

static int myNode_compareNode( myNode * r1, myNode * r2 ) {
    return strcmp( r1->key, r2->key );
}

static int myNode_compareKey( myNode * r1, char * key ) {
    return strcmp( r1->key, key );
}
```

* Define a description record for myNode

```c
RBTDEF * myNode_DEF = (RBTDEF[]) {{
    .left_ofs  = offsetof( myNode, left  ),          /* offsetof to left child */
    .right_ofs = offsetof( myNode, right ),          /* offsetof to right child */
    .color_ofs = offsetof( myNode, color ),          /* offsetof to color attribute (char) */
    .nodeCmp   = (int (*)(void *, void *)) myNode_compareNode, /* compare function for nodes */
    .keyCmp    = (int (*)(void *, void *)) myNode_compareKey,  /* compare function for key */
    .allocRoot = (void *(*)(size_t))       malloc,         /* malloc function for root data */
    .freeRoot  = (void (*)(void *))        free,           /* free function for root data */
    .freeNode  = (void (*)(void *))        myNode_freeNode /* free function for node */
}};
```

* Create a new rbt.

```c
RBT * tree;
tree  = rbt_new( myNode_DEF ); 
if ( tree == NULL ) {} // error...
```

### Insertion functions

* Inserting:

```c
int rc;
rc = rbt_insert( tree, myNode_newNode( "key001", "data001" ) );
if( rc != 0 ) {} // error...
```

* Inserting, but keep old replaced node (if exist);

```c
int rc;
myNode * oldnode;
myNode * newnode = myNode_newNode( "key001", "data001" );
rc = rbt_insert_keep( tree, newnode, (void**)&oldnode );
if( rc != 0 ) {} // error...
if( oldnode != NULL ) {
    printf("'%s' changed data from '%s' to '%s'\n",
        newnode->key, oldnode->data, newnode->data);
    myNode_freeNode( oldnode ); 
}else {
    printf("'%s' new data '%s'\n", newnode->key, newnode->data);
}
```

### Delete functions

* delete a node \(by key)

```c
int rc;
rc = rbt_delkey( tree, "key001" );
if( rc == -1 ) {} // error...
if( rc == 0 ) {} // ok...
if( rc == 1 ) {} // not found
```

* other delete functions:

```c
myNode * node;
myNode * oldnode;
...
rc = rbt_delkey_keep( tree, "key001", &oldnode );
// delete by found node:
rc = rbt_delnode( tree, node );
rc = rbt_delnode_keep( tree, node, &oldnode );
// rc: -1==error, 0==ok, 1==not found..
```

### Traversal functions

* search

```c
myNode * node;
node = rbt_get( tree, "key001" );
if( node == NULL ) {...} // not found
```

* list all nodes:

```c
// from left to right:
myNode * node;
for(node=rbt_first(tree);node!=NULL;node=rbt_next(tree,node))
{
    printf("'%s' : '%s'\n", node->key, node->data );
}

// from right to left 
myNode * node;
for(node=rbt_last(tree);node!=NULL;node=rbt_prev(tree,node))
{
    printf("'%s' : '%s'\n", node->key, node->data );
}
```

* range search functions

```c
myNode * node_sta, * node_end;
// helper function. In this example range is a 2 char string 
// containing lowest and highest starting character in a range.
int compareRange( myNode * node, char * range )
{
    if( node->key[0] < range[0] ) return -1; // to low
    if( node->key[0] > range[1] ) return 1;  // to high
    return 0; // match
}

// node points to first node with key starting with 'B' or 'C'
node_sta = rbt_find_feq( tree, (int(*)(void*,void*))compareRange, "BC" );

// node points to last node with key starting with 'B' or 'C'
node_end = rbt_find_leq( tree, (int(*)(void*,void*))compareRange, "BC" );

// rbt_next and rbt_prev can now be used to traverse the range.
for(node=node_sta ;node!=NULL;node=rbt_next(tree,node))
{
    printf("'%s' : '%s'\n", node->key, node->data );
    if(node==node_end) 
        break;
}
```

### Cleanup and freeing

* Clear all nodes \(but not the tree itself).
```c
rbt_clr( tree )
```

* Free complete tree with all its nodes.
```c
rbt_free( tree )
```

## Samples:

* The above as a samples c-file:
```
samples/sampRBTc01.c
```

* Multible tree support example / C++ example:
```
samples/sampRBTcpp01.cpp - multi tree in C++. 
```

## Tested
   gcov
