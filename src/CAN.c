#include <CAN.h>
#include <CANinsertData.h>
#include <CANsearchData.h>
#include <display.h>


#include <point.h>
#include <space.h>
#include <node.h>
#include <list_node.h>

/* each CAN process has : */
node myNode; /* it's node */

node getNode()
{
  return myNode;
}
void setNode(node n){ myNode = n;}

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
void CANhandleRemoveRequest(node* n);
int findNodesDirection(node* n);
void updateNeighbors(int dir, node* n);
void recalculateNeighborsForDirection(int dir, node* n);
int  isNodeNeighbor(int dir, node* src, node* trg);
void propagateInNodesSpace(int dir, node* n);
void updateNeighborsAfterRemove(int dir, node* n);
void recalculateNeighborsAfterRemove(int dir, node* n);

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

      printf("INIT: inserted node %d\n", status.MPI_SOURCE);

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

  printNode(myNode);
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
 * Step 4 : Remove
 ******************************************************************************/
void CANremove()
{
  /* si j'ai un voisinnage avec un seul voisin je lui attribue directement mon
     espace, c'est le plus simple */
  int i;
  MPI_Status status;
  for(i=0; i<NB_DIRECTIONS; i++){
    if(myNode.neighbors[i].size == 1){
      MPI_Send(&myNode, sizeof(node), MPI_BYTE, myNode.neighbors[i].idList[0],
         REQUEST_REMOVE, MPI_COMM_WORLD);
      MPI_Recv(&myNode, sizeof(node), MPI_BYTE, myNode.neighbors[i].idList[0],
         REQUEST_REMOVE, MPI_COMM_WORLD, &status);
      return;
    }
  }

  /* sinon je prends le coté avec le moins de voisins 
     ( le moins de messages ) */
  int min = 0;
  for(i=0; i<NB_DIRECTIONS; i++){
    min = ( myNode.neighbors[i].size > i ) ? myNode.neighbors[i].size : i;
  }
  if( min ){
    for(i=0; i<myNode.neighbors[min].size; i++){
      MPI_Send(&myNode, sizeof(node), MPI_BYTE, myNode.neighbors[min].idList[i],
         REQUEST_REMOVE, MPI_COMM_WORLD);
      MPI_Recv(&myNode, sizeof(node), MPI_BYTE, myNode.neighbors[min].idList[i],
         REQUEST_REMOVE, MPI_COMM_WORLD, &status);
    }
  }
  return;
}

/*******************************************************************************
 * Message handling ( may be better in an other thread )
 ******************************************************************************/
