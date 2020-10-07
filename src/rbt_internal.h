/*********************************************************************
* Red Black Tree functions (threaded)
*
* rbt_internal.h
*
**********************************************************************
* Internal helper macros for rbt_ functions.
* Important note!: a 'RBTDEF * def' must be defined for using these
* macros. The intent is to make the code cleaner, but clean coders
* might not agree with this.
**********************************************************************
* Copyright (c) 2020 Michael Walsh Pedersen
*********************************************************************/
#ifndef RBT_INTERNAL_H_
#define RBT_INTERNAL_H_

/*********************************************************************
*
* bitmap for color attribute:
* 00000cba
*  a: 1=red, 0=black,
*  b: 1=left data, 0=left thrd
*  c: 1=right data, 0=right thrd
*
*********************************************************************/

#define child_left(n)       (*(void**)((char*)(n)+def->left_ofs))
#define child_right(n)      (*(void**)((char*)(n)+def->right_ofs))
#define node_color(n)       (*((char*)(n)+def->color_ofs))

#define set_black(n)        (node_color(n)&=~1)
#define set_red(n)          (node_color(n)|=1)
#define is_red(n)           ((node_color(n)&1)==1)
#define is_black(n)         ((node_color(n)&1)!=1)

#define set_left_thrd(n)    (node_color(n)&=~2)
#define set_left_data(n)    (node_color(n)|=2)
#define is_left_thrd(n)     ((node_color(n)&2)!=2)
#define is_left_data(n)     ((node_color(n)&2)==2)

#define set_right_thrd(n)   (node_color(n)&=~4)
#define set_right_data(n)   (node_color(n)|=4)
#define is_right_thrd(n)    ((node_color(n)&4)!=4)
#define is_right_data(n)    ((node_color(n)&4)==4)

#endif//RBT_INTERNAL_H_

/***[end-of-file]****************************************************/
/********************************************************************/
