/*********************************************************************
* Red Black Tree functions (threaded)
*
* rbt_del.c
*
**********************************************************************
* functions:
*
* delete by key:
*
*   int rbt_delkey      ( RBT * rbt, void * key )
*   int rbt_delkey_keep ( RBT * rbt, void * key, void ** old_node )
*
* delete by node:
*
*   int rbt_delnode     ( RBT * rbt, void * node )
*   int rbt_delnode_keep( RBT * rbt, void * node, void ** old_node )
*
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/

#include "rbt.h"
#include "rbt_internal.h"

/* A_D_xxx codes: Action Delete: */
#define A_D_NODE_NOT_FOUND      1
#define A_D_BLACK_NO_KID        2
#define A_D_RED_NO_KID          3
#define A_D_INVALID_TREE        4
#define A_D_DELETE_COMPLETED    5
#define A_D_MIS_ONE_BLACK_LEVEL 6

/* Delete modes: */
#define DELMODE_SEARCH   0
#define DELMODE_GO_LEFT  1
#define DELMODE_GO_RIGHT 2

/*********************************************************************
*
*********************************************************************/

typedef struct {
    int      (*deleteCmp)(void*node1,void*node2);
    void     * node_arg;
    void     * old_node;
    int        delmode;
    int        ajust_thread;
    RBT      * rbt;
} VAR;

static int find_unlink_fix( RBTDEF * def, VAR * var, void**p );

/*********************************************************************
* static int balance_black_left(...)
* Return: 1: missing black level, 0: ok
*********************************************************************/

static int balance_black_left(
    RBTDEF    * def,
    void     ** p)
{
    /* child_left(*p) is one level black short */
    void * s,* c; /* sibling */

    s = child_right(*p);

    /* REMOVAL Case 2. */
    if( is_red(s) ) /**/
    {
        set_red(*p);
        set_black(s);
        /* left rotate start (*p), s is already = child_right(*p); */
        child_right(*p) = child_left(s);
        child_left(s) = (*p);
        (*p) = s;
        /* left rotate done. */
        /* change p ! */
        p = &child_left(*p);
        s = child_right(*p);
    }

    /* REMOVAL Case 3. Note: s is BLACK ! */
    if( is_right_thrd(s) || is_black(child_right(s)) )
    {
        if( is_left_thrd(s) || is_black(child_left(s)) )
        {
            if( is_black(*p) ) /**/
            {
                set_red(s);
                return 1; /* one black level to short */
            }
            else
            {
                /* REMOVAL Case 4. */
                set_red(s);
                set_black(*p);
                return 0; /* COMPLETED */
            }
        }

        /* REMOVAL Case 5. */
        /* right rotate start s */
        c = child_left(s);    /* c is sibling left child */
        if( is_right_thrd(c) ) /**/
        {
            child_left(s) = c;
            set_left_thrd(s);
            set_right_data(c);
        }
        else
            child_left(s) = child_right(c);
        child_right(c) = s;
        set_red(s);
        set_black(c);
        child_right(*p) = s = c;    /* c is now out of scope ! */
        /* right rotate done. */
    }

    /* REMOVAL Case 6. */
    if( is_red(*p) )
        set_red(s);
    else
        set_black(s);
    set_black(*p);
    set_black(child_right(s));
    /* left rotate start (*p), s is already = child_right(*p); */
    if( is_left_thrd(s) ) /**/
    {
        child_right(*p) = s;
        set_right_thrd(*p);
        set_left_data(s);
    }
    else
        child_right(*p) = child_left(s);
    child_left(s) = (*p);
    (*p) = s;
    /* left rotate done. */
    return 0; /* COMPLETED */
}

/*********************************************************************
* static int balance_black_right(...)
* Return: 1: missing black level, 0: ok
*********************************************************************/

