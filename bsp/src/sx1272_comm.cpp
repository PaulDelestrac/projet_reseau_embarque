#include <sx1272_comm.h>

// Declaration globals variables
int cpok = 0;
int cpall = 0;

char expAddress = '0';
char destAddress = '0';
char messageSize = '0';
char messageCS = '0';
char message[255];

// State Variables
int8_t e;
bool configOK = true;

// Setup function
void SX1272_Setup()
{

	// Power ON the module
	e = sx1272.ON(MOD_TYPE);
	if (e == 0)
	{
		configOK = true;
	}
	else
	{
		configOK = false;
	}

	// Select frequency channel
	e = sx1272.setChannel(FREQ_CENTER);
	if (e == 0)
	{
		configOK = true;
	}
	else
	{
		configOK = false;
	}

	// Select output power
	e = sx1272.setPower(OUT_POWER);
	if (e == 0)
	{
		configOK = true;
	}
	else
	{
		configOK = false;
	}

	if (configOK == true)
	{
		// Config mode LORA
		if (MOD_TYPE == 0)
		{
			// Set header
			e = sx1272.setHeaderON();

			// Set transmission mode
			e = sx1272.setBW(BANDWIDTH); 			// BW = 125 KHz
			e = sx1272.setCR(CODING_RATE); 			// CR = 4/5
			e = sx1272.setSF(SPREADING_FACTOR); 	// SF = 12

			// Set CRC
			e = sx1272.setCRC_ON();

			// Set the node address
			e = sx1272.setNodeAddress(RX_ADDR);

			// Set the length of preamble
			e = sx1272.setPreambleLength(LONG_PREAMB);

			// Set the number of transmission retries
			sx1272._maxRetries = MAX_RETRY;
		}
		// Config mode FSK
		else
		{
			// Set CRC
			e = sx1272.setCRC_ON();

			// Set the node address
			e = sx1272.setNodeAddress(RX_ADDR);

			// Set the length of preamble
			e = sx1272.setPreambleLength(LONG_PREAMB);

			// Set the number of transmission retries
			sx1272._maxRetries = MAX_RETRY;

			//my_printf("REG_SYNC_CONFIG: ");
			sx1272.readRegister(REG_SYNC_CONFIG);
			sx1272.readRegister(REG_SYNC_VALUE1);
			sx1272.readRegister(REG_SYNC_VALUE2);
			sx1272.readRegister(REG_SYNC_VALUE3);
			sx1272.readRegister(REG_SYNC_VALUE4);

			sx1272.writeRegister(REG_SYNC_VALUE1, 0x05);
			sx1272.writeRegister(REG_SYNC_VALUE2, 0x05);
			sx1272.writeRegister(REG_SYNC_VALUE3, 0x05);
			sx1272.writeRegister(REG_SYNC_VALUE4, 0x05);

			//Set the frequency deviation an bit rate parameters
			sx1272.writeRegister(REG_FDEV_MSB, (F_DEV >> 8) & 0x00FF);	   // FDA = Fstep*FDEV = 61Hz*F_DEV : ex: 0x7FF*61 = 125kHz ex2: 0X52*61=5kHz
			sx1272.writeRegister(REG_FDEV_LSB, F_DEV & 0x00FF);			   //...
			sx1272.writeRegister(REG_BITRATE_MSB, (BITRATE >> 8) & 0x00FF); //FXOSC=32Mz, BR = FXOSC/(Bitrate + BitrateFrac/16), ex: FXOSC/0x682B = 1200 bauds, ex2: FXOSC/0x200=62.5 kbauds
			sx1272.writeRegister(REG_BITRATE_LSB, BITRATE & 0x00FF);		   //...
		}
	}
	else
	{
		my_printf("Config not OK!\n");
	}
}

char RXSync()
{
	char stateRX = '0';
	uint8_t lgMessage = 0;
	sx1272.packet_received.data[0] = '\0';

	// Receive packets continuously
	if (configOK == true)
	{
		e = sx1272.receivePacketTimeout(WAIT_RX_MAX);
		cpall = cpall + 1; // total number packet +1

		// Received packet
		if (e == 0)
		{
			stateRX = '0';
			if (sx1272._reception == CORRECT_PACKET)
			{
				// Check if the received packet is correct
				// The length and the content of the packet is checked
				// if it is valid, the cpok counter is incremented
				lgMessage = MAX_LENGTH_MSG;
				if (sx1272.packet_received.length >= lgMessage) // check the length
				{
					cpok = cpok + 1;
					stateRX = '0';
				}
			}
			else
			{
				stateRX = '1';
			}
		}
		// RX Timeout !! No packet received
		else
		{
			stateRX = '2';
		}

		// Plot receive packets in the serial monitor
		for (uint8_t i = 0; i < sx1272.packet_received.length - OFFSET_PAYLOADLENGTH; i++)
		{
			my_printf("%c", (char)sx1272.packet_received.data[i]);
		}

		// Plot RSSI
		// LORA mode
		if (MOD_TYPE == 0)
		{
			e = sx1272.getRSSIpacket();
		}
		// FSK mode
		else
		{
			e = sx1272.getRSSI(); // done during RX, no packet RSSI available in FSK mode;
		}

		// Verify RSSI return
		if (e == 0)
		{
			stateRX = '0';
		}
		else
		{
			stateRX = '3';
		}
	}

	while (!estMessInit(sx1272.packet_received.data));
	return stateRX;
}

