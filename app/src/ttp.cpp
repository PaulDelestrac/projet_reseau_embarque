/*
 * ttp.c
 *
 *  Created on: 1 sept. 2020
 *      Author: Sebastien
 */


extern "C"{
	#include "main.h"
	#include "bsp.h"
}

/*
 * Structure switch case state TTP
 */
typedef enum
{
	INIT,
	SYNCHRO,
	ATT_AV,
	SEND,
	ATT_AP
} state;

state Etat = INIT;
void ttp(void)
{
	int compteur_synchro = 0;

	switch(Etat)
	{
	case(INIT):
		init();
		state = SYNCHRO;
		break;
	case(SYNCHRO):
		synchro();
		if (INDEX == 0)
		{
			state = SEND;
		}
		else
		{
			state = ATT_AV;
		}
		break;
	case(ETAT_AV):
		for (int i=0; i<INDEX; i++)
		{
			RX(PERIOD * 1000);
		}
		state = SEND;
		break;
	case(SEND):
		// set send
		state = ATT_AP;
		break;
	case(ATT_AP):
		for (int i=INDEX+1; i<NBR_NOEUD; i++)
		{
			RX(PERIOD * 1000);
		}
		if (compteur_synchro > 9)
		{
			state = SYNCHRO;
		}
		else
		{
			state = ATT_AV;
		}
		break;
	}
}


