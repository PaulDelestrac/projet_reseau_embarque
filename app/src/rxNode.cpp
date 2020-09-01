/*
 * rxNode.c
 *
 *  Created on: 28 août 2020
 *      Author: Alex
 */

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 // LoRa Rx node program
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "sx1272_INSAT.h"

#include "string.h"

#include "main.h"

extern "C"{
	#include "delay.h"
	#include "bsp.h"

}

#include "math.h"




///////////////////////////////////////////////////////////////
// Configuration du module en mode LoRA ou FSK
///////////////////////////////////////////////////////////////



//Main parameters (to be changed)
#define OutPower POW_14 //min -1, max 14
#define TypeModulation 0 //0: LORA, 1:FSK

//LORA configuration (to be changed)
#define BW BW_125 //BW_125, BW_250 or BW_500
#define CR CR_5 //CR_5
#define SF SF_12 //SF_7 to SF_12

//FSK configuration (to be changed)
#define BitRate BR1200bauds //BR1200bauds or BR4800bauds
#define Fdev FDA5kHz//FDA5kHz

//Other common parameters (do not change!!)
#define PreambLong 12//12
#define RX_Addr 8 //address of the Rx node
#define freq_centrale CH_868v3
#define MaxNbRetries 3
#define WaitRxMax 10000 //en ms
#define AddrRxNode BROADCAST_ADDR
///////////////////////////////////////////////////////////////
// Déclaration variables globales
///////////////////////////////////////////////////////////////

char LgMsg = 0;
char Message[] = "Thibault et Lolo les bests\r\n";
uint8_t rx_address = RX_Addr;
int type_modulation=TypeModulation;
uint16_t RegBitRate = BitRate;
uint16_t RegFdev = Fdev;
int cpok=0;
int cpall=0;
// status variables
int8_t e;
bool ConfigOK = true;

///////////////////////////////////////////////////////////////
// Setup function
///////////////////////////////////////////////////////////////
void setup() {

  // Power ON the module
  e = sx1272.ON(type_modulation);
  if (e == 0)
  {
  }
  else
  {
    ConfigOK = false;
  }

  // Select frequency channel
  e = sx1272.setChannel(freq_centrale);
  if (e == 0)
  {
  }
  else
  {
    ConfigOK = false;
  }
  // Select output power
  e = sx1272.setPower(OutPower);
  if (e == 0)
  {
  }
  else
  {
    ConfigOK = false;
  }
  if (ConfigOK == true)
  {
    //////////////////////////////////////////////////////////////////////
    //config supplémentaire mode LORA
    //////////////////////////////////////////////////////////////////////
    if(type_modulation==0)
    {
      // Set header
      e = sx1272.setHeaderON();
      // Set transmission mode
      e = sx1272.setCR(CR);    // CR = 4/5
      e = sx1272.setSF(SF);   // SF = 12
      e = sx1272.setBW(BW);    // BW = 125 KHz
      // Set CRC
      e = sx1272.setCRC_ON();
      // Set the node address
      e = sx1272.setNodeAddress(rx_address);
      // Set the length of preamble
      e = sx1272.setPreambleLength(PreambLong);
      // Set the number of transmission retries
      sx1272._maxRetries = MaxNbRetries;
    }
    //////////////////////////////////////////////////////////////////////
    //config supplémentaire mode FSK
    //////////////////////////////////////////////////////////////////////
    else
    {
      // Set CRC
      e = sx1272.setCRC_ON();
      // Set the node address
      e = sx1272.setNodeAddress(rx_address);
      // Set the length of preamble
      e = sx1272.setPreambleLength(PreambLong);
      // Set the number of transmission retries
      sx1272._maxRetries = MaxNbRetries;

      //my_printf("REG_SYNC_CONFIG: ");
      sx1272.readRegister(REG_SYNC_CONFIG);
      sx1272.readRegister(REG_SYNC_VALUE1);
      sx1272.readRegister(REG_SYNC_VALUE2);
      sx1272.readRegister(REG_SYNC_VALUE3);
      sx1272.readRegister(REG_SYNC_VALUE4);


      sx1272.writeRegister(REG_SYNC_VALUE1,0x05);
      sx1272.writeRegister(REG_SYNC_VALUE2,0x05);
      sx1272.writeRegister(REG_SYNC_VALUE3,0x05);
      sx1272.writeRegister(REG_SYNC_VALUE4,0x05);


      //Set the frequency deviation an bit rate parameters
      sx1272.writeRegister(REG_FDEV_MSB,(RegFdev>>8)&0x00FF);// FDA = Fstep*FDEV = 61Hz*Fdev : ex: 0x7FF*61 = 125kHz ex2: 0X52*61=5kHz
      sx1272.writeRegister(REG_FDEV_LSB,RegFdev&0x00FF);//...
      sx1272.writeRegister(REG_BITRATE_MSB,(RegBitRate>>8)&0x00FF);//FXOSC=32Mz, BR = FXOSC/(Bitrate + BitrateFrac/16), ex: FXOSC/0x682B = 1200 bauds, ex2: FXOSC/0x200=62.5 kbauds
      sx1272.writeRegister(REG_BITRATE_LSB,RegBitRate&0x00FF);//...

    }
  }
  else
  {
  }

  if (ConfigOK == true) {
    //affichage entête
    //statut (correct = 1 or bad = 0 or non received = 2)

  }
}

