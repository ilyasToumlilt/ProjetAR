#ifndef __CANsearchData_H_
#define __CANsearchData_H_

#include <CAN.h>
#include <data.h>

/* 
 * Lance la recherche sur les NB_STOCKAGE premieres data ajoutés 
 * et les NB_STOCKAGE dernieres
 */
void CANsearchDataInit();

/**
 * Traitement d'une reception de demande de recherche
 */
void CANhandleSearchDataRequest(data* n);

#endif