int CANhandleMessage()
{
  node buf;
  data data;
  MPI_Status status;

  /* waiting for message */
  MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  
  /* Si il s'agit d'un traitement sur une donnée */
  if(status.MPI_TAG == REQUEST_INSERT_DATA || status.MPI_TAG == REQUEST_SEARCH_DATA){

    MPI_Recv(&data, sizeof(data), MPI_BYTE, MPI_ANY_SOURCE,
    MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    switch(status.MPI_TAG)
      {
      case REQUEST_INSERT_DATA: CANhandleInsertDataRequest(&data); break;
      case REQUEST_SEARCH_DATA: CANhandleSearchDataRequest(&data); break;
      default: break;
      }
    
  }
  else{ 
      MPI_Recv(&buf, sizeof(node), MPI_BYTE, MPI_ANY_SOURCE,
     MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    printf("%d handled %d from %d message type %d\n", myNode.id, buf.id, status.MPI_SOURCE, status.MPI_TAG);

    switch(status.MPI_TAG)
      {
      case REQUEST_INSERT: CANhandleInsertRequest(&buf); break;
      case ADD_NEIGHBOR: CANhandleAddNeighbor(&buf); break;
      case RMV_NEIGHBOR: CANhandleRmvNeighbor(&buf); break;
      case INFO_REQUEST: CANhandleInfoRequest(&buf); break;
      case REQUEST_REMOVE: CANhandleRemoveRequest(&buf); break;
      case WANT_INFO: handleDisplayRequest(); break;
      case END: return -1; break;
      default: break;
      }
  }
  return 1;
}

void CANhandleAddNeighbor(node* n)
{
  pushNodeToListNode(&(n->neighbors[findInsertDirection(&(n->coord), &myNode)]),
		     n->id);
  
  /* @todo pas besoin d'ACK ...
  int buf = 0;
  MPI_Ssend(&buf, 1, MPI_INT, n->id, ADD_NEIGHBOR_ACK, MPI_COMM_WORLD);
  */
}

void CANhandleRmvNeighbor(node* n)
{
  popNodeFromListNodeById(&(n->neighbors[findInsertDirection(&(n->coord), &myNode)])
			  , n->id);

  /* @todo pas besoin d'ACK ...
  int buf = 0;
  MPI_Send(&buf, 1, MPI_INT, n->id, RMV_NEIGHBOR_ACK, MPI_COMM_WORLD);
  */
}

void CANhandleInfoRequest(node* n)
{
  MPI_Send(&myNode, sizeof(node), MPI_BYTE, n->id, INFO_REQUEST_ACK,
	   MPI_COMM_WORLD);
}

void CANhandleRemoveRequest(node* n)
{
  /* d'abord je cherche la direction */
  int dir = findNodesDirection(n);

  /* je me propage dans cette direction */
  propagateInNodesSpace(dir, n);

  /* MAJ des voisins */
  updateNeighborsAfterRemove(dir, n);

  /* notification de suppression */
  MPI_Send(&myNode, sizeof(node), MPI_BYTE, n->id, REQUEST_REMOVE,
     MPI_COMM_WORLD);
  
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

int findNodesDirection(node* n)
{
  if(isNodeNeighbor(NORTH, &myNode, n))
    return NORTH;
  if(isNodeNeighbor(SOUTH, &myNode, n))
    return SOUTH;
  if(isNodeNeighbor(EAST, &myNode, n))
    return EAST;
  if(isNodeNeighbor(WEST, &myNode, n))
    return WEST;
  return -1;
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
  int i;
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
      /* @todo pas besoin d'ACK ...
      MPI_Recv(&tmp, 1, MPI_INT, buf.id, RMV_NEIGHBOR_ACK,
	       MPI_COMM_WORLD, &status);
      */
    }
    if( isNodeNeighbor(dir, n, &buf) ){
      pushNodeToListNode(&(n->neighbors[dir]), buf.id);
      MPI_Send(n, sizeof(node), MPI_BYTE, buf.id, ADD_NEIGHBOR,
	       MPI_COMM_WORLD);
      /* @todo pas besoin d'ACK ...
      MPI_Recv(&tmp, 1, MPI_INT, buf.id, ADD_NEIGHBOR_ACK,
	       MPI_COMM_WORLD, &status);
      */
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

void propagateInNodesSpace(int dir, node* n)
{
  if( dir == NORTH ){
    myNode.area.north_east.y = n->area.north_east.y;
  }
  else if( dir == SOUTH ){
    myNode.area.south_west.y = n->area.south_west.y;
  }
  else if( dir == WEST ){
    myNode.area.south_west.x = n->area.south_west.x;
  }
  else if( dir == EAST ){
    myNode.area.north_east.x = n->area.north_east.x;
  }
  return;
}

void updateNeighborsAfterRemove(int dir, node* n)
{
  /* pour la direction dir je refais ma liste */
  myNode.neighbors[dir].size = 0;
  recalculateNeighborsAfterRemove(dir, n);

  /* pour l'opposée je ne fais rien*/

  /* et pour les deux autres je rajoute les news à ma liste */
  recalculateNeighborsAfterRemove((dir+1)%NB_DIRECTIONS, n);
  recalculateNeighborsAfterRemove((dir+3)%NB_DIRECTIONS, n);
}

void recalculateNeighborsAfterRemove(int dir, node* n)
{
  int i;
  node buf;
  MPI_Status status;
  for(i=0; i<n->neighbors[dir].size; i++){
    MPI_Send(&myNode, sizeof(node), MPI_BYTE, n->neighbors[dir].idList[i],
       INFO_REQUEST, MPI_COMM_WORLD);
    MPI_Recv(&buf, sizeof(node), MPI_BYTE, n->neighbors[dir].idList[i],
       INFO_REQUEST_ACK, MPI_COMM_WORLD, &status);
    if( isNodeNeighbor(dir, &myNode, &buf) ){
      addNodeToListNode(&(myNode.neighbors[dir]), buf.id);
      MPI_Send(&myNode, sizeof(node), MPI_BYTE, buf.id, ADD_NEIGHBOR,
         MPI_COMM_WORLD);
    }
    MPI_Send(n, sizeof(node), MPI_BYTE, buf.id, RMV_NEIGHBOR,
       MPI_COMM_WORLD);
}
}
