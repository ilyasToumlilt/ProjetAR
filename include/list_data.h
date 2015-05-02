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
#ifndef __LIST_DATA_H__
#define __LIST_DATA_H__

#include <CAN.h>
#include <data.h>

typedef struct _list_data list_data;
struct _list_data {
  data d;
  list_data* next;
};

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
list_data newListData();

/*******************************************************************************
 * Destructors
 ******************************************************************************/
/**
 * Destructeur d'une liste de données
 * Free l'element pointé et tous ses successeurs
 *
 * @param ld pointeur sur la list_data à detruire.
 */
void freeListData(list_data* ld);

/*******************************************************************************
 * Operations
 ******************************************************************************/
/**
 * Empile ( au début ) un élement dans la liste
 *
 * @param ld adresse de la liste 
 * @param d la donnée à empiler
 */
list_data pushDataToListData(data d, list_data listData);

#endif
