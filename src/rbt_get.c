/*********************************************************************
* Red Black Tree functions (threaded)
*
* rbt_get.c
*
**********************************************************************
* function:
*
*   void * rbt_get   ( RBT * rbt, void * key ) // Find node
*
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/

#include "rbt.h"
#include "rbt_internal.h"

/*********************************************************************
* void * rbt_get(...)
* Return found node (by key) or NULL if not found.
*********************************************************************/

void * rbt_get(
    RBT      * rbt,
    void     * key)
{
    int    rc;
    void * node;
    RBTDEF * def;

    def = rbt->def;
    node = rbt->root;
    if( node == NULL )
        return NULL;
    for( ; ; )
    {
        rc = def->keyCmp( node, key );
        if( rc == 0 ) /* node found */
            return node;
        else if( rc > 0 ) /* data < (*node)->data ) */
        {
            if( is_left_thrd(node) )
                return NULL;
            node = child_left(node);
        }
        else
        {
            if( is_right_thrd(node) )
                return NULL;
            node = child_right(node);
        }
    }
}

/***[end-of-file]****************************************************/
/********************************************************************/
