/*********************************************************************
* sampRBTcpp01.cpp
*
* To compile and run:
*
g++ -Wall -Wextra -pedantic-errors -I../src -o sampRBTcpp01 \
sampRBTcpp01.cpp ../src/librbt.a && ./sampRBTcpp01
*
* Sample C++ program for rbt_ functions.
*
* Shows the use of multiples red-black trees.
*
* All identifiers starting with 'my', 'My' or 'MY' are
* used to identify the sample-specific code.
*
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/

#include <iostream>
#include <cstring>
#include "rbt.h"

/*********************************************************************
* define 'struct myNode' node and 'class myTree' :
*********************************************************************/

struct myNode {
    void *left[2];    // used by rbt_ functions - 2 trees
    void *right[2];   // used by rbt_ functions - 2 trees
    char  color[2];   // used by rbt_ functions - 2 trees
    char  key[40];    // primary unique key
    char  key2[40];   // secondary non-unique key
    char  data[40];   // data
    myNode( const char * key, const char * key2, const char * data );
};

void myNode_freeNode(
    struct myNode  * r );

class myTree {
private:
    RBT tree[2]; // - 2 trees
public:
    myTree();
    ~myTree();
    void clr();
    size_t size();
    int insert      ( struct myNode * node );
    int insert_keep ( struct myNode * node, struct myNode ** oldnode );
    int delkey      ( const char * key );
    int delkey_keep ( const char * key, struct myNode ** oldnode );
    int delnode     ( struct myNode * node );
    int delnode_keep( struct myNode * node, struct myNode ** oldnode );
    struct myNode * get  ( const char * key );
    struct myNode * first();
    struct myNode * next ( struct myNode * node );
    struct myNode * last ();
    struct myNode * prev ( struct myNode * node );

// methods for 2nd tree:
    struct myNode * first2nd();
    struct myNode * next2nd ( struct myNode * node );
    struct myNode * last2nd ();
    struct myNode * prev2nd ( struct myNode * node );

    struct myNode * feq2nd  ( const char * key2 );
    struct myNode * leq2nd  ( const char * key2 );
};

/*********************************************************************
* supporting functions and definitions for 'struct myNode'
*********************************************************************/

myNode::myNode( const char * key, const char * key2, const char * data )
{
    snprintf( this->key,  sizeof(this->key),  "%s", key  );
    snprintf( this->key2, sizeof(this->key2), "%s", key2 );
    snprintf( this->data, sizeof(this->data), "%s", data );
}

void myNode_freeNode( struct myNode  * r )
{
    delete r;
}

/**/

static int myNode_compareNode( struct myNode * r1, struct myNode * r2 )
{
    return strcmp( r1->key, r2->key );
}

static int myNode_compareNode2nd( struct myNode * r1, struct myNode * r2 )
{
    int rc;
    rc = strcmp( r1->key2, r2->key2 );
    if( rc )
        return rc;
    return myNode_compareNode( r1, r2 );
}

static int myNode_compareKey( struct myNode * r1, char * key )
{
    return strcmp( r1->key, key );
}

static int myNode_compareKey2( struct myNode * r1, char * key2 )
{
    return strcmp( r1->key2, key2 );
}

//// alternate implementation of offsetof macro
//#ifndef offsetof
//#define offsetof(type, member) ((size_t)&(((type*)0)->member))
//#endif

/*
* Notes:
*
* About offsetof in C++, see:
*
https://en.cppreference.com/w/cpp/types/offsetof
*
*
* Designated initializers in C++ (since C++20), see:
*
https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers
*
* (only supported in C++20, so thats why they are comment out below)
*
*/

