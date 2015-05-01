#include <CAN.h>

#include <point.h>
#include <space.h>
#include <node.h>
#include <list_node.h>

/* each CAN process has : */
node myNode; /* it's node */

/*******************************************************************************
 * Private Declaration
 ******************************************************************************/
void CANinsertInit();
void CANinsertBootstrap();
void CANinsertNode();
void CANhandleInsertRequest(node* n);
int findInsertDirection(point* p, node* trg);
int chooseDirectionRandomly(int direction);

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
 * Step 1 : Insertion
 ******************************************************************************/
void CANinsert()
{
  if( idProcess == INIT_NODE ){
    CANinsertInit();
  }
  else if( idProcess == BOOTSTRAP_NODE ){
    CANinsertBootstrap();
  }
  else {
    CANinsertNode();
  }

  return;
}

void CANinsertInit()
{
  int buf, i;
  MPI_Status status;

  /* Le coordinateur demande au bootstrap de s'insérer */
  MPI_Send(&buf, 1, MPI_INT, BOOTSTRAP_NODE, U_CAN_INSERT, MPI_COMM_WORLD);
  
  /* Attente de bonne insertion du bootstrap */
  MPI_Recv(&buf, 1, MPI_INT, BOOTSTRAP_NODE, DONE_INSERT,
	   MPI_COMM_WORLD, &status);

  for(i=0; i<nbProcess; i++){
    if( i != BOOTSTRAP_NODE && i != INIT_NODE ){
      /* On lui demande de s'inserer */
      MPI_Send(&buf, 1, MPI_INT, i, U_CAN_INSERT, MPI_COMM_WORLD);
      
      /* Attente de bonne insertion du noeud */
      MPI_Recv(&buf, 1, MPI_INT, i, MPI_ANY_TAG,
	       MPI_COMM_WORLD, &status);

      if( status.MPI_TAG == FAILED_INSERT ){
	fprintf(stderr, "ERROR: Failed insert\n");
	continue;
      }
    }
  }
}

void CANinsertBootstrap()
{
  int buf;
  MPI_Status status;

  /* attente de l'autorisation à s'insérer */
  MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, U_CAN_INSERT, 
	   MPI_COMM_WORLD, &status);
  
  /* le BOOTSTRAP_NODE sera toujours le premier à s'insérer donc
     il prendra par défaut toute la surface */
  myNode = newNode(idProcess,
		   newRandomPoint(),
		   newSpaceWithCoord(COORD_MIN_X,
				     COORD_MIN_Y,
				     COORD_MAX_X,
				     COORD_MAX_Y));
  
  MPI_Send(&buf, 1, MPI_INT, INIT_NODE, DONE_INSERT,
	   MPI_COMM_WORLD);
  
  return;
}

void CANinsertNode()
{
  /* attente d'autorisation du boss */
  int buf;
  MPI_Status status;
  MPI_Recv(&buf, 1, MPI_INT, INIT_NODE, U_CAN_INSERT,
	   MPI_COMM_WORLD, &status);

  /* allocation du noeud avec génération aléatoire des coordonnées */
  myNode = newNodeWithRandomPoint(idProcess);

  /* requete d'insertion -> bootstrap */
  MPI_Send(&myNode, sizeof(node), MPI_BYTE, BOOTSTRAP_NODE,
	   REQUEST_INSERT, MPI_COMM_WORLD);

  /* attente de réponse */
  MPI_Recv(&myNode, sizeof(node), MPI_BYTE, MPI_ANY_SOURCE,
	   REQUEST_INSERT, MPI_COMM_WORLD, &status);
  
  /* notification du boss */
  if(myNode.id == -1){
    MPI_Send(&buf, 1, MPI_INT, INIT_NODE, FAILED_INSERT, MPI_COMM_WORLD);
    return;
  }
  MPI_Send(&buf, 1, MPI_INT, INIT_NODE, DONE_INSERT, MPI_COMM_WORLD);
}

void CANhandleInsertRequest(node* n)
{
  /* @todo si mon espace ne fait qu'un seul pixel, l'insertion est impossible */

  printf("[%d] is handling insert request from %d\n", idProcess, n->id);
  fflush(NULL);
  /* le noeud fait-il partie de mon espace ? */
  if( isPointInNodesSpace(&(n->coord), &myNode) ){
    /* si oui, je lui donne d'abord son sous espace */
    n->area = splitNodesSpace(&myNode);

    /* si je ne suis pas dans mon sous-espace je retire une nouvelle 
       valeur dedans ... */
    if( !isPointInNodesSpace(&(n->coord), n) ){
      n->coord.x = (rand()%n->area.north_east.x) + n->area.south_west.x;
      n->coord.y = (rand()%n->area.north_east.y) + n->area.south_west.y;
    }

    /* insertion done */
    MPI_Send(n, sizeof(node), MPI_BYTE, n->id, REQUEST_INSERT, MPI_COMM_WORLD);
  }
  else {
    /* redirection d'insertion */
    int trg = findInsertDirection(&(n->coord), &myNode);
    if( trg >= NB_DIRECTIONS )
      trg = chooseDirectionRandomly(trg);
    printf("HELLO\n");
    MPI_Send(n, sizeof(node), MPI_BYTE, myNode.neighbors[trg].idList[0], 
	     REQUEST_INSERT, MPI_COMM_WORLD);
    printf("OLLEH\n");
  }
  
  return;
}
/*******************************************************************************
 * Message handling ( may be better in an other thread )
 ******************************************************************************/
void CANhandleMessage()
{
  node buf;
  MPI_Status status;

  printf("%d waiting for message\n", myNode.id);

  /* waiting for message */
  MPI_Recv(&buf, sizeof(node), MPI_BYTE, MPI_ANY_SOURCE,
	   MPI_ANY_TAG, MPI_COMM_WORLD, &status);

  printf("%d handled %d message\n", myNode.id, buf.id);

  switch(status.MPI_TAG)
    {
    case REQUEST_INSERT: CANhandleInsertRequest(&buf); break;
    default: break;
    }

  return;
}

/*******************************************************************************
 * Other Operations 
 ******************************************************************************/
int findInsertDirection(point* p, node* trg)
{
  if( p->x >= trg->area.north_east.x &&
      p->y >= trg->area.south_west.y &&
      p->y <  trg->area.north_east.y )
    return EAST;
  if( p->x >= trg->area.south_west.x &&
      p->x <  trg->area.north_east.x  &&
      p->y <  trg->area.south_west.y)
    return SOUTH;
  if( p->x <  trg->area.south_west.x &&
      p->y >= trg->area.south_west.y &&
      p->y <  trg->area.north_east.y )
    return WEST;
  if( p->x >= trg->area.south_west.x &&
      p->x <  trg->area.north_east.x  &&
      p->y >= trg->area.north_east.y )
    return NORTH;
  if(p->x >= trg->area.north_east.x &&
     p->y >= trg->area.north_east.y)
    return NORTHEAST;
  if(p->x <  trg->area.south_west.x &&
     p->y >= trg->area.north_east.y)
    return NORTHWEST;
  if(p->x >= trg->area.north_east.x &&
     p->y <  trg->area.south_west.y)
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
