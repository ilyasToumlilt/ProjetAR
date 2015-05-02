/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des listes de noeuds - Header
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabilot@etu.upmc.fr>
 *
 * @version 1.0
 */
#ifndef __LIST_NODE_H__
#define __LIST_NODE_H__

#include <CAN.h>

typedef struct _list_node list_node;
struct _list_node {
  int idList[N-1]; /* @todo largement améliorable */
  int size;
};

/*******************************************************************************
 * Constructors
 ******************************************************************************/
/**
 * Constructeur d'un element list_node
 *
 * @param n le noeud contenu dans l'element ( attention aux NULL )
 * @param next le noeud suivant
 * @return liste_node* l'element alloué.
 */
list_node newListNode();

/*******************************************************************************
 * Destructors
 ******************************************************************************/
/**
 * Destructeur d'une liste de noeuds
 * Free l'element pointé et tous ses successeurs
 *
 * @param ln pointeur sur la list_node à detruire.
 */
/*void freeListNode(list_node* ln);*/

/*******************************************************************************
 * Operations
 ******************************************************************************/
/**
 * Empile ( au début ) un élement dans la liste
 *
 * @param ln adresse de la liste 
 * @param n le noeud à empiler
 */
void pushNodeToListNode(list_node* ln, int id);

/**
 * Dépile un élement ( le premier ) de la liste
 *
 * @param ln adresse de la liste
 * @return le noeud dépilé, NULL si liste vide
 */
int popNodeFromListNode(list_node* ln);

/**
 * Dépile l'element d'identidiant id de la liste.
 *
 * @param ln adresse de la liste de noeuds.
 * @param id l'identifiant du noeud à dépiler.
 * @return le noeud dépilé
 */
int popNodeFromListNodeById(list_node* ln, int id);

/*******************************************************************************
 * Views
 ******************************************************************************/
void printListNode(list_node ln);

#endif