static RBTDEF myNode_DEF[] = // (RBTDEF[])
{
  {
    /* .left_ofs  = */ offsetof( struct myNode, left [0] ),          /* offsetof to left child */
    /* .right_ofs = */ offsetof( struct myNode, right[0] ),          /* offsetof to right child */
    /* .color_ofs = */ offsetof( struct myNode, color[0] ),          /* offsetof to color attribute (char) */
    /* .nodeCmp   = */ (int (*)(void *, void *)) myNode_compareNode, /* compare function for nodes */
    /* .keyCmp    = */ (int (*)(void *, void *)) myNode_compareKey,  /* compare function for key */
    /* .allocRoot = */ (void *(*)(size_t))       NULL,         /* malloc function for root data */
    /* .freeRoot  = */ (void (*)(void *))        NULL,           /* free function for root data */
    /* .freeNode  = */ (void (*)(void *))        myNode_freeNode /* free function for node */
  },
  {
    /* .left_ofs  = */ offsetof( struct myNode, left [1] ),          /* offsetof to left child */
    /* .right_ofs = */ offsetof( struct myNode, right[1] ),          /* offsetof to right child */
    /* .color_ofs = */ offsetof( struct myNode, color[1] ),          /* offsetof to color attribute (char) */
    /* .nodeCmp   = */ (int (*)(void *, void *)) myNode_compareNode2nd, /* compare function for nodes */
    /* .keyCmp    = */ (int (*)(void *, void *)) NULL,  /* compare function for key */
    /* .allocRoot = */ (void *(*)(size_t))       NULL,         /* malloc function for root data */
    /* .freeRoot  = */ (void (*)(void *))        NULL,           /* free function for root data */
    /* .freeNode  = */ (void (*)(void *))        myNode_freeNode /* free function for node */
  }
};

/*********************************************************************
*   methods for 'class myTree'.
*********************************************************************/

myTree::myTree()
{
    rbt_init( &tree[0], &myNode_DEF[0] );
    rbt_init( &tree[1], &myNode_DEF[1] );
}

myTree::~myTree()
{
    rbt_free( &tree[0] );
}

void myTree::clr()
{
    rbt_clr( &tree[0] );
    rbt_clr2( &tree[1] );
}

size_t myTree::size()
{
    return rbt_size( &tree[0] );
}

int myTree::insert( struct myNode * node )
{
    void * old;
    rbt_insert_keep( &tree[0], (void*)node, &old );
    if( old )
        rbt_delnode( &tree[1], old );
    return rbt_insert( &tree[1], (void*)node );
}

int myTree::insert_keep( struct myNode * node, struct myNode ** oldnode )
{
    rbt_insert_keep( &tree[0], (void*)node, (void**)oldnode );
    if( *oldnode )
        rbt_delnode_keep( &tree[1], (void*)oldnode, (void**)oldnode );
    return rbt_insert( &tree[1], (void*)node );
}

int myTree::delkey( const char * key )
{
    void * old;
    int rc;
    rc = rbt_delkey_keep( &tree[0], (void*)key, &old );
    if( old )
        return rbt_delnode( &tree[1], old );
    return rc;
}

int myTree::delkey_keep( const char * key, struct myNode ** oldnode )
{
    int rc;
    rc = rbt_delkey_keep( &tree[0], (void*)key, (void**)oldnode );
    if(*oldnode)
        return rbt_delnode_keep( &tree[1], (void*)*oldnode, (void**)oldnode );
    return rc;
}

int myTree::delnode( struct myNode * node )
{
    void * old;
    int rc;
    rc = rbt_delnode_keep( &tree[0], (void*)node, &old );
    if( old )
        return rbt_delnode( &tree[1], old );
    return rc;
}

int myTree::delnode_keep( struct myNode * node , struct myNode ** oldnode )
{
    int rc;
    rc = rbt_delnode_keep( &tree[0], (void*)node, (void**)oldnode );
    if( *oldnode )
        return rbt_delnode_keep( &tree[1], (void*)*oldnode, (void**)oldnode );
    return rc;
}

struct myNode * myTree::get( const char * key )
{
    return (struct myNode*)rbt_get( &tree[0], (void*)key );
}

struct myNode * myTree::first()
{
    return (struct myNode*)rbt_first( &tree[0] );
}

struct myNode * myTree::next( struct myNode * node )
{
    return (struct myNode*)rbt_next( &tree[0], (void*)node );
}

struct myNode * myTree::last()
{
    return (struct myNode*)rbt_last( &tree[0] );
}

struct myNode * myTree::prev( struct myNode * node )
{
    return (struct myNode*)rbt_prev( &tree[0], (void*)node );
}

struct myNode * myTree::first2nd()
{
    return (struct myNode*)rbt_first( &tree[1] );
}

struct myNode * myTree::next2nd( struct myNode * node )
{
    return (struct myNode*)rbt_next( &tree[1], (void*)node );
}

struct myNode * myTree::last2nd()
{
    return (struct myNode*)rbt_last( &tree[1] );
}

struct myNode * myTree::prev2nd( struct myNode * node )
{
    return (struct myNode*)rbt_prev( &tree[1], (void*)node );
}