static int balance_black_right(
    RBTDEF    * def,
    void     ** p)
{
    /* child_right(*p) is one level black short */
    void * s,*c; /* sibling */

    s = child_left(*p);

    /* REMOVAL Case 2. */
    if( is_red(s) ) /**/
    {
        set_red(*p);
        set_black(s);
        /* right rotate start (*p), s is already = child_left(*p); */
        child_left(*p) = child_right(s);
        child_right(s) = (*p);
        (*p) = s;
        /* right rotate done. */
        /* change p ! */
        p = &child_right(*p);
        s = child_left(*p);
    }

    /* REMOVAL Case 3. Note: s is BLACK ! */
    if( is_left_thrd(s) || is_black(child_left(s)) )
    {
        if( is_right_thrd(s) || is_black(child_right(s)) )
        {
            if( is_black(*p) ) /**/
            {
                set_red(s);
                return 1; /* one black level to short */
            }
            else
            {
                /* REMOVAL Case 4. */
                set_red(s);
                set_black(*p);
                return 0; /* COMPLETED */
            }
        }

        /* REMOVAL Case 5. */
        /* left rotate start s */
        c = child_right(s);    /* c is sibling right child */
        if( is_left_thrd(c) ) /**/
        {
            child_right(s) = c;
            set_right_thrd(s);
            set_left_data(c);
        }
        else
            child_right(s) = child_left(c);
        child_left(c) = s;
        set_red(s);
        set_black(c);
        child_left(*p) = s = c;    /* c is now out of scope ! */
        /* left rotate done. */
    }

    /* REMOVAL Case 6. */
    if( is_red(*p) )
        set_red(s);
    else
        set_black(s);
    set_black(*p);
    set_black(child_left(s));
    /* right rotate start (*p), s is already = child_left(*p); */
    if( is_right_thrd(s) ) /**/
    {
        child_left(*p) = s;
        set_left_thrd(*p);
        set_right_data(s);
    }
    else
        child_left(*p) = child_right(s);
    child_right(s) = (*p);
    (*p) = s;
    /* right rotate done. */
    return 0; /* COMPLETED */
}

/*********************************************************************
* static void * prev_node(...)
* Return: node or NULL
*********************************************************************/

static void * prev_node(
    RBTDEF   * def,
    void     * node)
{
    if( node == NULL )
        return NULL;
    if( is_left_thrd(node) )
        return child_left(node);
    node = child_left(node);
    while( is_right_data(node) )
        node = child_right(node);
    return node;
}

/*********************************************************************
* static int unlink_from_tree(...)
* Return: A_D_xxx codes
*********************************************************************/

static int unlink_from_tree(
    RBTDEF    * def,
    VAR       * var,
    void     ** p)
{
    int    action;
    void * a; /* RBT_NODE */

    switch( node_color(*p) ) /**/
    {
    case 0: /* black, no kid */
        var->old_node = (*p);
        return A_D_BLACK_NO_KID;
    case 1: /* red, no kid */
        var->old_node = (*p);
        return A_D_RED_NO_KID;
    case 2: /* black, leftkid (red) */
        var->old_node = (*p);
        *p = child_left(*p);  /* replace red child with parent */
        set_black(*p);
        if( var->ajust_thread )
            child_right(*p) = child_right(var->old_node); /* thread ptr */
        return A_D_DELETE_COMPLETED;
    case 4: /* black, rightkid (red) */
        var->old_node = (*p);
        *p = child_right(*p);  /* replace red child with parent */
        set_black(*p);
        if( var->ajust_thread )
            child_left(*p) = child_left(var->old_node); /* thread ptr */
        return A_D_DELETE_COMPLETED;
    case 6: /* black, 2 kids */
    case 7: /* red, 2 kids */
        var->delmode = DELMODE_GO_LEFT; /* or: var->delmode = DELMODE_GO_RIGHT; */
        var->ajust_thread = 0;
        action = find_unlink_fix( def, var, &child_right(*p) );  /* or: action = find_unlink_fix( def, &child_left(*p)); */
        var->delmode = DELMODE_SEARCH;
        var->ajust_thread = 1;
        switch(action)
        {
        case A_D_NODE_NOT_FOUND:
            return A_D_NODE_NOT_FOUND;
        case A_D_INVALID_TREE:
            return A_D_INVALID_TREE;
        case A_D_BLACK_NO_KID:
            var->old_node = (*p);
            *p = child_right(*p);

            child_left(*p) = child_left(var->old_node);
            node_color(*p) = node_color(var->old_node) ;
            set_right_thrd(*p);
            child_right(prev_node( def,  *p )) = *p; /* thread ptr */

            return balance_black_right( def, p ) ?
                A_D_MIS_ONE_BLACK_LEVEL : A_D_DELETE_COMPLETED;
        case A_D_RED_NO_KID:
            var->old_node = (*p);
            *p = child_right(*p);

            child_left(*p) = child_left(var->old_node);
            node_color(*p) = node_color(var->old_node);
            set_right_thrd(*p);
            child_right(prev_node( def,  *p )) = *p; /* thread ptr */

            return A_D_DELETE_COMPLETED;
        case A_D_DELETE_COMPLETED:
            a = var->old_node;
            var->old_node = (*p);
            *p = a;

            child_left(*p)  = child_left(var->old_node);
            child_right(*p) = child_right(var->old_node);
            node_color(*p)  = node_color(var->old_node);
            child_right(prev_node( def,  *p )) = *p; /* thread ptr */

            return A_D_DELETE_COMPLETED;
        case A_D_MIS_ONE_BLACK_LEVEL:
            a = var->old_node;
            var->old_node = (*p);
            *p = a;

            child_left(*p)  = child_left(var->old_node);
            child_right(*p) = child_right(var->old_node);
            node_color(*p)  = node_color(var->old_node);
            child_right(prev_node( def,  *p )) = *p; /* thread ptr */

            return balance_black_right( def, p ) ?
                A_D_MIS_ONE_BLACK_LEVEL : A_D_DELETE_COMPLETED;
        }
        return A_D_INVALID_TREE;
    case 3: /* red, leftkid  - error */
        return A_D_INVALID_TREE;
    case 5: /* red, rightkid - error */
        return A_D_INVALID_TREE;
    default:
        return A_D_INVALID_TREE;
    }
}

