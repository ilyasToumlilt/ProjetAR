/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des listes de noeuds - Header
 *
 * Une liste de data <list_node> est une liste chaînée dont chaque élement
 * pointe vers une donnée d, et sur son successeur dans la liste.
 * La liste vide est un pointeur sur NULL
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabilot@etu.upmc.fr>
 *
 * @version 1.0
 */

#include <list_data.h>

/*******************************************************************************
 * Constructors
 ******************************************************************************/
/**
 * Constructeur d'un element list_data
 *
 * @param d la donnée contenu dans l'element ( attention aux NULL )
 * @param next la donnée suivant
 * @return liste_data* l'element alloué.
 */
list_data* newListData(data* d, list_data* next)
{
  list_data* ret = (list_data*)malloc(sizeof(list_data));
  ret->d = d;
  ret->next = next;
  return ret;
}

/*******************************************************************************
 * Destructors
 ******************************************************************************/
/**
 * Destructeur d'une liste de données
 * Free l'element pointé et tous ses successeurs
 *
 * @param ld pointeur sur la list_data à detruire.
 */
void freeListData(list_data* ld)
{
  if( ld ){
    freeListNode(ld->next);
    free(ld);
  }

  return;
}

/*******************************************************************************
 * Operations
 ******************************************************************************/
/*
/**
 * Empile ( au début ) un élement dans la liste
 *
 * @param ld adresse de la liste 
 * @param d la donnée à empiler
 */
void pushDataToListData(list_data** ld, data* d)
{
  *(ld) = newListNode(d, *(ld));
  return;
}

/**
 * Produit une copie de la liste de données passée en params
 *
 * @param ld liste de datas à copier
 * @return copie de la liste
 */
list_data* cloneListData(list_data* ld)
{
  if( !ld )
    return NULL;
  return newListData(ld->d, cloneListData(ld->next));
}

/*******************************************************************************
 * Views
 ******************************************************************************/
/**
 * Imprime le contenu de la liste ( valeur des données ) sur STDOUT
 *
 * @param ld la liste de data à imprimer
 */
void printListData(list_data* ld)
{
  if( !ld ){
    printf("->[]\n");
  } else {
    printf("->[%d]", ld->d->val);
    printListNode(ld->next);
  }
  
  return;
}
