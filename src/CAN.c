#include <CAN.h>

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

/*******************************************************************************
 * Initialization
 ******************************************************************************/
int nbProcess;
int idProcess;
void CANinitialize()
{
  /* getting total number of process */
  MPI_Comm_size(MPI_COMM_WORLD, &nbProcess);

  /* getting my process id */
  MPI_Comm_rank(MPI_COMM_WORLD, &idProcess);
}

/*******************************************************************************
 * Etape 1 : Insertion de noeud
 ******************************************************************************/
void CANinsert()
{
  int buf, i;
  MPI_Status status;

  if( idProcess == INIT_NODE ){
    /* Le coordinateur demande au bootstrap de s'insérer */
    MPI_Send(&buf, 1, MPI_INT, BOOTSTRAP_NODE, CAN_INSERT, MPI_COMM_WORLD);

    /* Attente de bonne insertion du bootstrap */
    MPI_Recv(&buf, 1, MPI_INT, BOOTSTRAP_NODE, DONE_INSERT,
	     MPI_COMM_WORLD, &status);

    for(i=0; i<nbProcess; i++){
      if( i != BOOTSTRAP_NODE && i != INIT_NODE ){
	/* On lui demande de s'inserer */
	MPI_Send(&buf, 1, MPI_INT, i, CAN_INSERT, MPI_COMM_WORLD);

	/* Attente de bonne insertion du noeud */
	MPI_Recv(&buf, 1, MPI_INT, i, MPI_ANY_TAG,
		 MPI_COMM_WORLD, &status);

	if( status.MPI_TAG == FAILED_INSERT ){
	  fprintf(stderr, "ERROR: Failed insert\n");
	  continue;
	}
      }
    }
    
  } else {
    /* chaque process tire aléatoirement son point */
    point* coord = newRandomPoint();

    /* attente de l'autorisation à s'insérer */
    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, U_CAN_INSERT, 
	     MPI_COMM_WORLD, &status);

    if( idProcess == BOOTSTRAP_NODE ){
      /* le BOOTSTRAP_NODE sera toujours le premier à s'insérer donc
	 il prendra par défaut toute la surface */
      node* myNode = newNode(idProcess,
			     coord,
			     newSpaceWithCoord(COORD_MIN_X,
					       COORD_MIN_Y,
					       COORD_MAX_X,
					       COORD_MAX_Y));
      
      MPI_Send(&buf, 1, MPI_INT, INIT_NODE, DONE_INSERT,
	       MPI_COMM_WORLD);
    } else {
      
    }
  }
  
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