/*********************************************************************
* static int find_unlink_fix(...)
* Return: A_D_xxx codes
*********************************************************************/

static int find_unlink_fix(
    RBTDEF    * def,
    VAR       * var,
    void     ** p)
{
    int rc;
    int action;

    rc = 0;
    switch( var->delmode )
    {
    case DELMODE_SEARCH:
        rc = var->deleteCmp( *p, var->node_arg );
        break;
    case DELMODE_GO_LEFT:
        rc = is_left_thrd(*p) ? 0 : 1;
        break;
    case DELMODE_GO_RIGHT:
        rc = is_right_thrd(*p) ? 0 : -1;
        break;
    }
    if( rc == 0 ) /* node found - delete it; */
    {
        return unlink_from_tree( def, var, p );
    }
    else if( rc > 0 ) /* data < (*node)->data ) */
    {
        if( is_left_thrd(*p) )
            return A_D_NODE_NOT_FOUND;
        switch( action = find_unlink_fix( def, var, &child_left(*p) ) )
        {
        case A_D_NODE_NOT_FOUND:
            return A_D_NODE_NOT_FOUND;
        case A_D_INVALID_TREE:
            return A_D_INVALID_TREE;
        case A_D_DELETE_COMPLETED:
            return A_D_DELETE_COMPLETED;
        case A_D_RED_NO_KID:     /* change action to: left-is-RED_NO_KID; */
            if( var->ajust_thread )
                child_left(*p) = child_left(child_left(*p)); /* thread ptr */
            set_left_thrd(*p);
            return A_D_DELETE_COMPLETED;
        case A_D_BLACK_NO_KID:     /* change action to: left-is-BLACK_NO_KID; */
            if( var->ajust_thread )
                child_left(*p) = child_left(child_left(*p)); /* thread ptr */
            set_left_thrd(*p);
            /* we are starting on case 2 now ... */
            return balance_black_left( def, p ) ?
                A_D_MIS_ONE_BLACK_LEVEL : A_D_DELETE_COMPLETED;
        case A_D_MIS_ONE_BLACK_LEVEL:
            return balance_black_left( def, p ) ?
                A_D_MIS_ONE_BLACK_LEVEL : A_D_DELETE_COMPLETED;
        }
    }
    else /* data > (*node)->data ) */
    {
        if( is_right_thrd(*p) )
            return A_D_NODE_NOT_FOUND;
        switch( action = find_unlink_fix( def, var, &child_right(*p) ) )
        {
        case A_D_NODE_NOT_FOUND:
            return A_D_NODE_NOT_FOUND;
        case A_D_INVALID_TREE:
            return A_D_INVALID_TREE;
        case A_D_DELETE_COMPLETED:
            return A_D_DELETE_COMPLETED;
        case A_D_RED_NO_KID:     /* change action to: right-is-RED_NO_KID; */
            if( var->ajust_thread )
                child_right(*p) = child_right(child_right(*p)); /* thread ptr */
            set_right_thrd(*p);
            return A_D_DELETE_COMPLETED;
        case A_D_BLACK_NO_KID:     /* change action to: right-is-BLACK_NO_KID; */
            if( var->ajust_thread )
                child_right(*p) = child_right(child_right(*p)); /* thread ptr */
            set_right_thrd(*p);
            /* we are starting on case 2 now ... */
            return balance_black_right( def, p ) ?
                A_D_MIS_ONE_BLACK_LEVEL : A_D_DELETE_COMPLETED;
        case A_D_MIS_ONE_BLACK_LEVEL:
            return balance_black_right( def, p ) ?
                A_D_MIS_ONE_BLACK_LEVEL : A_D_DELETE_COMPLETED;
        }
    }
    return 0; /* will never reach here */
}

