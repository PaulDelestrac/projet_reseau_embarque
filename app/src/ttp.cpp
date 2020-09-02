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
	INIT,
	SYNCHRO,
	ATT_AV,
	SEND,
	ATT_AP
} state;

state Etat = INIT;
void ttp(char* packet, char* msgContent)
{
	int compteur_synchro = 0;

	switch(Etat)
	{
	case(INIT):
		Etat = SYNCHRO;
		break;
	case(SYNCHRO):
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
			receivePacket(PERIOD * 1000);
		}
		Etat = SEND;
		break;
	case(SEND):
		sendPacket(packet, INDEX, BROADCAST_ADDRESS, msgContent);
		Etat = ATT_AP;
		break;
	case(ATT_AP):
		for (int i=INDEX+1; i<NBR_NOEUD; i++)
		{
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


