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
      myNode = newNode(idProcess,
			     coord,
			     newSpaceWithCoord(COORD_MIN_X,
					       COORD_MIN_Y,
					       COORD_MAX_X,
					       COORD_MAX_Y));
      
      MPI_Send(&buf, 1, MPI_INT, INIT_NODE, DONE_INSERT,
	       MPI_COMM_WORLD);
    } else {
      /* recherche d'un node où je peux m'insérer, en commençant par
	 le bootstrap */
      int trg = BOOTSTRAP_NODE;
      node tmpNode;

      while(1){
	/* demande d'insertion */
	MPI_Send(&buf, 1, MPI_INT, trg, WANNA_INSERT, MPI_COMM_WORLD);
      
	/* réponse */
	MPI_Recv(&tmpNode, sizeof(node), MPI_BYTE, trg, TRY_INSERT,
		 MPI_COMM_WORLD, &status);

	/* traitement de la réponse : */

	/* suis-je dans l'espace du noeud ? */
	if( isPointInNodesSpace(coord, tmpNode) ){
	  /* si oui c'est que j'ai trouvé le noeud où je peux m'insérer
	     je quitte donc la boucle */
	  break;
	  
	}

	/* sinon je dois trouver une nouveau trg */
	trg = findInsertDirection(coord, tmpNode);
	trg = ( trg < NB_DIRECTIONS ) ? trg : chooseDirectionRandomly(trg);
	trg = tmpNode.neighbors[trg]->n->id;
      }
      
      /* Je ne peux pas m'insérer si l'espace ne fait qu'un seul pixel */
      if(tmpNode.area->south_west->x + 1 == tmpNode.area->north_east->x
	 && tmpNode.area->south_west->y + 1 == tmpNode.area->north_east->y){
	tmpNode.id = -1;
	MPI_Send(&tmpNode, sizeof(node), MPI_BYTE, trg, TRYED_INSERT,
		 MPI_COMM_WORLD);
	return;
      }

      /* je peux maintenant créer mon noeud */
      myNode = newNode(idProcess,
			     coord,
			     getNodesSubSpace(&newNode));

      /* dernière chose à vérifier, 
	 mes coords doivent etre dans le sous-espace,
	 et si ce n'est pas le cas je dois tirer des nouvelles
	 coord dans cet espace */
      if( !isPointInNodesSpace(coord, &myNode) ){
	myNode.coord->x = (rand()%myNode.area->north_east->x)
	  + myNode.area->south_west->x;
	myNode.coord->y = (rand()%myNode.area->north_east->y)
	  + myNode.area->south_west->y;
      }
      
      /* notification de bonne insertion */
      MPI_Send(myNode, sizeof(node), MPI_BYTE, trg, TRYED_INSERT, 
	       MPI_COMM_WORLD);
    }
  }
  
}


/*
 * @todo si je peux me passer de cette merde ..
 * @todo cette merde est largement optimisable 
 */
