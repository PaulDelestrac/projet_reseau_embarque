/*
 * ttp.cpp
 *
 *  Created on: 1 sept. 2020
 *      Author: Sebastien
 */

#include "ttp.h"
extern "C"{
	#include "main.h"
	#include "bsp.h"
}

/*
 * Structure switch case state TTP
 */
typedef enum
{
	SYNCHRO,
	ATT_AV,
	SEND,
	ATT_AP
} state;

state Etat = SYNCHRO;
void ttp(char* packet, char* msgContent)
{
	int compteur_synchro = 0;

	switch(Etat)
	{
	case(SYNCHRO):
		my_printf("SYNCHRO\r\n");
		synchro();
		if (INDEX == 0)
		{
			Etat = SEND;
		}
		else
		{
			Etat = ATT_AV;
		}
		break;
	case(ATT_AV):
		for (int i=0; i<INDEX; i++)
		{
			my_printf("ATT_AV %d\r\n", i);
			receivePacket(PERIOD * 1000);
		}
		Etat = SEND;
		break;
	case(SEND):
		my_printf("ENVOI %d\r\n", INDEX);
		sendPacket(packet, INDEX, BROADCAST_ADDRESS, msgContent);
		Etat = ATT_AP;
		break;
	case(ATT_AP):
		for (int i=INDEX+1; i<NBR_NOEUD; i++)
		{
			my_printf("ATT_AP %d\r\n", i);
			receivePacket(PERIOD * 1000);
		}
		if (compteur_synchro > 9)
		{
			Etat = SYNCHRO;
		}
		else
		{
			Etat = ATT_AV;
		}
		break;
	}
}


