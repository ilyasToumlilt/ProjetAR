/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des noeuds - Header
 *
 * Un Noeud contient un identifiant unique, ses coordonnées,
 * l'espace dont il s'occupe, et la liste de ses voisins
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabillot@etu.upmc.fr>
 *
 * @version 1.0
 */

#ifndef __NODE_H__
#define __NODE_H__

#include <CAN.h>

struct _node {
  int id;
  point* coord;
  space* area;
  list_node* neighbors[NB_DIRECTIONS];
  //list_data* datas;
};

/*******************************************************************************
 * Contructor
 ******************************************************************************/
node* newNode(int id, point* coord, space* area);
node* newNodeWithRandomPoint(int id);
/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeNode(node* n);
/*******************************************************************************
 * Node containing
 ******************************************************************************/
int isNodeInCoord(node* src, int x1, int y1, int x2, int y2);
int isNodeInSpace(node* src, space* sp);
int isNodeInNodesSpace(node* src, node* trg);

#endif
