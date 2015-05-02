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

/**
 * getter pour que le binome puisse travailler séparemment sur un autre fichier 
 *
 * @return node le node du process courant
 */
node getNode()
{
  return myNode;
}
void setNode(node n){ 
  myNode = n;
}

/*******************************************************************************
 * Private Declarations
 ******************************************************************************/
/**
 * Opérations d'insertion concernant l'initiateur
 * insère d'abord le BOOTSTRAP_NODE
 * puis tous les autres
 */
void CANinsertInit();
/**
 * Fonction d'insertion du BOOTSTRAP_NODE
 * Comme ça sera toujours le premier à s'insérer
 * il prendra par défaut tout l'espace
 */
void CANinsertBootstrap();
/**
 * Fonction d'insertion pour tous les autres noeuds 
 * Le noeud attend d'abord l'autorisation de l'initiateur
 * puis envoi une demande d'insertion au bootstrap
 * cette demande sera relayée jusqu'à ce qu'on trouve le noeud 
 * contenant ses coordonnées ( générée aléatoirement )
 */
void CANinsertNode();
/**
 * Fonction qui s'occupe de traiter le message d'insertion
 * Si le noeud à insérer ne setrouve pas dans mon espace,
 * je cherche une direction où je pourrais l'envoyer
 * puis je transmet la requete à mon premier voisin de cette direction
 * sinon je divise mon espace et je réaffecte mes ( et ses ) voisins 
 *
 * @param n le noeud à insérer 
 */
void CANhandleInsertRequest(node* n);
/**
 * Traitement d'une requete d'ajout de voisin
 *
 * @param n le noeud à rajouter
 */
void CANhandleAddNeighbor(node* n);
/**
 * Traitement d'une requete de suppression de voisin
 *
 * @param n le noeud à supprimer
 */
void CANhandleRmvNeighbor(node* n);
/**
 * Traitement d'une requete de demande d'informations
 *
 * @param n le noeud demandeur
 */
void CANhandleInfoRequest(node* n);
/**
 * Traitement d'une requete de suppression de noeud
 *
 * Le noeud est surement mon voisin, je propage sur son espace
 * et je récupère ses voisins.
 *
 * @param n le noeud à supprimer
 */
void CANhandleRemoveRequest(node* n);

/**
 * Le noeud passé en param est-il mon voisin ? 
 * Si oui dans quelle direction ?
 *
 * @param n le noeud à repérer
 * @return la direction ou -1 si ce n'est pas mon voisin
 */
int  findNodesDirection(node* n);

/**
 * Au cours de l'insertion j'ai besoin de remettre à jour les voisins 
 * 
 * @param dir la direction d'insertion
 * @param n le noeud à insérer
 */
void updateNeighbors(int dir, node* n);

/**
 * Au cours de l'insertion, pour les directions adjacentes 
 * on a besoin de recalculer toute la liste des voisins ...
 *
 * @param dir direction de recalcul 
 * @param n le noeud à insérer
 */
void recalculateNeighborsForDirection(int dir, node* n);

/**
 * is trg src's neighbor in dir direction ? 
*/
int  isNodeNeighbor(int dir, node* src, node* trg);

/**
 * au cours de la suppression j'ai besoin de me propager sur 
 * l'espace du noeud à supprimer
 *
 * @param dir direction de propagation 
 * @param n le noeud à supprimer
 */
void propagateInNodesSpace(int dir, node* n);

/**
 * Opérations de mise à jour des voisins 
 * à la suppression
 *
 * @param dir direction de suppression
 * @param n le noeud à supprimer
 */
void updateNeighborsAfterRemove(int dir, node* n);

/**
 * Recalcule les voisins à la suppression
 * pour une direction précise
 *
 * @param dir direction de recalcul
 * @param n le noeud à supprimer
 */
void recalculateNeighborsAfterRemove(int dir, node* n);

/*******************************************************************************
 * Initialization
 ******************************************************************************/
int nbProcess;
int idProcess;
/**
 * Opérations d'Initialization
 * Ne fait pas grand chose à part récupérer l'id et 
 * le nombre de process 
 */
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
/**
 * Opération d'insertion, appelle la bonne fonction selon le type de
 * process ( INIT / BOOTSTRAP / OTHERS )
 */
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

/**
 * Opérations d'insertion concernant l'initiateur
 * insère d'abord le BOOTSTRAP_NODE
 * puis tous les autres
 */
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

/**
 * Fonction d'insertion du BOOTSTRAP_NODE
 * Comme ça sera toujours le premier à s'insérer
 * il prendra par défaut tout l'espace
 */
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

/**
 * Fonction d'insertion pour tous les autres noeuds 
 * Le noeud attend d'abord l'autorisation de l'initiateur
 * puis envoi une demande d'insertion au bootstrap
 * cette demande sera relayée jusqu'à ce qu'on trouve le noeud 
 * contenant ses coordonnées ( générée aléatoirement )
 */
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

