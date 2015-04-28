#ifndef __CAN_H_
#define __CAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* for directions array */
#define NB_DIRECTIONS 4
#define NORTH     0
#define SOUTH     1
#define WEST      2
#define EAST      3
#define NORTHWEST 4
#define NORTHEAST 5
#define SOUTHWEST 6
#define SOUTHEAST 7

#define N 10

/* MPI's Process vars */
#define INIT_NODE      0
#define BOOTSTRAP_NODE 1

/*******************************************************************************
 * MPI tags definition
 ******************************************************************************/
#define CAN_INSERT 10


/* structs */
typedef struct _point     point;
typedef struct _space     space;
typedef struct _node      node;
typedef struct _list_node list_node;

#endif
