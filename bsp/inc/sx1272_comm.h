#ifndef APP_INC_RXNODE_H_
#define APP_INC_RXNODE_H_

#include "main.h"
#include "sx1272_INSAT.h"

void SX1272_Setup();
char RX(uint32_t waitPeriod);
void TX(uint8_t destAddress, uint32_t waitPeriod);

// Configuration LoRA or FSK

// Main parameters
#define OUT_POWER POW_14	 // min -1, max 14
#define MOD_TYPE 0           // 0: LORA, 1:FSK
#define MAX_LENGTH_MSG 32

// LORA configuration
#define BANDWIDTH BW_125 // BW_125, BW_250 or BW_500
#define CODING_RATE CR_5	  // CR_5
#define SPREADING_FACTOR SF_12  // SF_7 to SF_12

// FSK configuration
#define BITRATE BR1200bauds // BR1200bauds or BR4800bauds
#define F_DEV FDA5kHz		// FDA5kHz

// Other common parameters
#define LONG_PREAMB 12 //12
#define RX_ADDR 8	  //address of the Rx node
#define FREQ_CENTER CH_868v3
#define MAX_RETRY 3
#define WAIT_RX_MAX 10000 //en ms
#define ADDR_RX_NODE BROADCAST_ADDR

#endif /* APP_INC_RXNODE_H_ */