space* getNodesSubSpace(node* src)
{
  int width  = src->area->north_east->x - src->area->south_west->x;
  int height = src->area->north_east->y - src->area->south_west->y;
  
  if( height > width ){
    if( isNodeInCoord(src, src->area->south_west->x, src->area->south_west->y,
		      src->area->north_east->x, src->area->south_west->y + (height/2)) ){
      return newSpaceWithCoord(src->area->south_west->x,
			       src->area->south_west->y + (height/2),
			       src->area->north_east->x,
			       src->area->north_east->y);
    } else {
      return newSpaceWithCoord(src->area->south_west->x,
			       src->area->south_west->y,
			       src->area->north_east->x,
			       src->area->south_west->y + (height/2));
    }
  } else {
     if( isNodeInCoord(src, src->area->south_west->x, src->area->south_west->y,
		       src->area->south_west->x + (width/2), src->area->north_east->y) ){
       return newSpaceWithCoord(src->area->south_west->x + (width/2),
				src->area->south_west->y,
				src->area->north_east->x,
				src->area->north_east->y);
     } else {
       return newSpaceWithCoord(src->area->south_west->x,
				src->area->south_west->y,
				src->area->south_west->x + (width/2),
				src->area->north_east->y);
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

int findInsertDirection(point* p, node* trg)
{
  if( p->x >= trg->area->north_east->x &&
      p->y >= trg->area->south_west->y &&
      p->y <  trg->area->north_east->y )
    return EAST;
  if( p->x >= trg->area->south_west->x &&
      p->x <  trg->area->north_east->x  &&
      p->y <  trg->area->south_west->y)
    return SOUTH;
  if( p->x <  trg->area->south_west->x &&
      p->y >= trg->area->south_west->y &&
      p->y <  trg->area->north_east->y )
    return WEST;
  if( p->x >= trg->area->south_west->x &&
      p->x <  trg->area->north_east->x  &&
      p->y >= trg->area->north_east->y )
    return NORTH;
  if(p->x >= trg->area->north_east->x &&
     p->y >= trg->area->north_east->y)
    return NORTHEAST;
  if(p->x <  trg->area->south_west->x &&
     p->y >= trg->area->north_east->y)
    return NORTHWEST;
  if(p->x >= trg->area->north_east->x &&
     p->y <  trg->area->south_west->y)
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

/*
 * On sauvegarde au cas ou ...
 */
 
//int manageSpaceInsert(node* src, node* trg)
//{
//  src->area = getNodesSubSpace(trg);
//  /* si c'est le même x c'est que trg est ou en haut ou à droite */
//  if( src->area->south_west->x == trg->area->south_west->x ){
//    /* si c'est le même y en north_east c'est à droite */
//    if( src->area->north_east->y == trg->area->north_east->y ){
//      /* à droite */
//      trg->area->south_west->x = src->area->north_east->x;
//      src->neighbors[EAST]->n = trg;
//      /* @todo vider la liste avant de la reremplir */
//      src->neighbors[NORTH]   = cloneListNode(trg->neighbors[NORTH]);
//      src->neighbors[SOUTH]   = cloneListNode(trg->neighbors[SOUTH]);
//      src->neighbors[WEST]    = trg->neighbors[WEST];
//      trg->neighbors[WEST]    = newListNodeWithNode(src);
//      /* update neighbors */
//      updateNeighborsForDirection(src->neighbors[WEST], EAST, src, trg);
//    } else {
//      trg->area->south_west->y = src->area->north_east->y;
//    }
//  } else {
//    if( src->area->south_west->y == trg->area->south_west->y ){
//      /* à gauche */
//      trg->area->north_east->x = src->area->south_west->x;
//    } else {
//      trg->area->north_east->y = src->area->south_west->y;
//    }
//  }
//}

/*
 * Quatre cas possible
 * Deux verticales, Deux horizontales.
 * 
 * A voir si on modifie ici l'espace et les voisins de trg ou pas.
 */
int manageSpaceInsert(node* src, node* trg)
{
  src->area = getNodesSubSpace(trg);
  
  
  if( src->area->south_west->x == trg->area->south_west->x ){
	/* séparation horizontale (2 cas) ou verticale avec src a gauche */
	if( src->area->south_west->y == trg->area->south_west->y ){
		/* séparation horizontale src sous trg ou verticale avec src a gauche */
		if( src->area->north_east->y == trg->area->north_east->y ){
			/* Src a GAUCHE de Trg */
			/* Voisin de src */
			src->neighbors[EAST]->n = newListNode(trg, NULL);
			src->neighbors[WEST]->n = cloneListNode(trg->neighbors[WEST]);
			
			/* A voir en fonction des espaces */
			src->neighbors[NORTH]->n = cloneListNode(trg->neighbors[NORTH]);
			src->neighbors[SOUTH]->n = cloneListNode(trg->neighbors[SOUTH]);
		}else{
			/* Src en BAS de Trg */
			/* Voisin de src */
			src->neighbors[NORTH]->n = newListNode(trg, NULL);
			src->neighbors[SOUTH]->n = cloneListNode(trg->neighbors[SOUTH]);
			
			/* A voir en fonction des espaces */
			src->neighbors[EAST]->n = cloneListNode(trg->neighbors[EAST]);
			src->neighbors[WEST]->n = cloneListNode(trg->neighbors[WEST]);
		}
	}else{
		/* Src a en HAUT de Trg */
		/* Voisin de src */
		src->neighbors[NORTH]->n = cloneListNode(trg->neighbors[NORTH]);
		src->neighbors[SOUTH]->n = newListNode(trg, NULL);
			
		/* A voir en fonction des espaces */
		src->neighbors[EAST]->n = cloneListNode(trg->neighbors[EAST]);
		src->neighbors[WEST]->n = cloneListNode(trg->neighbors[WEST]);
	}
  } else{
	/* Src à DROITE de Trg */
	/* Voisin de src */
	src->neighbors[EAST]->n = cloneListNode(trg->neighbors[EAST])
	src->neighbors[WEST]->n = newListNode(trg, NULL);
			
	/* A voir en fonction des espaces */
	src->neighbors[NORTH]->n = cloneListNode(trg->neighbors[NORTH]);
	src->neighbors[SOUTH]->n = cloneListNode(trg->neighbors[SOUTH]);
  }
  return 1;
}

int insertNodeInNode(node* src, node* trg)
{
  src->area = getNodesSubSpace(trg);
  
}
