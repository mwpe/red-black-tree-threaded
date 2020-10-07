/*********************************************************************
* Red Black Tree functions (threaded)
*
* rbt_feq.c
*
**********************************************************************
* range find functions:
*
*   void * rbt_feq( RBT * rbt, int (*cmp)(void*,void*), void * key)
*   void * rbt_leq( RBT * rbt, int (*cmp)(void*,void*), void * key)
*
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/

#include "rbt.h"
#include "rbt_internal.h"

/*********************************************************************
* void * rbt_feq(...)
* First Equal-to
* Return: node or NULL.
*********************************************************************/

void * rbt_feq(
    RBT  * rbt,
    int  (*cmp)(void*,void*),
    void * key)
{
    void * p;
    void * p2;
    int    rc;
    int    rc2;
    RBTDEF * def;

    def = rbt->def;
    p = rbt->root;
    if( p == NULL )
        return NULL;
    for( ; ; )
    {
        /* test if p is a candidate */
        rc = cmp( p, key );
        if( rc >= 0 )
            break;
        /* p is to low */
        if( is_right_thrd(p) )
            return NULL;
        p = child_right(p);
    }
    for( ; ; )
    {
        /* p is now a candidate, see if there is one lower */
        if( is_left_thrd(p) )
            return rc == 0 ? p : NULL ; /* no lower candidate */
        p2 = child_left(p);
        for( ; ; )
        {
            rc2 = cmp( p2, key );
            if( rc2 >= 0 )
                break;
            /* p2 is to low - seek one higher */
            if( is_right_thrd(p2) )
                return  rc == 0 ? p : NULL ;
            p2 = child_right(p2);
        }
        /* p2 is the new candidate */
        p = p2;
        rc = rc2;
    }
}

/*********************************************************************
* void * rbt_leq(...)
* Find Last Equal-to
* Return: node or NULL.
*********************************************************************/

void * rbt_leq(
    RBT  * rbt,
    int  (*cmp)(void*,void*),
    void * key)
{
    void * p;
    void * p2;
    int    rc;
    int    rc2;
    RBTDEF * def;

    def = rbt->def;
    p = rbt->root;
    if( p == NULL )
        return NULL;
    for( ; ; )
    {
        /* test if p is a candidate */
        rc = cmp( p, key );
        if( rc <= 0 )
            break;
        /* p is to high */
        if( is_left_thrd(p) )
            return NULL;
        p = child_left(p);
    }
    for( ; ; )
    {
        /* p is now a candidate, see if there is one higher */
        if( is_right_thrd(p) )
            return rc == 0 ? p : NULL; /* no higher candidate */
        p2 = child_right(p);
        for( ; ; )
        {
            rc2 = cmp( p2, key );
            if( rc2 <= 0 )
                break;
            /* p2 is to high - seek one lower */
            if( is_left_thrd(p2) )
                return  rc == 0 ? p : NULL;
            p2 = child_left(p2);
        }
        /* p2 is the new candidate */
        p = p2;
        rc = rc2;
    }
}

/***[end-of-file]****************************************************/
/********************************************************************/
