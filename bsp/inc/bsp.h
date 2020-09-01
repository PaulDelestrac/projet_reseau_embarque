#ifdef __cplusplus
extern "C"
{
#endif

#ifndef BSP_INC_BSP_H_
#define BSP_INC_BSP_H_

#include "stm32f0xx.h"

    // LED driver functions
    void BSP_LED_Init(void);
    void BSP_LED_On(void);
    void BSP_LED_Off(void);
    void BSP_LED_Toggle(void);

    // Push-Button driver functions
    void BSP_PB_Init(void);
    uint8_t BSP_PB_GetState(void);

    // Debug Console driver functions
    void BSP_Console_Init(void);
    void BSP_SPI1_Init(void);
    uint8_t BSP_SPI_SendReceive(uint8_t tx_byte);
    void BSP_Write(uint8_t register_address, uint8_t data);
    void BSP_Read(uint8_t register_address, uint8_t *buffer, uint8_t nbytes);

#endif /* BSP_INC_BSP_H_ */

#ifdef __cplusplus
}
#endif
