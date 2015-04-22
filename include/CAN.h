#ifndef __CAN_H_
#define __CAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <MPI.h>

#define COORD_MIN 0
#define COORD_MAX 1000

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

#define BOOTSTRAP_NODE 1

typedef struct _point     point;
typedef struct _space     space;
typedef struct _node      node;
typedef struct _list_node list_node;

struct _point {
  int x;
  int y;
};

struct _space{
  point* down_left;
  point* up_right;
};

struct _node {
  int id;
  point* coord;
  space* area;
  list_node* neighbors[NB_DIRECTIONS];
};

struct _list_node {
  node* n;
  list_node* next;
};

#endif
