/*********************************************************************
* Red Black Tree functions (threaded)
*
* rbt_insert.c
*
**********************************************************************
* functions:
*
*  int rbt_insert_keep( RBT * rbt, void * node, void ** old_node )
*  int rbt_insert     ( RBT * rbt, void * node )
*
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/

#include "rbt.h"
#include "rbt_internal.h"

/*********************************************************************
*
*********************************************************************/

/* insert actions: */
#define ACTION_BLACK          0
#define ACTION_RED            1
#define ACTION_RED_LEFT_RED   2
#define ACTION_RED_RIGHT_RED  3
#define ACTION_DUPLICATE      4

typedef struct {
    void  * node_arg;
    void  * old_node;
    RBT   * rbt;
} VAR;

/*********************************************************************
* static int insert_node (...)
* Return: ACTION_xxx code
*********************************************************************/

static int insert_node (
    RBTDEF * def,
    VAR    * var,
    void  ** node)
{
    int      action;
    int      rc;
    void   * c;  /* NODE */

    if ( *node == NULL )
    {
        *node = var->node_arg;
        child_left(*node) = NULL;
        child_right(*node) = NULL;
        node_color(*node) = 0;
        set_red(*node);
        return ACTION_RED;
    }
    rc = def->nodeCmp( *node, var->node_arg );
    if( rc == 0 )  /* node replacement */
    {
        void * n;

        var->old_node = *node;
        child_left(var->node_arg)  = child_left(*node);
        child_right(var->node_arg) = child_right(*node);
        node_color(var->node_arg)  = node_color(*node);

        n = *node;
        if( is_right_data(n) ) /**/
        {
            n = child_right(n);
            while( is_left_data(n) )
                n = child_left(n);
            child_left(n) = var->node_arg;
        }
        n = *node;
        if( is_left_data(n) ) /**/
        {
            n = child_left(n);
            while( is_right_data(n) )
                n = child_right(n);
            child_right(n) = var->node_arg;
        }
        *node = var->node_arg;

        return ACTION_DUPLICATE;
    }
    else if ( rc > 0 ) /* data < (*node)->data */
    {
        if( is_left_thrd(*node) ) /**/
        {
            c = child_left(*node);
            child_left(*node) = NULL;
            action = insert_node( def, var, &child_left(*node) );
            if ( child_left(*node) ) /**/
            {
                child_left(child_left(*node)) = c;
                child_right(child_left(*node)) = (*node);
                set_left_data(*node);
            }
            else
                child_left(*node) = c; /* restore - in error */
        }
        else
            action = insert_node( def, var, &child_left(*node) );
        switch(action)
        {
        case ACTION_DUPLICATE:
        case ACTION_BLACK:
            return action;
        case ACTION_RED:
            return is_red(*node) ? ACTION_RED_LEFT_RED : ACTION_BLACK ;
        }
        /* ACTION_RED_LEFT_RED */
        /* ACTION_RED_RIGHT_RED */
        /* case 3 */
        if( is_right_data(*node) && is_red(child_right(*node)) )
        {
            set_black(child_left(*node));
            set_black(child_right(*node));
            set_red((*node));
            return ACTION_RED;
        }
        /* case 4 - left rotation */
        if( action == ACTION_RED_RIGHT_RED )
        {
            c = child_right(child_left(*node));
            if( is_left_thrd(c) ) /**/
            {
                child_right(child_left(*node)) = c;
                set_right_thrd(child_left(*node));
                set_left_data(c);
            }
            else
                child_right(child_left(*node)) = child_left(c);
            child_left(c) = child_left(*node);
            child_left(*node) = c;
            /* lr-done. */
            /* case 4 is now case 5 */
        }
        /* case 5 - right rotation */
        c = child_left(*node);
        if( is_right_thrd(c) ) /**/
        {
            child_left(*node) = c;
            set_left_thrd(*node);
            set_right_data(c);
        }
        else
            child_left(*node) = child_right(c);
        child_right(c) = (*node);
        (*node) = c;
        /* rr-done. */
        set_black(*node);
        set_red(child_right(*node));
        return ACTION_BLACK;
    }
    else  /* data > (*node)->data */
    {
        if( is_right_thrd(*node) ) /**/
        {
            c = child_right(*node);
            child_right(*node) = NULL;
            action = insert_node( def, var, &child_right(*node) );
            if ( child_right(*node) ) /**/
            {
                child_right(child_right(*node)) = c;
                child_left(child_right(*node)) = *node;
                set_right_data(*node);
            }
            else
                child_right(*node) = c; /* restore - in error */
        }
        else
            action = insert_node( def, var, &child_right(*node) );
        switch(action)
        {
        case ACTION_DUPLICATE:
        case ACTION_BLACK:
            return action;
        case ACTION_RED:
            return is_red(*node) ? ACTION_RED_RIGHT_RED : ACTION_BLACK ;
        }
        /* ACTION_RED_LEFT_RED */
        /* ACTION_RED_RIGHT_RED */
        /* case 3 */
        if( is_left_data(*node) && is_red(child_left(*node)) )
        {
            set_black(child_right(*node));
            set_black(child_left(*node));
            set_red((*node));
            return ACTION_RED;
        }
        /* case 4 - right rotation */
        if(action == ACTION_RED_LEFT_RED)
        {
            c = child_left(child_right(*node));
            if( is_right_thrd(c) ) /**/
            {
                child_left(child_right(*node)) = c;
                set_left_thrd(child_right(*node));
                set_right_data(c);
            }
            else
                child_left(child_right(*node)) = child_right(c);
            child_right(c) = child_right(*node);
            child_right(*node) = c;
            /* case 4 is now case 5 */
        }
        /* case 5 - left rotation */
        c = child_right(*node);
        if( is_left_thrd(c) ) /**/
        {
            child_right(*node) = c;
            set_right_thrd(*node);
            set_left_data(c);
        }
        else
            child_right(*node) = child_left(c);
        child_left(c) = (*node);
        (*node) = c;
        /* lr-done. */
        set_black(*node);
        set_red(child_left(*node));
        return ACTION_BLACK;
    }
}

