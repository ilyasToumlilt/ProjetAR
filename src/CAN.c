#include <CAN.h>

/*******************************************************************************
 * Gestion de listes 
 ******************************************************************************/
list_node* newListNodeWithNode(node* n)
{
  list_node* ret = (list_node*)malloc(sizeof(list_node));
  ret->n = n;
  ret->next = NULL;
  return ret;
}

list_node* newListNode()
{
  return newListNodeWithNode(NULL);
}

/* @todo opti par rapport à la fréquence de parcours */
list_node* addNodeToListNode(list_node* ln, node* n)
{
  list_node* ret = newListNodeWithNode(n);
  ret->next = ln;
  return ret;
}

node* popNodeFromListNode(list_node** ln, int nodeId)
{
  node* ret = NULL;
  if( (*ln)->n ){
    list_node *prev = NULL, *tmp = *ln;
    while( tmp ){
      if( tmp->n->id == nodeId ){
	if( prev )
	  prev->next = tmp->next;
	else
	  (*ln) = (*ln)->next;
      }
      prev = tmp;
      tmp = tmp->next;
    }
  }
  return ret;
}

list_node* cloneListNode(list_node* ln)
{
  list_node* ret = newListNodeWithNode(ln->n);
  list_node* tmp = ret;
  while(ln->next) {
    ln = ln->next;
    tmp->next = newListNodeWithNode(ln->n);
    tmp = tmp->next;
  }
  return ret;
}

/* src : le nouveau né 
 * trg : le vieux con 
 */
void updateNeighborsForDirection(list_node* ln, int dir, node* src, node* trg)
{
  while(ln){
    popNodeFromListNode(&(ln->n->neighbors[dir]), trg->id);
    ln->n->neighbors[dir] = addNodeToListNode(ln->n->neighbors[dir], src);
    ln = ln->next;
  }
}

void freeListNode(list_node* ln)
{
  if( ln ){
    freeListNode(ln->next);
    free(ln);
  }

  return;
}

/* Je m'engage à ne pas toucher les fonctions précédemment écrites ( ou pas ) */

/*******************************************************************************
 * Primitives
 ******************************************************************************/
/* Point */
point* newPoint(int x, int y)
{
  point* ret = (point*)malloc(sizeof(point));
  ret->x = x;
  ret->y = y;

  return ret;
}

void freePoint(point* p)
{
  free(p);

  return;
}

/*** Space ***/
space* newSpace(point* down_left, point* up_right)
{
  space* ret = (space*)malloc(sizeof(space));
  ret->down_left = down_left;
  ret->up_right  = up_right;

  return ret;
}

space* newSpaceWithCoord(int x1, int y1, int x2, int y2)
{
  space* ret = (space*)malloc(sizeof(space));

  ret->down_left = newPoint(x1, y1);
  ret->up_right  = newPoint(x2, y2);

  return ret;
}

void freeSpace(space* sp)
{
  freePoint(sp->down_left);
  freePoint(sp->up_right);
  free(sp);

  return;
}

/*** Node ***/
node* newNode(int id, point* coord, space* area)
{
  node* ret = (node*)malloc(sizeof(node));

  ret->id = id;
  ret->coord = coord;
  ret->area = area;
  int i;
  for(i=0; i<NB_DIRECTIONS;i++)
    ret->neighbors[i] = newListNode();

  return ret;
}

void freeNode(node* n)
{
  freePoint(n->coord);
  freeSpace(n->area);
  int i;
  for(i=0; i<NB_DIRECTIONS; i++)
    freeListNode(n->neighbors[i]);
  free(n);

  return;
}

/*******************************************************************************
 * Etape 1 : Insertion de noeud
 ******************************************************************************/
point* newRandomPoint()
{
  srand(getpid());
  return newPoint(rand()%COORD_MAX, rand()%COORD_MAX);
}

node* makeNode(int id)
{
  return newNode(id, newRandomPoint(), NULL);
}

int isNodeInCoord(node* src, int x1, int y1, int x2, int y2)
{
  return (src->coord->x >= x1 &&
	  src->coord->x <  x2 &&
	  src->coord->y >= y1 &&
	  src->coord->y <  y2) ? 1 : 0;
}

int isNodeInSpace(node* src, space* sp)
{
  return isNodeInCoord(src,
		       sp->down_left->x,
		       sp->down_left->y,
		       sp->up_right->x,
		       sp->up_right->y);
}

/*
 * Is src in trg's space ?
 */
int isNodeInNodesSpace(node* src, node* trg)
{
  return isNodeInSpace(src, trg->area);
}

/*
 * @todo si je peux me passer de cette merde ..
 * @todo cette merde est largement optimisable 
 */