bool estMessInit(uint8_t* recu)
{
	return (char) recu[0] == '@';
}

// Main loop function
/*
*  	stateRX = '0' => Good packet received
*	stateRX = '1' => Incorrect packet
*	stateRX = '2' => Receive command not executed
*	stateRX = '3' => RSSI command not executed
*/
char RX(uint32_t waitPeriod = 1000)
{
	char stateRX = '0';
	uint8_t lgMessage = 0;

	// Receive packets continuously
	if (configOK == true)
	{
		e = sx1272.receivePacketTimeout(WAIT_RX_MAX);
		cpall = cpall + 1; // total number packet +1

		// Received packet
		if (e == 0)
		{
			stateRX = '0';
			if (sx1272._reception == CORRECT_PACKET)
			{
				// Check if the received packet is correct
				// The length and the content of the packet is checked
				// if it is valid, the cpok counter is incremented
				lgMessage = MAX_LENGTH_MSG;
				if (sx1272.packet_received.length >= lgMessage) // check the length
				{
					cpok = cpok + 1;
					stateRX = '0';
				}
			}
			else
			{
				stateRX = '1';
			}
		}
		// RX Timeout !! No packet received
		else
		{
			stateRX = '2';
		}

		// Plot receive packets in the serial monitor
		for (uint8_t i = 0; i < sx1272.packet_received.length - OFFSET_PAYLOADLENGTH; i++)
		{
			my_printf("%c", (char)sx1272.packet_received.data[i]);
		}

		// Plot RSSI
		// LORA mode
		if (MOD_TYPE == 0)
		{
			e = sx1272.getRSSIpacket();
		}
		// FSK mode
		else
		{
			e = sx1272.getRSSI(); // done during RX, no packet RSSI available in FSK mode;
		}

		// Verify RSSI return
		if (e == 0)
		{
			stateRX = '0';
		}
		else
		{
			stateRX = '3';
		}
	}

	delay_ms(waitPeriod);
	return stateRX;
}

void TX(char* message, uint8_t destAddress, uint32_t waitPeriod)
{
	// Transmit a packet continuously with a pause of "waitPeriod"
	if (configOK == true)
	{
		e = sx1272.sendPacketTimeout(destAddress, message, waitPeriod);
		delay_ms(waitPeriod); //delay to send packet every PeriodTransmission
	}
}

void sendPacket(char* packet, char expIndex, char destIndex, char* msgContent)
{
	 uint8_t lengthMsg = 0;
	 uint8_t checksum = 0;
	 lengthMsg = strlen(msgContent);

	 packet[0] = '*';
	 packet[1] = expIndex;
	 packet[2] = destIndex;
	 packet[3] = lengthMsg;
	 for(uint8_t i = 0 ; i < lengthMsg ; i++)
	 {
		 packet[4+i] = msgContent[i];
	 }
	 packet[4 + lengthMsg] = '&';
	 for(uint8_t i = 1 ; i < 3 + lengthMsg ; i++)
	 {
		 checksum += packet[i];
	 }
	 checksum = checksum & 0xFF;
	 packet[5 + lengthMsg] = checksum;
	 packet[6 + lengthMsg] = '\r';
	 packet[7 + lengthMsg] = '\n';
	 TX(packet, ADDR_RX_NODE, 1000);
}

// Main loop function
/*
*  	'0' => CORRECT
*	'1' => SOF false
*	'2' => EOF false
*	'3' => Not my address
*	'4' => CS false
*/
char receivePacket(uint32_t waitPeriod = 1000)
{
	char e = '0';
	uint8_t checksum;

	// Receive
	e = RX(waitPeriod);

	// Verify reception
	if(e == '0')
	{
		// Verify SOF
		if((char)sx1272.packet_received.data[0] == '*')
		{
			// Save EXP address, DEST address & msg Size
			expAddress = (char) sx1272.packet_received.data[1];
			destAddress = (char) sx1272.packet_received.data[2];
			messageSize = (char) sx1272.packet_received.data[3];

			// Verify EOF
			if((char) sx1272.packet_received.data[5 + messageSize] == '&')
			{
				// Verify destAddress is our address
				if(SELF_ADDRESS == destAddress)
				{
					// Calculate CS
					for(uint8_t i = 1 ; i < 3 + messageSize ; i++)
					{
						checksum += sx1272.packet_received.data[i];
					}

					// Verify checksum
					if (checksum == (char) sx1272.packet_received.data[4 + messageSize])
					{
						// Save the message
						for (uint8_t i = 0; i < messageSize; i++)
						{
							message[i] = (char) sx1272.packet_received.data[i+4];
						}

						// Return CORRECT
						return '0';
					}
					else
					{
						// CS false
						return '4';
					}

				}
				else
				{
					// Not my address
					return '3';
				}
			}
			else
			{
				// EOF false
				return '2';
			}
		}
		else
		{
			// SOF false
			return '1';
		}
	}

	return '5';
}
