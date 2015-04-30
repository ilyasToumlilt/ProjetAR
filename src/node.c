/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des noeuds - Implémentation
 *
 * Un Noeud contient un identifiant unique, ses coordonnées,
 * l'espace dont il s'occupe, et la liste de ses voisins
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabilot@etu.upmc.fr>
 *
 * @version 1.0
 */

#include <node.h>

/*******************************************************************************
 * Contructor
 ******************************************************************************/
node* newNode(int id, point* coord, space* area)
{
  node* ret = (node*)malloc(sizeof(node));

  ret->id = id;
  ret->coord = coord;
  ret->area = area;
  int i;
  for(i=0; i<NB_DIRECTIONS;i++)
    ret->neighbors[i] = newListNode();
  //ret->datas = newListData(NULL, NULL);
  return ret;
}

node* newNodeWithRandomPoint(int id)
{
  return newNode(id, newRandomPoint(), NULL);
}

/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeNode(node* n)
{
  freePoint(n->coord);
  freeSpace(n->area);
  int i;
  for(i=0; i<NB_DIRECTIONS; i++)
    freeListNode(n->neighbors[i]);
  //freeListData(n->datas);
  free(n);

  return;
}

/*******************************************************************************
 * Node containing
 ******************************************************************************/
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