space* getNodesSubSpace(node* src)
{
  int width  = src->area->up_right->x - src->area->down_left->x;
  int height = src->area->up_right->y - src->area->down_left->y;
  
  if( height > width ){
    if( isNodeInCoord(src, src->area->down_left->x, src->area->down_left->y,
		      src->area->up_right->x, src->area->down_left->y + (height/2)) ){
      return newSpaceWithCoord(src->area->down_left->x,
			       src->area->down_left->y + (height/2),
			       src->area->up_right->x,
			       src->area->up_right->y);
    } else {
      return newSpaceWithCoord(src->area->down_left->x,
			       src->area->down_left->y,
			       src->area->up_right->x,
			       src->area->down_left->y + (height/2));
    }
  } else {
     if( isNodeInCoord(src, src->area->down_left->x, src->area->down_left->y,
		       src->area->down_left->x + (width/2), src->area->up_right->y) ){
       return newSpaceWithCoord(src->area->down_left->x + (width/2),
				src->area->down_left->y,
				src->area->up_right->x,
				src->area->up_right->y);
     } else {
       return newSpaceWithCoord(src->area->down_left->x,
				src->area->down_left->y,
				src->area->down_left->x + (width/2),
				src->area->up_right->y);
    }
  }
}

/*
 * Can src be inserted in trg's area 
 */
space* isNodeInsertable(node* src, node* trg)
{
  space* s = getNodesSubSpace(trg);
  if( !isNodeInSpace(src, s) ){
    freeSpace(s);
    return NULL;
  }
  return s;
}

/*
 * @todo c'est mieux, tellement mieux, d'inserer un point.
 */
int insertNodeFromBootstrap(node* src)
{
  return insertNodeFromNode(src, /* BOOTSTRAP */);
}

int insertNodeFromNode(node* src, node* trg)
{
  if( isNodeInsertable(src, trg) ){
    insertNodeInNode(src, trg); /* @todo on le recup déjà ici */
    return 1;
  }
  /* @todo je le fais d'un manière naïve et puis faudra réfléchir au parcours log n */
  if( isNodeInNodesSpace(src, trg) )
    return 0; /* @todo on change pour que ça passe */

  int dir = findInsertDirection(src, trg);
  if( dir < NB_DIRECTIONS )
    return insertNodeFromNode(src, trg->neighbors[dir]->n);
  return insertNodeFromNode(src, trg->neighbors[chooseDirectionRandomly(dir)]->n);
}

int findInsertDirection(node* src, node* trg)
{
  if( src->coord->x >= trg->area->up_right->x &&
      src->coord->y >= trg->area->down_left->y &&
      src->coord->y <  trg->area->up_right->y )
    return EAST;
  if( src->coord->x >= trg->area->down_left->x &&
      src->coord->x <  trg->area->up_right->x  &&
      src->coord->y <  trg->area->down_left->y)
    return SOUTH;
  if( src->coord->x <  trg->area->down_left->x &&
      src->coord->y >= trg->area->down_left->y &&
      src->coord->y <  trg->area->up_right->y )
    return WEST;
  if( src->coord->x >= trg->area->down_left->x &&
      src->coord->x <  trg->area->up_right->x  &&
      src->coord->y >= trg->area->up_right->y )
    return NORTH;
  if(src->coord->x >= trg->area->up_right->x &&
     src->coord->y >= trg->area->up_right->y)
    return NORTHEAST;
  if(src->coord->x <  trg->area->down_left->x &&
     src->coord->y >= trg->area->up_right->y)
    return NORTHWEST;
  if(src->coord->x >= trg->area->up_right->x &&
     src->coord->y <  trg->area->down_left->y)
    return SOUTHEAST;
  return SOUTHWEST;
}

int chooseDirectionRandomly(int direction)
{
  switch(direction){
  case NORTHWEST:
    return (rand()%2) ? NORTH : WEST;
  case NORTHEAST:
    return (rand()%2) ? NORTH : EAST;
  case SOUTHWEST:
    return (rand()%2) ? SOUTH : WEST;
  default:
    return (rand()%2) ? SOUTH : EAST;
  }
}

int manageSpaceInsert(node* src, node* trg)
{
  src->area = getNodesSubSpace(trg);
  /* si c'est le même x c'est que trg est ou en haut ou à droite */
  if( src->area->down_left->x == trg->area->down_left->x ){
    /* si c'est le même y en up_right c'est à droite */
    if( src->area->up_right->y == trg->area->up_right->y ){
      /* à droite */
      trg->area->down_left->x = src->area->up_right->x;
      src->neighbors[EAST]->n = trg;
      /* @todo vider la liste avant de la reremplir */
      src->neighbors[NORTH]   = cloneListNode(trg->neighbors[NORTH]);
      src->neighbors[SOUTH]   = cloneListNode(trg->neighbors[SOUTH]);
      src->neighbors[WEST]    = trg->neighbors[WEST];
      trg->neighbors[WEST]    = newListNodeWithNode(src);
      /* update neighbors */
      updateNeighborsForDirection(src->neighbors[WEST], EAST, src, trg);
    } else {
      trg->area->down_left->y = src->area->up_right->y;
    }
  } else {
    if( src->area->down_left->y == trg->area->down_left->y ){
      /* à gauche */
      trg->area->up_right->x = src->area->down_left->x;
    } else {
      trg->area->up_right->y = src->area->down_left->y;
    }
  }
}

int insertNodeInNode(node* src, node* trg)
{
  src->area = getNodesSubSpace(trg);
  
}
