#include <CAN.h>
#include <CANinsertData.h>
#include <display.h>

#include <point.h>
#include <space.h>
#include <node.h>
#include <data.h>
#include <list_node.h>

/* each CAN process has : */
node myNode; /* it's node */

node getNode()
{
  return myNode;
}

void setNode(node n)
{
  myNode = n;
}

/*******************************************************************************
 * Private Declaration
 ******************************************************************************/
void CANinsertInit();
void CANinsertBootstrap();
void CANinsertNode();
void CANhandleInsertRequest(node* n);
void CANhandleAddNeighbor(node* n);
void CANhandleRmvNeighbor(node* n);
void CANhandleInfoRequest(node* n);
int findInsertDirection(point* p, node* trg);
int chooseDirectionRandomly(int direction);
void updateNeighbors(int dir, node* n);
void recalculateNeighborsForDirection(int dir, node* n);
int isNodeNeighbor(int dir, node* src, node* trg);

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

  buf = idProcess;
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

void CANend()
{
  int buf, i;

  for(i=0; i<nbProcess; i++){
    if( i != INIT_NODE){
      /* On lui demande de s'inserer */
      MPI_Send(&buf, 1, MPI_INT, i, END, MPI_COMM_WORLD);      
    }
  }
}

void CANinsertBootstrap()
{
  int buf = idProcess;
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

    /* update des voisins */
    updateNeighbors(findInsertDirection(&(n->coord), &myNode), n);

    /* insertion done */
    MPI_Send(n, sizeof(node), MPI_BYTE, n->id, REQUEST_INSERT, MPI_COMM_WORLD);
  }
  else {
    /* redirection d'insertion */
    int trg = findInsertDirection(&(n->coord), &myNode);
    if( trg >= NB_DIRECTIONS )
      trg = chooseDirectionRandomly(trg);

    MPI_Send(n, sizeof(node), MPI_BYTE, myNode.neighbors[trg].idList[0], 
       REQUEST_INSERT, MPI_COMM_WORLD);
  }
  
  return;
}
/*******************************************************************************
 * Message handling ( may be better in an other thread )
 ******************************************************************************/
int CANhandleMessage()
{
  
  MPI_Status status;

  printf("%d waiting for message\n", myNode.id);

  /* waiting for message */
  MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  
  printf("%d handled %d message\n", myNode.id, status.MPI_SOURCE);

  /* SI on doit traiter une data */
  if(status.MPI_TAG == REQUEST_INSERT_DATA){
    data buf;
    MPI_Recv(&buf, sizeof(data), MPI_BYTE, MPI_ANY_SOURCE,
     REQUEST_INSERT_DATA, MPI_COMM_WORLD, &status);

    CANhandleInsertDataRequest(&buf);
  }else{
    node buf;
    MPI_Recv(&buf, sizeof(node), MPI_BYTE, MPI_ANY_SOURCE,
     MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    switch(status.MPI_TAG)
      {
      case REQUEST_INSERT: CANhandleInsertRequest(&buf); break;
      case ADD_NEIGHBOR: CANhandleAddNeighbor(&buf); break;
      case RMV_NEIGHBOR: CANhandleRmvNeighbor(&buf); break;
      case INFO_REQUEST: CANhandleInfoRequest(&buf); break;
      case WANT_INFO: handleDisplayRequest(); break;
      case END: return -1;
      default: break;
      }
  }
  return 1;
}

void CANhandleAddNeighbor(node* n)
{
  pushNodeToListNode(&(n->neighbors[findInsertDirection(&(n->coord), &myNode)]),
         n->id);

  int buf;
  MPI_Send(&buf, 1, MPI_INT, n->id, ADD_NEIGHBOR_ACK, MPI_COMM_WORLD);
}

void CANhandleRmvNeighbor(node* n)
{
  popNodeFromListNodeById(&(n->neighbors[findInsertDirection(&(n->coord), &myNode)])
        , n->id);

  int buf;
  MPI_Send(&buf, 1, MPI_INT, n->id, RMV_NEIGHBOR_ACK, MPI_COMM_WORLD);
}

void CANhandleInfoRequest(node* n)
{
  MPI_Send(&myNode, sizeof(node), MPI_BYTE, n->id, INFO_REQUEST_ACK,
     MPI_COMM_WORLD);
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

/* dir : insert direction
   n : new inserted node 
*/ 
void updateNeighbors(int dir, node* n)
{
  int i;
  /* le nouveau prend tous mes voisin de sa direction d'insert */ 
  for(i=0; i<myNode.neighbors[dir].size; i++)
    n->neighbors[dir].idList[i]=myNode.neighbors[dir].idList[i];
  n->neighbors[dir].size = myNode.neighbors[dir].size;
  /* et devient mon unique voisin dans cette direction */
  myNode.neighbors[dir].idList[0] = n->id;
  myNode.neighbors[dir].size = 1;
  /* je suis également son unique voisin dans la direction opposée */
  n->neighbors[(dir+2)%NB_DIRECTIONS].idList[0] = myNode.id;
  n->neighbors[(dir+2)%NB_DIRECTIONS].size = 1;
  /* pour les deux directions restantes il faudra recalculer par 
     rapport aux limites */
  recalculateNeighborsForDirection((dir+1)%NB_DIRECTIONS, n);
  recalculateNeighborsForDirection((dir+3)%NB_DIRECTIONS, n);
  
  return;
}

void recalculateNeighborsForDirection(int dir, node* n)
{
  int i, tmp;
  node buf;
  MPI_Status status;
  for(i=0; i<myNode.neighbors[dir].size; i++){
    MPI_Send(&myNode, sizeof(node), MPI_BYTE, myNode.neighbors[dir].idList[i],
       INFO_REQUEST, MPI_COMM_WORLD);
    MPI_Recv(&buf, sizeof(node), MPI_BYTE, myNode.neighbors[dir].idList[i],
       INFO_REQUEST_ACK, MPI_COMM_WORLD, &status);
    if( !isNodeNeighbor(dir, &myNode, &buf) ){
      popNodeFromListNodeById(&(n->neighbors[dir]), buf.id);
      i--;
      MPI_Send(&myNode, sizeof(node), MPI_BYTE, buf.id, RMV_NEIGHBOR,
         MPI_COMM_WORLD);
      MPI_Recv(&tmp, 1, MPI_INT, buf.id, RMV_NEIGHBOR_ACK,
         MPI_COMM_WORLD, &status);
    }
    if( isNodeNeighbor(dir, n, &buf) ){
      pushNodeToListNode(&(n->neighbors[dir]), buf.id);
      MPI_Send(n, sizeof(node), MPI_BYTE, buf.id, ADD_NEIGHBOR,
         MPI_COMM_WORLD);
      MPI_Recv(&tmp, 1, MPI_INT, buf.id, ADD_NEIGHBOR_ACK,
         MPI_COMM_WORLD, &status);
    }
  }
  return;
}

/* is trg src's neighbor in dir direction ? */
int isNodeNeighbor(int dir, node* src, node* trg)
{
  if( dir == NORTH || dir == SOUTH ){
    if(trg->area.south_west.x < src->area.north_east.x ||
       trg->area.north_east.x > src->area.south_west.x )
      return 1;
  }
  else {
    if(trg->area.south_west.y < src->area.north_east.y ||
       trg->area.north_east.y > src->area.south_west.y)
      return 1;
  }
  return 0;
}