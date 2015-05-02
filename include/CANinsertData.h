#ifndef __CANinsertData_H_
#define __CANinsertData_H_

#include <CAN.h>
#include <data.h>

#define NB_DATA 	5
#define NB_STOCKAGE 5

/*******************************************************************************
 * Operations
 ******************************************************************************/
void CANhandleInsertDataRequest(data* d);
void CANinsertDataInit();
data* getLastInsert();
data* getFirstInsert();

#endif