/*********************************************************************
* static int delete_node(...)
* Return: RBT_RC_OK(0), RBT_RC_NOTFOUND(1), RBT_RC_ERROR(-1)
*********************************************************************/

static int delete_node(
    RBTDEF    * def,
    VAR       * var,
    void      * node,
    void     ** old_node)
{
    int action;

    if( old_node )
        *old_node = NULL;
    if( node == NULL )
        return RBT_RC_ERROR;
    if( var->rbt->root == NULL )
        return RBT_RC_NOTFOUND; /* notfound */

    var->old_node = NULL;
    var->node_arg = node;
    var->delmode = DELMODE_SEARCH;
    var->ajust_thread = 1;

    action = find_unlink_fix( def, var, &var->rbt->root );

    switch (action)
    {
    case A_D_NODE_NOT_FOUND:
        return RBT_RC_NOTFOUND; /* notfound */
    case A_D_INVALID_TREE:
        return RBT_RC_ERROR; /* error */
    case A_D_BLACK_NO_KID:
    case A_D_RED_NO_KID:
        var->old_node = var->rbt->root;
        var->rbt->root = NULL;
        action = A_D_DELETE_COMPLETED;
        break;
    case A_D_MIS_ONE_BLACK_LEVEL:
        action = A_D_DELETE_COMPLETED;
        break;
    }
    /* action is A_D_DELETE_COMPLETED */
    var->rbt->size--;

    if( old_node )
    {
        if( var->old_node )
        {
            child_left(var->old_node) = NULL;
            child_right(var->old_node) = NULL;
            node_color(var->old_node) = 0;
        }

        *old_node = var->old_node;
    }
    else if( var->old_node )
        def->freeNode( var->old_node );

    return RBT_RC_OK; /* ok */
}

/*********************************************************************
* int rbt_delkey      ( RBT * rbt, void * key )
* Delete by key.
* Return: RBT_RC_OK(0), RBT_RC_NOTFOUND(1), RBT_RC_ERROR(-1)
*********************************************************************/

int rbt_delkey(
    RBT       * rbt,
    void      * key )
{
    VAR Var;
    Var.deleteCmp = rbt->def->keyCmp; /* delete by key */
    Var.rbt = rbt;
    return delete_node( rbt->def, &Var, key, NULL );
}

/*********************************************************************
* int rbt_delkey_keep ( RBT * rbt, void * key, void ** old_node )
* Delete by key, keep old deleted node (if found).
* Return: RBT_RC_OK(0), RBT_RC_NOTFOUND(1), RBT_RC_ERROR(-1)
*********************************************************************/

int rbt_delkey_keep(
    RBT       * rbt,
    void      * key,
    void     ** old_node )
{
    VAR Var;
    Var.deleteCmp = rbt->def->keyCmp; /* delete by key */
    Var.rbt = rbt;
    return delete_node( rbt->def, &Var, key, old_node );
}

/*********************************************************************
* int rbt_delnode     ( RBT * rbt, void * node )
* Delete by node.
* Return: RBT_RC_OK(0), RBT_RC_NOTFOUND(1), RBT_RC_ERROR(-1)
*********************************************************************/

int rbt_delnode(
    RBT       * rbt,
    void      * node )
{
    VAR Var;
    Var.deleteCmp = rbt->def->nodeCmp; /* delete by node */
    Var.rbt = rbt;
    return delete_node( rbt->def, &Var, node, NULL );
}

/*********************************************************************
* int rbt_delnode_keep( RBT * rbt, void * node, void ** old_node )
* Delete by node, keep old deleted node (if found).
* Return: RBT_RC_OK(0), RBT_RC_NOTFOUND(1), RBT_RC_ERROR(-1)
*********************************************************************/

int rbt_delnode_keep(
    RBT       * rbt,
    void      * node,
    void     ** old_node )
{
    VAR Var;
    Var.deleteCmp = rbt->def->nodeCmp; /* delete by node */
    Var.rbt = rbt;
    return delete_node( rbt->def, &Var, node, old_node );
}

/***[end-of-file]****************************************************/
/********************************************************************/