/**
 * Fonction qui s'occupe de traiter le message d'insertion
 * Si le noeud à insérer ne setrouve pas dans mon espace,
 * je cherche une direction où je pourrais l'envoyer
 * puis je transmet la requete à mon premier voisin de cette direction
 * sinon je divise mon espace et je réaffecte mes ( et ses ) voisins 
 *
 * @param n le noeud à insérer 
 */
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
/**
 * Opérations de suppression d'un noeud.
 *
 * Pour économiser le nombre de messages, j'essaye d'abord d'attribuer mon
 * espace au voisinnage avec une unique voisin
 * Sinon au voisinnage avec le nombre min de voisins.
 * Je ne traite pas le cas où je n'ai pas de voisins car on considère ici
 * qu'on ne supprime pas le BOOTSTRAP_NODE
 * ( ou pas encore ).
 *
 * Le noeud à supprimer et bien sur le noeud courant.
 */
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
/**
 * Fonction de traitement des messages reçus
 *
 * Les messages sont classés par TAGs ( consulter CAN.h )
 * et selon le type de TAG la fonction de traitement correspondante sera
 * appelée
 */
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

/**
 * Traitement d'une requete d'ajout de voisin
 *
 * @param n le noeud à rajouter
 */
void CANhandleAddNeighbor(node* n)
{
  pushNodeToListNode(&(n->neighbors[findInsertDirection(&(n->coord), &myNode)]),
		     n->id);
  
  /* @todo pas besoin d'ACK ...
  int buf = 0;
  MPI_Ssend(&buf, 1, MPI_INT, n->id, ADD_NEIGHBOR_ACK, MPI_COMM_WORLD);
  */
}

/**
 * Traitement d'une requete de suppression de voisin
 *
 * @param n le noeud voisin à supprimer
 */
void CANhandleRmvNeighbor(node* n)
{
  popNodeFromListNodeById(&(n->neighbors[findInsertDirection(&(n->coord), &myNode)])
			  , n->id);

  /* @todo pas besoin d'ACK ...
  int buf = 0;
  MPI_Send(&buf, 1, MPI_INT, n->id, RMV_NEIGHBOR_ACK, MPI_COMM_WORLD);
  */
}

/**
 * Traitement d'une requete de demande d'informations
 *
 * @param n le noeud demandeur
 */
void CANhandleInfoRequest(node* n)
{
  MPI_Send(&myNode, sizeof(node), MPI_BYTE, n->id, INFO_REQUEST_ACK,
	   MPI_COMM_WORLD);
}

/**
 * Traitement d'une requete de suppression de noeud
 *
 * Le noeud est surement mon voisin, je propage sur son espace
 * et je récupère ses voisins.
 *
 * @param n le noeud à supprimer
 */
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
/*******************************************************************************
 * Other Operations 
 ******************************************************************************/
/**
 * à l'insertion j'ai besoin de savoir où se trouve un point par
 * rapport à mon espace, pour rediriger l'insertion ...
 *
 * @param p le point recherché
 * @param trg la cible
 * @return la direction où se trouve p par rapport à trg
 */
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

/**
 * au cours de la redirection des fois une point se trouve dans une 
 * direction "diagonale" par rapport à mon espace, dans ce cas là j'ai
 * besoin de choisir aléatoirement entre les deux
 * par exemple NORTHWEST c'est alétoirement ou NORTH ou WEST
 *
 * @param direction la direction composée
 * @return la direction choisie
 */
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

/**
 * Le noeud passé en param est-il mon voisin ? 
 * Si oui dans quelle direction ?
 *
 * @param n le noeud à repérer
 * @return la direction ou -1 si ce n'est pas mon voisin
 */
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

/**
 * Au cours de l'insertion j'ai besoin de remettre à jour les voisins 
 * 
 * @param dir la direction d'insertion
 * @param n le noeud à insérer
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

/**
 * Au cours de l'insertion, pour les directions adjacentes 
 * on a besoin de recalculer toute la liste des voisins ...
 *
 * @param dir direction de recalcul 
 * @param n le noeud à insérer
 */
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

/**
 * is trg src's neighbor in dir direction ? 
*/
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

/**
 * au cours de la suppression j'ai besoin de me propager sur 
 * l'espace du noeud à supprimer
 *
 * @param dir direction de propagation 
 * @param n le noeud à supprimer
 */
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

/**
 * Opérations de mise à jour des voisins 
 * à la suppression
 *
 * @param dir direction de suppression
 * @param n le noeud à supprimer
 */
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

/**
 * Recalcule les voisins à la suppression
 * pour une direction précise
 *
 * @param dir direction de recalcul
 * @param n le noeud à supprimer
 */
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