struct myNode * myTree::feq2nd( const char * key2 )
{
    return (struct myNode*)rbt_feq(
        &tree[1],
        (int (*)(void*,void*))myNode_compareKey2,
        (void*)key2 );
}

struct myNode * myTree::leq2nd( const char * key2 )
{
    return (struct myNode*)rbt_leq(
        &tree[1],
        (int (*)(void*,void*))myNode_compareKey2,
        (void*)key2 );
}

/*********************************************************************
*
*********************************************************************/

void testRun()
{
    struct myNode * r;
    myTree * t = new myTree();

    // testdata:

    t->insert( new struct myNode( "B01", "DD", "data for B01 and DD" ) );
    t->insert( new struct myNode( "B02", "AA", "data for B02 and AA" ) );
    t->insert( new struct myNode( "B03", "--", "data for B03 and BB" ) );
    t->insert( new struct myNode( "A01", "AA", "data for A01 and AA" ) );
    t->insert( new struct myNode( "D02", "CC", "data for D02 and CC" ) );
    t->insert( new struct myNode( "D03", "DD", "data for D03 and DD" ) );
    t->insert( new struct myNode( "BXX", "BC", "(delete me)" ) );
    t->insert( new struct myNode( "C03", "AA", "data for C03 and AA" ) );
    t->insert( new struct myNode( "D01", "BB", "data for D01 and BB" ) );
    t->insert( new struct myNode( "A02", "BB", "data for A02 and BB" ) );
    t->insert( new struct myNode( "A03", "CC", "data for A03 and CC" ) );
    t->insert( new struct myNode( "C01", "CC", "data for C01 and CC" ) );

    // get one node
    r = t->get( "B02" );
    if( r != NULL )
        std::cout << "found "<< r->key << ":" << r->key2
            <<" with data: " << r->data << std::endl;
    else
        std::cout << "not found" << std::endl;

    // get one node
    r = t->get( "D01" );
    if( r != NULL )
        std::cout << "found "<< r->key << ":" << r->key2
            <<" with data: " << r->data << std::endl;
    else
        std::cout << "not found" << std::endl;

    // list all by 1st tree
    for( r = t->first() ; r != NULL ; r = t->next( r ) )
    {
        std::cout << "1> ( " << r->key << ":" << r->key2 <<  " , "
            << r->data << " );"  << std::endl;
    }
    std::cout << std::endl;

    // list all by 2nd tree
    for( r = t->first2nd() ; r != NULL ; r = t->next2nd( r ) )
    {
        std::cout << "2> ( " << r->key2 << ":" << r->key <<  " , "
            << r->data << " );"  << std::endl;
    }

    std::cout << std::endl
        << "--- fixing B03, deleted BXX, adding C02 ---" << std::endl;

    t->insert( new struct myNode( "B03", "BB", "data for B03 and BB" ) );
    t->delkey( "BXX" );
    t->insert( new struct myNode( "C02", "DD", "data for C02 and DD" ) );

    // list all by 1st tree
    for( r = t->first() ; r != NULL ; r = t->next( r ) )
    {
        std::cout << "1> ( " << r->key << ":" << r->key2 <<  " , "
            << r->data << " );"  << std::endl;
    }
    std::cout << std::endl;

    // list all by 2nd tree
    for( r = t->first2nd() ; r != NULL ; r = t->next2nd( r ) )
    {
        std::cout << "2> ( " << r->key2 << ":" << r->key <<  " , "
            << r->data << " );"  << std::endl;
    }

    // list range on 2nd tree
    std::cout << std::endl << "--- range 'BB' to 'CC' on 2nd tree ---"
        << std::endl;
    struct myNode * r_begin,*r_end;
    r_begin = t->feq2nd("BB");
    r_end = t->leq2nd("CC");
    for( r = r_begin ; r != NULL ; r = t->next2nd( r ) )
    {
        std::cout << "2> ( " << r->key2 << ":" << r->key <<  " , "
            << r->data << " );"  << std::endl;
        if( r == r_end )
            break;
    }

    // delete first node in 2nd tree
    if( t->delnode_keep( t->first2nd(), &r ) == 0 )
    {
        if(r)
        {
            std::cout << std::endl << "deleting: " << r->data << ";"
                << std::endl;
            delete r;
        }
    }

    delete t;
}

int main()
{
    std::cout << "start testRun\n";
    testRun();
    std::cout << "done testRun\n";

    return 0;
}

/********************************************************************/
