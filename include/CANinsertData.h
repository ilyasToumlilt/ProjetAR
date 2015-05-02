#ifndef __CANinsertData_H_
#define __CANinsertData_H_

#include <CAN.h>
#include <data.h>

#define NB_DATA 	10
#define NB_STOCKAGE 5

/*******************************************************************************
 * Operations
 ******************************************************************************/
/* 
 * Tente d'inserer NB_DATA données dans le CAN
 */
void CANhandleInsertDataRequest(data* d);

/*
 * Traitement d'une reception d'insertion de donnée
 */
void CANinsertDataInit();

data* getLastInsert();
data* getFirstInsert();

#endif
