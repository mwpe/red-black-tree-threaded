/*********************************************************************
* Red Black Tree functions (threaded)
*
* rbt.h
*
**********************************************************************
*
* header file for all the rbt_ functions.
*
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/
#ifndef RBT_H_
#define RBT_H_

#ifdef __cplusplus
#include <cstddef>  /* for size_t, offsetof */
#else
#include <stddef.h>  /* for size_t, offsetof */
#endif

#ifdef __cplusplus
extern "C" {
#endif
/********************************************************************/

/*********************************************************************
* return codes:
*********************************************************************/

#define RBT_RC_OK         0
#define RBT_RC_NOTFOUND   1
#define RBT_RC_ERROR     -1

/*********************************************************************
* structs:
*********************************************************************/

/* definition struct: */

typedef struct
{
    size_t     left_ofs;             /* offsetof to left child */
    size_t     right_ofs;            /* offsetof to right child */
    size_t     color_ofs;            /* offsetof to color attribute (char) */
    int      (*nodeCmp)(
                void*node1,
                void*node2);         /* compare function for nodes */
    int      (*keyCmp)(
                void*node,
                void*key);           /* compare function for key */
    void     *(*allocRoot)(size_t);  /* malloc function for root data */
    void     (*freeRoot)(void*);     /* free function for root data */
    void     (*freeNode)(void*);     /* free function for node */
}
RBTDEF;

/* tree root struct: */

typedef struct
{
    void         * root;             /* root pointer */
    size_t         size;             /* total number of nodes */
    RBTDEF       * def;              /* */
}
RBT;

/*********************************************************************
* prototypes:
*********************************************************************/

/*** Creation, etc ***/

RBT * rbt_new(                     /* return pointer to root data */
    RBTDEF * def );                /* definition struct */

void rbt_init(
    RBT    * r,                    /* struct to init */
    RBTDEF * def );                /* definition struct */

void   rbt_free( RBT * rbt );      /* free complete tree */
void   rbt_clr ( RBT * rbt );      /* clear all nodes */
void   rbt_clr2( RBT * rbt );      /* clear all nodes for 2nd (3rd..) tree */
size_t rbt_size( RBT * rbt );      /* return total number of nodes */

/*** Insertion ***/

int rbt_insert     ( RBT * rbt, void * node );
int rbt_insert_keep( RBT * rbt, void * node, void ** old_node );
/* return: RBT_RC_OK(0), RBT_RC_ERROR(-1) */

/*** Deletion ***/

int rbt_delkey      ( RBT * rbt, void * key );
int rbt_delkey_keep ( RBT * rbt, void * key, void ** old_node );
int rbt_delnode     ( RBT * rbt, void * node );
int rbt_delnode_keep( RBT * rbt, void * node, void ** old_node );
/* return: RBT_RC_OK(0), RBT_RC_NOTFOUND(1), RBT_RC_ERROR(-1) */

/*** Traversal ***/

void * rbt_get   ( RBT * rbt, void * key );  /* Equal-to */
void * rbt_first ( RBT * rbt );              /* First node */
void * rbt_next  ( RBT * rbt, void * node ); /* Ascending node */
void * rbt_last  ( RBT * rbt );              /* Last node */
void * rbt_prev  ( RBT * rbt, void * node ); /* Descending node */
void * rbt_feq   ( RBT * rbt,
                    int (*cmp)(void*,void*),
                    void * key );            /* First Equal-to */
void * rbt_leq   ( RBT * rbt,
                    int (*cmp)(void*,void*),
                    void * key );            /* Last Equal-to */

/*** (validation tests) ***/

int rbttest_black( RBT * rbt );
int rbttest_red  ( RBT * rbt );
int rbttest_first( RBT * rbt );
int rbttest_last ( RBT * rbt );
int rbttest_ascending( RBT * rbt );
int rbttest_all  ( RBT * rbt );

/********************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* RBT_H_ */
/***[end-of-file]****************************************************/
/********************************************************************/
