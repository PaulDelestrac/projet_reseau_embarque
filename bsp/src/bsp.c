#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp.h"

	/*
 * BSP_LED_Init()
 * Initialize LED pin (PA5) as  High-Speed Push-Pull Output
 * Set LED initial state to OFF
 */

	void BSP_LED_Init()
	{
		// Enable GPIOA clock
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

		// Configure PA5 as output
		GPIOA->MODER &= ~GPIO_MODER_MODER5_Msk;
		GPIOA->MODER |= (0x01 << GPIO_MODER_MODER5_Pos);

		// Configure PA5 as Push-Pull output
		GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;

		// Configure PA5 as High-Speed Output
		GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR5_Msk;
		GPIOA->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR5_Pos);

		// Disable PA5 Pull-up/Pull-down
		GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5_Msk;

		// Set Initial State OFF
		GPIOA->BSRR = GPIO_BSRR_BR_5;
	}

	/*
 * BSP_LED_On()
 * Turn ON LED on PA5
 */

	void BSP_LED_On()
	{
		GPIOA->BSRR = GPIO_BSRR_BS_5;
	}

	/*
 * BSP_LED_Off()
 * Turn OFF LED on PA5
 */

	void BSP_LED_Off()
	{
		GPIOA->BSRR = GPIO_BSRR_BR_5;
	}

	/*
 * BSP_LED_Toggle()
 * Toggle LED on PA5
 */

	void BSP_LED_Toggle()
	{
		GPIOA->ODR ^= GPIO_ODR_5;
	}

	/*
 * BSP_PB_Init()
 * Initialize Push-Button pin (PC13) as input without Pull-up/Pull-down
 */

	void BSP_PB_Init()
	{
		// Enable GPIOC clock
		RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

		// Configure PC13 as input
		GPIOC->MODER &= ~GPIO_MODER_MODER13_Msk;
		GPIOC->MODER |= (0x00 << GPIO_MODER_MODER13_Pos);

		// Disable PC13 Pull-up/Pull-down
		GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR13_Msk;
	}

	/*
 * BSP_PB_GetState()
 * Returns the state of the button (0=released, 1=pressed)
 */

	uint8_t BSP_PB_GetState()
	{
		uint8_t state;

		if ((GPIOC->IDR & GPIO_IDR_13) == GPIO_IDR_13)
		{
			state = 0;
		}
		else
		{
			state = 1;
		}
		return state;
	}

	/*
 * BSP_Console_Init()
 * USART2 @ 115200 Full Duplex
 * 1 start - 8-bit - 1 stop
 * TX -> PA2 (AF1)
 * RX -> PA3 (AF1)
 */

	void BSP_Console_Init()
	{
		// Enable GPIOA clock
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

		// Configure PA2 and PA3 as Alternate function
		GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);
		GPIOA->MODER |= (0x02 << GPIO_MODER_MODER2_Pos) | (0x02 << GPIO_MODER_MODER3_Pos);

		// Set PA2 and PA3 to AF1 (USART2)
		GPIOA->AFR[0] &= ~(0x0000FF00);
		GPIOA->AFR[0] |= (0x00001100);

		// Enable USART2 clock
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

		// Clear USART2 configuration (reset state)
		// 8-bit, 1 start, 1 stop, CTS/RTS disabled
		USART2->CR1 = 0x00000000;
		USART2->CR2 = 0x00000000;
		USART2->CR3 = 0x00000000;

		// Select PCLK (APB1) as clock source
		// PCLK -> 48 MHz
		RCC->CFGR3 &= ~RCC_CFGR3_USART2SW_Msk;

		// Baud Rate = 115200
		// With OVER8=0 and Fck=48MHz, USARTDIV =   48E6/115200 = 416.6666
		// BRR = 417 -> Actual BaudRate = 115107.9137 -> 0.08% error
		//
		// With OVER8=1 and Fck=48MHz, USARTDIV = 2*48E6/115200 = 833.3333
		// BRR = 833 -> Actual BaudRate = 115246.0984 -> 0.04% error (better choice)

		USART2->CR1 |= USART_CR1_OVER8;
		USART2->BRR = 833;

		// Enable both Transmitter and Receiver
		USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

		// Enable USART2
		USART2->CR1 |= USART_CR1_UE;
	}

	void BSP_SPI1_Init()
	{
		// SPI_SCK  -> PB3 (AF0)
		// SPI_MISO -> PA6 (AF0)
		// SPI_MOSI -> PA7 (AF0)
		// CS pin	-> PC7 (GPIO)

		// Configure PB6 as CS pin

		// Enable GPIOB clock
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

		// Configure PB6 as output
		GPIOB->MODER &= ~GPIO_MODER_MODER6_Msk;
		GPIOB->MODER |= (0x01 << GPIO_MODER_MODER6_Pos);

		// Configure PB6 as Push-Pull output
		GPIOB->OTYPER &= ~GPIO_OTYPER_OT_6;

		// Configure PB6 as High-Speed Output
		GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR6_Msk;
		GPIOB->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR6_Pos);

		// Disable PB6 Pull-up/Pull-down
		GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6_Msk;

		// Idle state is high
		GPIOB->BSRR = GPIO_BSRR_BS_6;

		// Configure PA5, PA6, PA7 as SPI1 pins (AF0)

		// Enable GPIOA clock
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

		// Configure PA5, PA6, PA7 as AF mode
		GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
		GPIOA->MODER |= (0x02 << 10U) | (0x02 << 12U) | (0x02 << 14U);

		// Connect to SPI1 (AF0)
		GPIOA->AFR[0] &= ~(0xFFF00000);
		GPIOA->AFR[0] |= (0x00000000);

		// Enable SPI1 Clock
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

		// Configure SPI with default config
		// 4-wire = full-duplex (MISO/MOSI on separated lines)
		// 8-bit, no CRC, MSB first
		// CPHA = 0, CPOL = 0 -> SPI mode 0 (first clock rising edge)
		SPI1->CR1 = 0x0000;
		SPI1->CR2 = 0x0000;

		// Set the baudrate to 48MHz /128 = 375kHz (slow, but easy to debug)
		SPI1->CR1 |= 0x06 << SPI_CR1_BR_Pos;

		// Set data size to 8-bit
		SPI1->CR2 |= 0x07 << SPI_CR2_DS_Pos;

		// Set as master (SSI must be high), with software managed NSS
		SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;

		// Enable SPI1
		SPI1->CR1 |= SPI_CR1_SPE;
	}

	uint8_t BSP_SPI_SendReceive(uint8_t tx_byte)
	{
		uint8_t rx_byte;

		// Make sure TXE is set before sending data
		while ((SPI1->SR & SPI_SR_TXE_Msk) == 0)
			;

		// Send tx_byte
		*(__IO uint8_t *)&SPI1->DR = tx_byte;

		// Wait until incoming data has arrived
		while ((SPI1->SR & SPI_SR_RXNE_Msk) == 0)
			;

		// Read data
		rx_byte = *(__IO uint8_t *)&SPI1->DR;

		return rx_byte;
	}

	void BSP_Write(uint8_t register_address, uint8_t data)
	{
		// Set FIFO threshold to 1-byte
		SPI1->CR2 |= SPI_CR2_FRXTH;

		// Select slave (CS -> low)
		GPIOB->BSRR = GPIO_BSRR_BR_6;

		// Send register address to write to
		BSP_SPI_SendReceive(register_address | 0x80);

		// Send data to write
		BSP_SPI_SendReceive(data);

		// Release slave (CS -> High)
		GPIOB->BSRR = GPIO_BSRR_BS_6;
	}

	void BSP_Read(uint8_t register_address, uint8_t *buffer, uint8_t nbytes)
	{
		uint8_t n;

		// Set FIFO threshold to 1-byte
		SPI1->CR2 |= SPI_CR2_FRXTH;

		// Select slave (CS -> low)
		GPIOB->BSRR = GPIO_BSRR_BR_6;

		// Send register address to read from
		BSP_SPI_SendReceive(register_address);

		// Read loop
		n = nbytes;

		while (n > 0)
		{
			*buffer = BSP_SPI_SendReceive(0x00);
			buffer++;
			n--;
		}

		// Release slave (CS -> High)
		GPIOB->BSRR = GPIO_BSRR_BS_6;
	}

#ifdef __cplusplus
}
#endif
