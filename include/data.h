*/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des donn�es - Header
 *
 * Une Donn�e contient une valeur, ses coordonn�es,
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabillot@etu.upmc.fr>
 *
 * @version 1.0
 */

#ifndef __DATA_H__
#define __DATA_H__

#include <CAN.h>

struct _data {
  int val;
  point* coord;
};

/*******************************************************************************
 * Contructor
 ******************************************************************************/
node* newData(point* coord);

/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeData(data* d);
/*******************************************************************************
 * Node containing
 ******************************************************************************/
int isDataInCoord(data* d, int x1, int y1, int x2, int y2);
int isDataInSpace(data* d, space* sp);
int isDataInNodesSpace(data* d, node* trg);

#endif