///////////////////////////////////////////////////////////////
// Main loop function
///////////////////////////////////////////////////////////////
void RX()
{
  char StatusRXMessage;
  uint8_t inByte = 0;

 /* //////////////////////////////////////////////////////////////////////////////////
  // Check the serial port to know if there is an input character to be treated
  if (Serial.available())
  {
    inByte = Serial.read()-'0';
    cpall=0;
    cpok=0;
  }*/

  //////////////////////////////////////////////////////////////////////////////////
  // Receive packets continuously
  if (ConfigOK == true)
  {
    e = sx1272.receivePacketTimeout(WaitRxMax);
    cpall=cpall+1;//increment the total number of packet supposed to be received
    //paquet reçu, correct ou non
    if (e == 0)
    {
      StatusRXMessage = '0';
      if (sx1272._reception == CORRECT_PACKET)
      {
       // Check if the received packet is correct
       // The length and the content of the packet is checked
       // if it is valid, the cpok counter is incremented
       LgMsg=strlen(Message);
       if(sx1272.packet_received.length>=LgMsg)//check the length
       {
        if(memcmp(Message,sx1272.packet_received.data,LgMsg)==0)//check the content
        {
          cpok=cpok+1;
          StatusRXMessage = '1';
        }
       }
      }
    }
    // RX Timeout !! No packet received
    else
    {
      StatusRXMessage = '2';
    }

    //////////////////////////////////////////////////////////////////////////////////
      // Plot receive packets in the serial monitor

    //char messageArray[sx1272.packet_received.length-OFFSET_PAYLOADLENGTH];

      for (uint8_t i =0; i < sx1272.packet_received.length-OFFSET_PAYLOADLENGTH; i++)
      {
        //printchar(char(sx1272.packet_received.data[i]),i);
    	  my_printf("%c",(char)sx1272.packet_received.data[i]);



    	 // messageArray[i] = sx1272.packet_received.data[i];
    	  //my_printf(char (sx1272.packet_received.data[i]));
     //   my_printf(" ");
      }


     // my_sprintf(messageArray );




    //////////////////////////////////////////////////////////////////////////////////
    // Plot receive packets in the serial monitor
    for (uint8_t i =0; i < sx1272.packet_received.length-OFFSET_PAYLOADLENGTH; i++)
    {

    }
    ///////////////////////////////////////////////////////////////////////////////////
    // Plot RSSI
    // LORA mode
    if(TypeModulation == 0)
    {
      e = sx1272.getRSSIpacket();
    }
    // FSK mode
    else
    {
      //e = sx1272.getRSSI() done during RX, no packet RSSI available in FSK mode;
    }
  }
  delay_ms(1000);
}

void TX() {

  uint8_t dest_address = AddrRxNode;
  //uint8_t inByte = 0;
  //uint8_t flagchange=0;

  //////////////////////////////////////////////////////////////////////////////////
  // Check the serial port to know if there is an input character to be treated
  //////////////////////////////////////////////////////////////////////////////////
  // Transmit a packet continuously with a pause of "waitPeriod"
  if (ConfigOK == true)
  {

    LgMsg=strlen(Message);
    e = sx1272.sendPacketTimeout(dest_address,Message,5000);

    /*if(type_modulation)
    {
      sx1272.writeRegister(REG_OP_MODE, FSK_STANDBY_MODE); // FSK standby mode to switch off the RF field
    }

    if (e == 0)
    {
      my_printf(F("\n Packet number "));
      my_printf(cp,DEC);
	    my_printf(F(" ;Rx node address "));
	    my_printf(dest_address,DEC);
      my_printf(F(" ; Output power (dBm)"));
      my_printf(newpower-1,DEC);
      cp++;
    }
    else
    {
      my_printfln("\n Trasmission problem !");
    }*/
  delay_ms(1000); //delay to send packet every PeriodTransmission
  }
}