/*********************************************************************
* int rbt_insert_keep(...)
* Insert a new node, keep old replaced node (if it exist). The replaced
* old node must then be freed separatly.
* Return: RBT_RC_OK(0), RBT_RC_ERROR(-1)
*********************************************************************/

int rbt_insert_keep(
    RBT   * rbt,
    void  * node,
    void ** old_node )
{
    int  rc;
    int  action;
    RBTDEF * def;
    VAR  Var;

    Var.rbt = rbt;
    def = rbt->def;

    if( node == NULL )
        return RBT_RC_ERROR;
    if( old_node )
        *old_node = NULL;
    Var.node_arg = node;
    Var.old_node = NULL;

    action = insert_node( def, &Var, &rbt->root );

    if( action == ACTION_RED )
    {
        set_black(rbt->root);
        action = ACTION_BLACK;
    }
    if( action == ACTION_BLACK )
    {
        rbt->size++;
        rc = RBT_RC_OK; /* ok */
    }
    else if( action == ACTION_DUPLICATE )
        rc = RBT_RC_OK;   /* duplicate */
    else
        rc = RBT_RC_ERROR;  /* error */

    if( Var.old_node )
    {
        if( old_node )
        {
            child_left(Var.old_node) = NULL;
            child_right(Var.old_node) = NULL;
            node_color(Var.old_node) = 0;
            *old_node = Var.old_node;
        }
        else
            def->freeNode( Var.old_node );
        rc = RBT_RC_OK; /* ok */
    }
    return rc;
}

/*********************************************************************
* int rbt_insert(...)
* Insert a new node.
* Return: RBT_RC_OK(0), RBT_RC_ERROR(-1)
*********************************************************************/

int rbt_insert(
    RBT   * rbt,
    void  * node )
{
    return rbt_insert_keep( rbt, node, NULL );
}

/***[end-of-file]****************************************************/
/********************************************************************/
