/*********************************************************************
* Red Black Tree functions (threaded)
*
* rbt_first.c
*
**********************************************************************
* functions:
*
*   void * rbt_first( RBT * rbt)
*   void * rbt_next( RBT * rbt, void * node )
*   void * rbt_last( RBT * rbt)
*   void * rbt_prev( RBT * rbt, void * node )
*
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/

#include "rbt.h"
#include "rbt_internal.h"

/*********************************************************************
* void * rbt_first(...)
*
* Return: first node or NULL (if tree is empty).
*********************************************************************/

void * rbt_first(
    RBT  * rbt)
{
    void * node;
    RBTDEF * def;

    def = rbt->def;
    if( rbt->root == NULL )
        return NULL;
    node = rbt->root;
    while( is_left_data(node) )
        node = child_left(node);
    return node;
}

/*********************************************************************
* void * rbt_next(...)
* Return: next node (relative to node) or NULL (=no more).
*********************************************************************/

void * rbt_next(
    RBT  * rbt,
    void * node)
{
    RBTDEF * def;

    def = rbt->def;
    if( node == NULL )
        return NULL;
    if( is_right_thrd(node) )
        return child_right(node);
    node = child_right(node);
    while( is_left_data(node) )
        node = child_left(node);
    return node;
}

/*********************************************************************
* void * rbt_last(...)
* Return: last node or NULL (if tree is empty).
*********************************************************************/

void * rbt_last(
    RBT  * rbt)
{
    void * node;
    RBTDEF * def;

    def = rbt->def;
    if( rbt->root == NULL )
        return NULL;
    node = rbt->root;
    while( is_right_data(node) )
        node = child_right(node);
    return node;
}

/*********************************************************************
* void * rbt_prev(...)
* Return: previous node (relative to node) or NULL (=no more).
*********************************************************************/

void * rbt_prev(
    RBT  * rbt,
    void * node)
{
    RBTDEF * def;

    def = rbt->def;
    if( node == NULL )
        return NULL;
    if( is_left_thrd(node) )
        return child_left(node);
    node = child_left(node);
    while( is_right_data(node) )
        node = child_right(node);
    return node;
}

/***[end-of-file]****************************************************/
/********************************************************************/
