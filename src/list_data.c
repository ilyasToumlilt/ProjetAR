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
list_data newListData(data d)
{
  list_data ret;
  ret.d = d;
  ret.next = NULL;
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
    freeListData(ld->next);
    free(ld);
  }

  return;
}

/*******************************************************************************
 * Operations
 ******************************************************************************/
 
/**
 * Empile ( au début ) un élement dans la liste
 *
 * @param ld adresse de la liste 
 * @param d la donnée à empiler
 */
list_data pushDataToListData(data d, list_data listData)
{
  list_data ret = newListData(d);
  ret.next = &listData;
  return ret;
}
