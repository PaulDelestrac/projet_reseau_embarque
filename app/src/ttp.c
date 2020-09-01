/*
 * ttp.c
 *
 *  Created on: 1 sept. 2020
 *      Author: Sebastien
 */

#include "bsp.h"

#define INDEX 0
#define NBR_NOEUD 4
#define PERIOD 2

/*
 * Structure switch case state TTP
 */
typedef enum
{
	IDLE,
	RUN,
	END
}state;

state Etat = IDLE;
void ttp(void)
{
	switch(Etat)
	{
	case(IDLE):

	break;

	}
}


