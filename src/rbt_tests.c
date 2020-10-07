/*********************************************************************
* Red Black Tree functions (threaded)
*
* rbt_tests.c
*
**********************************************************************
* Validation tests.
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/

#include "rbt.h"
#include "rbt_internal.h"

/*********************************************************************
*
*********************************************************************/

static int count_black_depth( RBT * rbt, void * node )
{
    int r1, r2;
    RBTDEF * def;

    def = rbt->def;

    if( node == NULL )
        return 0;

    r1 = is_left_data(node) ? count_black_depth( rbt, child_left(node) ) : 0;

    r2 = is_right_data(node) ? count_black_depth( rbt, child_right(node) ) : 0;

    if( r1 == -1 || r2 == -1 || r1 != r2 )
        return -1;
    return is_black(node) ? r1+1 : r1;
}

int rbttest_black( RBT * rbt )
{
    int rc;
    RBTDEF * def;

    def = rbt->def;
    if( rbt->root != NULL && is_red(rbt->root) ) return -1;
    rc=count_black_depth( rbt, rbt->root );
    return rc < 0 ? -1 : 0;
}

/****/

static int count_doublered( RBT * rbt, void * node )
{
    int sz;
    RBTDEF * def;

    def = rbt->def;
    sz = 0;
    if( node == NULL )
        return 0;
    if( is_red(node) && is_left_data(node) && is_red(child_left(node)) )
        sz++;
    if( is_red(node) && is_right_data(node) && is_red(child_right(node)) )
        sz++;
    if( is_left_data(node) )
        sz += count_doublered( rbt, child_left(node) );
    if( is_right_data(node) )
        sz += count_doublered( rbt, child_right(node) );
    return sz;
}

int rbttest_red( RBT * rbt )
{
    int rc;
    rc = count_doublered( rbt, rbt->root );
    return rc != 0 ? -1 : 0;
}

/****/

int rbttest_first( RBT * rbt )
{
    void * r;
    size_t sz;
    for( sz = 0, r = rbt_first(rbt) ; r ; r = rbt_next(rbt,r) )
        sz++;
    //printf(" first:%s", rbt->size == sz ? "ok" : "ERROR" );
    return sz-rbt->size;
}

int rbttest_last( RBT * rbt )
{
    void * r;
    size_t sz;
    for( sz=0, r = rbt_last(rbt) ; r ; r = rbt_prev(rbt,r) )
        sz++;
    //printf(" last:%s", rbt->size == sz ? "ok" : "ERROR" );
    return sz != rbt->size ? -1 : 0 ;
}

int rbttest_ascending( RBT * rbt )
{
    size_t sz;
    int rc;
    void * r1;
    void * r2;

    sz = 0;
    r1 = rbt_first(rbt);
    if( r1 )
        sz++;
    r2 = r1 ? rbt_next( rbt, r1 ) : NULL ;
    for( ; r2 !=NULL ; r1 = r2, r2 = rbt_next(rbt,r2) )
    {
        rc = rbt->def->nodeCmp( r1, r2 );
        if( rc < 0 )
            sz ++;
    }
    //printf(" acc:%s", size==rbt->size ? "ok" : "ERROR" );

    //printf( "    size=%ld; ", rbt->size);
    //printf("\n");
    return sz != rbt->size ? -1 : 0 ;
}


int rbttest_all( RBT * rbt )
{
    if( rbttest_black(rbt) != 0 )
        return -1;
    if( rbttest_red(rbt) != 0 )
        return -2;
    if( rbttest_first( rbt ) != 0 )
        return -3;
    if( rbttest_last( rbt )!= 0 )
        return -4;
    if( rbttest_ascending( rbt ) != 0 )
        return -5;
    return 0;
}

/***[end-of-file]****************************************************/
/********************************************************************/
