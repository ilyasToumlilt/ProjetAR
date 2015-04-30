/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des point - Header
 *
 * Un point se compose de deux coordonnées x et y.
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabillot@etu.upmc.fr>
 *
 * @version 1.0
 */

#ifndef __POINT_H__
#define __POINT_H__

#include <CAN.h>

struct _point {
  int x;
  int y;
};

/*******************************************************************************
 * Constructor
 ******************************************************************************/
/**
 * Constructeur
 *
 * @param x coordonnée horizontale
 * @param y coordonnée verticale
 * @return le point alloué 
 */
point* newPoint(int x, int y);
/**
 * Constructeur d'un point avec des coordonnées aléatoires
 *
 * @return le point alloué
 */
point* newRandomPoint();
/*******************************************************************************
 * Destructor
 ******************************************************************************/
/**
 * Destructeur
 *
 * @param p le point à désallouer
 */
void freePoint(point* p);

#endif
