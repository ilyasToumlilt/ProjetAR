/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des listes de noeuds - Header
 *
 * Une liste de noeuds <list_node> est une liste chaînée dont chaque élement
 * pointe versun noeud <node> n, et sur son successeur dans la liste.
 * La liste vide est un pointeur sur NULL
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
  int id;
  list_node* next;
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
list_node* newListNode(int id, list_node* next);

/*******************************************************************************
 * Destructors
 ******************************************************************************/
/**
 * Destructeur d'une liste de noeuds
 * Free l'element pointé et tous ses successeurs
 *
 * @param ln pointeur sur la list_node à detruire.
 */
void freeListNode(list_node* ln);

/*******************************************************************************
 * Operations
 ******************************************************************************/
/**
 * Empile ( au début ) un élement dans la liste
 *
 * @param ln adresse de la liste 
 * @param n le noeud à empiler
 */
void pushNodeToListNode(list_node** ln, int id);

/**
 * Dépile un élement ( le premier ) de la liste
 *
 * @param ln adresse de la liste
 * @return le noeud dépilé, NULL si liste vide
 */
int popNodeFromListNode(list_node** ln);

/**
 * Dépile l'element d'identidiant id de la liste.
 *
 * @param ln adresse de la liste de noeuds.
 * @param id l'identifiant du noeud à dépiler.
 * @return le noeud dépilé
 */
int popNodeFromListNodeById(list_node** ln, int id);

/**
 * Produit une copie de la liste de noeuds passée en params
 *
 * @param ln liste de noeuds à copier
 * @return copie de la liste
 */
list_node* cloneListNode(list_node* ln);

/*******************************************************************************
 * Views
 ******************************************************************************/
/**
 * Imprime le contenu de la liste ( identifiants des noeuds ) sur STDOUT
 *
 * @param ln la liste de noeuds à imprimer
 */
void printListNode(list_node* ln);

#endif
