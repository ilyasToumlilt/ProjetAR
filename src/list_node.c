/**
 * Projet AR - Content-Adressable Network
 * Implémentation des primitives de gestion des listes de noeuds
 * cf. src/CAN.h : struct _list_node
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

#include <CAN.h>

/*******************************************************************************
 * Private Declarations
 ******************************************************************************/
list_node* newListNode(node* n, list_node* next);
void freeListNode(list_node* ln);
void pushNodeToListNode(list_node** ln, node* n);
node* popNodeFromListNode(list_node** ln);
node* popNodeFromListNodeById(list_node** ln, int id);
list_node* cloneListNode(list_node* ln);

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
list_node* newListNode(node* n, list_node* next)
{
  list_node* ret = (list_node*)malloc(sizeof(list_node));
  ret->n = n;
  ret->next = next;
  return ret;
}

/*******************************************************************************
 * Destructors
 ******************************************************************************/
/**
 * Destructeur d'une liste de noeuds
 * Free l'element pointé et tous ses successeurs
 *
 * @param ln pointeur sur la list_node à detruire.
 */
void freeListNode(list_node* ln)
{
  if( ln ){
    freeListNode(ln->next);
    free(ln);
  }

  return;
}

/*******************************************************************************
 * Operations
 ******************************************************************************/
/* @todo opti par rapport à la fréquence de parcours */
/**
 * Empile ( au début ) un élement dans la liste
 *
 * @param ln adresse de la liste 
 * @param n le noeud à empiler
 */
void pushNodeToListNode(list_node** ln, node* n)
{
  *(ln) = newListNode(n, *(ln));
  return;
}

/**
 * Dépile un élement ( le premier ) de la liste
 *
 * @param ln adresse de la liste
 * @return le noeud dépilé, NULL si liste vide
 */
node* popNodeFromListNode(list_node** ln)
{
  node* ret = NULL;
  if( (*ln) ){
    liste_node* first = *(ln);
    *(ln) = first->next;
    first->next = NULL;
    ret = first->n;
    freeListNode(first);
  }
  return ret;
}

/**
 * Dépile l'element d'identidiant id de la liste.
 *
 * @param ln adresse de la liste de noeuds.
 * @param id l'identifiant du noeud à dépiler.
 * @return le noeud dépilé
 */
node* popNodeFromListNodeById(list_node** ln, int id)
{
  node* ret = NULL;
  if( (*ln) ){
    liste_node **tmp = ln;
    while( *(tmp) ){
      if( (*tmp)->n->id == id ){
	ret = popNodeFromListNode(tmp);
	break;
      }
      tmp = &((*tmp)->next);
    }
  }
  return ret;
}

/**
 * Produit une copie de la liste de noeuds passée en params
 *
 * @param ln liste de noeuds à copier
 * @return copie de la liste
 */
list_node* cloneListNode(list_node* ln)
{
  if( !ln )
    return NULL;
  return newListNode(ln->n, cloneListNode(ln->next));
}
