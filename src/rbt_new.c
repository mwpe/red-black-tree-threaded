/*********************************************************************
* Red Black Tree functions (threaded)
*
* rbt_new.c
*
**********************************************************************
* functions:
*
*   RBT  * rbt_new ( RBTDEF * def )
*   void   rbt_init( RBT * r, RBTDEF * def )
*   void   rbt_free( RBT * rbt )
*   void   rbt_clr ( RBT * rbt )
*   void   rbt_clr2( RBT * rbt )
*   size_t rbt_size( RBT * rbt )
*
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/

#include "rbt.h"
#include "rbt_internal.h"

/*********************************************************************
* RBT * rbt_new(...)
* Create a new tree.
* Return a new tree or NULL on memory problems.
*********************************************************************/

RBT * rbt_new(
    RBTDEF * def)
{
    RBT    * r;

    if( def->allocRoot == NULL)
        return NULL;
    r = (RBT*)def->allocRoot( sizeof(RBT) );
    if( r == NULL )
        return NULL;
    r->root = NULL;
    r->size = 0;
    r->def  = def;
    return r;
}

/*********************************************************************
* void rbt_init(...)
* Initialize an existing tree structur
*********************************************************************/

void rbt_init(
    RBT    * r,
    RBTDEF * def)
{
    r->root = NULL;
    r->size = 0;
    r->def  = def;
}

/*********************************************************************
* static void free_node(...)
* Free all nodes.
*********************************************************************/

static void free_node(
    RBT  * rbt,
    void * node)
{
    RBTDEF * def;

    def = rbt->def;
    if( node )
    {
        if( is_left_data(node) )
            free_node( rbt, child_left(node) );
        if( is_right_data(node) )
            free_node( rbt, child_right(node) );
        def->freeNode( node );
    }
}

/*********************************************************************
* void rbt_free(...)
* Free a complete tree including all its nodes.
*********************************************************************/

void rbt_free(
    RBT * rbt)
{
    free_node( rbt, rbt->root );
    if( rbt->def->freeRoot )
        rbt->def->freeRoot( rbt );
    else
    {
        rbt->root = NULL;
        rbt->size = 0;
    }
}

/*********************************************************************
* void rbt_clr(...)
* Clear all nodes
*********************************************************************/

void rbt_clr(
    RBT * rbt)
{
    free_node( rbt, rbt->root );
    rbt->root = NULL;
    rbt->size = 0;
}

/*********************************************************************
* void rbt_clr2(...)
* Clear all nodes for 2nd (3rd..) tree
*********************************************************************/

void rbt_clr2(
    RBT * rbt)
{
    rbt->root = NULL;
    rbt->size = 0;
}

/*********************************************************************
* size_t rbt_size(...)
* Return count of nodes
*********************************************************************/

size_t rbt_size(
    RBT * rbt)
{
    return rbt->size;
}

/***[end-of-file]****************************************************/
/********************************************************************/
