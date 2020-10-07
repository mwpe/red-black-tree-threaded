/*********************************************************************
* sampRBTc01.c
*
* To compile and run:
*
gcc -Wall -Wextra -pedantic-errors -I../src -o sampRBTc01 \
sampRBTc01.c ../src/librbt.a && ./sampRBTc01
*
* Sample C program for rbt_ functions.
*
* All identifiers starting with 'my', 'My' or 'MY' are
* used to identify the sample-specific code.
*
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rbt.h"

/*********************************************************************
* define myNode node and supporting functions:
*********************************************************************/

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

/*********************************************************************
* supporting functions and definitions for myNode struct
*********************************************************************/

myNode * myNode_newNode( const char * key, const char * data )
{
	myNode  * r ;
	r = malloc(sizeof(myNode));
	if(r==NULL)
		return NULL;
	snprintf( r->key,  sizeof(r->key),  "%s", key  );
	snprintf( r->data, sizeof(r->data), "%s", data );
	return r;
}

void myNode_freeNode( myNode  * r )
{
	free( r );
}

/**/

static int myNode_compareNode( myNode * r1, myNode * r2 )
{
	return strcmp( r1->key, r2->key );
}

static int myNode_compareKey( myNode * r1, char * key )
{
	return strcmp( r1->key, key );
}

RBTDEF * myNode_DEF = (RBTDEF[])
{
  {
	.left_ofs  = offsetof( myNode, left  ),          /* offsetof to left child */
	.right_ofs = offsetof( myNode, right ),          /* offsetof to right child */
	.color_ofs = offsetof( myNode, color ),          /* offsetof to color attribute (char) */
	.nodeCmp   = (int (*)(void *, void *)) myNode_compareNode, /* compare function for nodes */
	.keyCmp    = (int (*)(void *, void *)) myNode_compareKey,  /* compare function for key */
	.allocRoot = (void *(*)(size_t))       malloc,         /* malloc function for root data */
	.freeRoot  = (void (*)(void *))        free,           /* free function for root data */
	.freeNode  = (void (*)(void *))        myNode_freeNode /* free function for node */
  }
};

/*********************************************************************
*
*********************************************************************/

int compareRange( myNode * node, char * range )
{
	if( node->key[0] < range[0] )
		return -1;                // to low
	if( node->key[0] > range[1] )
		return 1;                 // to high
	return 0;                     // match
}

void testRun()
{
	myNode * r;
	RBT    * t;

	t = rbt_new(myNode_DEF);
	if( t == NULL )
		return;

	// testdata:
	rbt_insert( t,  myNode_newNode( "B01", "data for B01" ) );
	rbt_insert( t,  myNode_newNode( "B02", "data for B02" ) );
	rbt_insert( t,  myNode_newNode( "B03", "blah blah" ) );
	rbt_insert( t,  myNode_newNode( "A01", "data for A01" ) );
	rbt_insert( t,  myNode_newNode( "D02", "data for D02" ) );
	rbt_insert( t,  myNode_newNode( "D03", "data for D03" ) );
	rbt_insert( t,  myNode_newNode( "BXX", "(delete me)" ) );
	rbt_insert( t,  myNode_newNode( "C03", "data for C03" ) );
	rbt_insert( t,  myNode_newNode( "D01", "data for D01" ) );
	rbt_insert( t,  myNode_newNode( "A02", "data for A02" ) );
	rbt_insert( t,  myNode_newNode( "A03", "data for A03" ) );
	rbt_insert( t,  myNode_newNode( "C01", "data for C01" ) );

	// get one node
	r = rbt_get( t, "B02" );
	if( r != NULL )
		printf( "found %s with data: %s\n", r->key, r->data );
	else
		printf( "not found\n");

	// get one node
	r = rbt_get( t,"D01" );
	if( r != NULL )
		printf( "found %s with data: %s\n", r->key, r->data );
	else
		printf( "not found\n" );

	// list all
	for( r = rbt_first(t) ; r != NULL ; r = rbt_next( t, r ) )
	{
		printf( "> %s : %s ;\n", r->key, r->data);
	}
	printf( "\n" );

	printf( "--- fixing B03, deleted BXX, adding C02, reverse output ---\n" );
	rbt_insert( t, myNode_newNode( "B03", "data for B03" ) );
	rbt_delkey( t, "BXX" );
	rbt_insert( t, myNode_newNode( "C02", "data for C02" ) );
	// list reverse
	for( r = rbt_last(t) ; r != NULL ; r = rbt_prev( t, r ) )
	{
		printf( "> %s : %s ;\n", r->key, r->data);
	}
	printf( "\n" );

	printf( "--- range output B... to C... ---\n" );
	myNode * r_sta, * r_end;
	r_sta = rbt_feq( t, (int(*)(void*,void*))compareRange, "BC" );
	r_end = rbt_leq( t, (int(*)(void*,void*))compareRange, "BC" );
	for( r = r_sta ; r != NULL ; r = rbt_next( t, r ) )
	{
		printf( "> %s : %s ;\n", r->key, r->data );
		if( r == r_end )
			break;
	}

	// delete first node
	if( rbt_delnode_keep(t, rbt_first(t), (void**)&r ) == 0 )
	{
		if(r)
		{
			printf( "deleting: %s:%s;\n", r->key, r->data );
			myNode_freeNode( r );
		}
	}

	rbt_free(t); // free all
}

int main()
{
	printf( "start testRun\n" );
	testRun();
	printf( "done testRun\n" );

	return 0;
}

/********************************************************************/
