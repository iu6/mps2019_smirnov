/* -----------------------------------------------------------------------------
 * Copyright (c) 2015 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * $Date:        6. September 2015
 * $Revision:    V1.00
 *
 * Project:      RTE Device Configuration for Milandr MDR1986VE9X
 * -------------------------------------------------------------------------- */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H

#include <MDR32Fx.h>

//		<h> System Core Clock
//			<o> HCLK (Hz) <1-80000000>
//			<i> System Core Clock
//		</h> System Core Clock
#define RTE_HCLK 80000000

// <e> UART0 (Universal Asynchronous Receiver Transmitter) [Driver_USART0]
#define   RTE_UART0                     0

//   <h> Pin Configuration
//     <o> TX <0=>PA7 <1=>PB0 <2=>PB5 <3=>PD8 <4=>PE13
//     <i> UART0 Serial Output pin
#define   RTE_UART0_TX_ID               0
#if      (RTE_UART0_TX_ID == 0)
  #define RTE_UART0_TX_PORT             MDR_PORTA
  #define RTE_UART0_TX_BIT              7
  #define RTE_UART0_TX_FUNC             PORT_FUNC_MODE_OVER
#elif    (RTE_UART0_TX_ID == 1)
  #define RTE_UART0_TX_PORT             MDR_PORTB
  #define RTE_UART0_TX_BIT              0
  #define RTE_UART0_TX_FUNC             PORT_FUNC_MODE_OVER
#elif    (RTE_UART0_TX_ID == 2)
  #define RTE_UART0_TX_PORT             MDR_PORTB
  #define RTE_UART0_TX_BIT              5
  #define RTE_UART0_TX_FUNC             PORT_FUNC_MODE_ALT
#elif    (RTE_UART0_TX_ID == 3)
  #define RTE_UART0_TX_PORT             MDR_PORTD
  #define RTE_UART0_TX_BIT              8
  #define RTE_UART0_TX_FUNC             PORT_FUNC_MODE_OVER
#elif    (RTE_UART0_TX_ID == 4)
  #define RTE_UART0_TX_PORT             MDR_PORTE
  #define RTE_UART0_TX_BIT              13
  #define RTE_UART0_TX_FUNC             PORT_FUNC_MODE_OVER
#else
  #error "Invalid UART0_TX Pin Configuration!"
#endif
//   <o> RX <0=>PA6 <1=>PB6 <2=>PD7 <3=>PE12
//   <i> UART0 Serial Input pin
#define   RTE_UART0_RX_ID               0
#if      (RTE_UART0_RX_ID == 0)
  #define RTE_UART0_RX_PORT             MDR_PORTA
  #define RTE_UART0_RX_BIT              6
  #define RTE_UART0_RX_FUNC             PORT_FUNC_MODE_OVER
#elif    (RTE_UART0_RX_ID == 1)
  #define RTE_UART0_RX_PORT             MDR_PORTB
  #define RTE_UART0_RX_BIT              6
  #define RTE_UART0_RX_FUNC             PORT_FUNC_MODE_ALT
#elif    (RTE_UART0_RX_ID == 2)
  #define RTE_UART0_RX_PORT             MDR_PORTD
  #define RTE_UART0_RX_BIT              7
  #define RTE_UART0_RX_FUNC             PORT_FUNC_MODE_OVER
#elif    (RTE_UART0_RX_ID == 3)
  #define RTE_UART0_RX_PORT             MDR_PORTE
  #define RTE_UART0_RX_BIT              12
  #define RTE_UART0_RX_FUNC             PORT_FUNC_MODE_OVER
#else
  #error "Invalid UART0_RX Pin Configuration!"
#endif

//     <h> Modem Lines
//       <o> CTS <0=>Not used <1=>PA15 
#define   RTE_UART0_CTS_ID              0
#if      (RTE_UART0_CTS_ID == 0)
  #define RTE_UART0_CTS_PIN_EN          0
#elif    (RTE_UART0_CTS_ID == 1)
  #define RTE_UART0_CTS_PORT            MDR_PORTA
  #define RTE_UART0_CTS_BIT             15
  #define RTE_UART0_CTS_FUNC            PORT_FUNC_MODE_ALT
#else
  #error "Invalid UART0_CTS Pin Configuration!"
#endif
#ifndef   RTE_UART0_CTS_PIN_EN
  #define RTE_UART0_CTS_PIN_EN          1
#endif
//       <o> RTS <0=>Not used <1=>PA11
#define   RTE_UART0_RTS_ID              0
#if      (RTE_UART0_RTS_ID == 0)
  #define RTE_UART0_RTS_PIN_EN          0
#elif    (RTE_UART0_RTS_ID == 1)
  #define RTE_UART0_RTS_PORT            MDR_PORTA
  #define RTE_UART0_RTS_BIT             11
  #define RTE_UART0_RTS_FUNC            PORT_FUNC_MODE_ALT
#else
  #error "Invalid UART0_RTS Pin Configuration!"
#endif
#ifndef   RTE_UART0_RTS_PIN_EN
  #define RTE_UART0_RTS_PIN_EN          1
#endif
//       <o> DCD <0=>Not used <1=>PA13
#define   RTE_UART0_DCD_ID              0
#if      (RTE_UART0_DCD_ID == 0)
  #define RTE_UART0_DCD_PIN_EN          0
#elif    (RTE_UART0_DCD_ID == 1)
  #define RTE_UART0_DCD_PORT            MDR_PORTA
  #define RTE_UART0_DCD_BIT             13
  #define RTE_UART0_DCD_FUNC            PORT_FUNC_MODE_ALT
#else
  #error "Invalid UART0_DCD Pin Configuration!"
#endif
#ifndef   RTE_UART0_DCD_PIN_EN
  #define RTE_UART0_DCD_PIN_EN          1
#endif
//       <o> DSR <0=>Not used <1=>PA14
#define   RTE_UART0_DSR_ID              0
#if      (RTE_UART0_DSR_ID == 0)
  #define RTE_UART0_DSR_PIN_EN          0
#elif    (RTE_UART0_DSR_ID == 1)
  #define RTE_UART0_DSR_PORT            MDR_PORTA
  #define RTE_UART0_DSR_BIT             14
  #define RTE_UART0_DSR_FUNC            PORT_FUNC_MODE_ALT
#else
  #error "Invalid UART0_DSR Pin Configuration!"
#endif
#ifndef   RTE_UART0_DSR_PIN_EN
  #define RTE_UART0_DSR_PIN_EN          1
#endif
//       <o> DTR <0=>Not used <1=>PA10
#define   RTE_UART0_DTR_ID              0
#if      (RTE_UART0_DTR_ID == 0)
  #define RTE_UART0_DTR_PIN_EN          0
#elif    (RTE_UART0_DTR_ID == 1)
  #define RTE_UART0_DTR_PORT            MDR_PORTA
  #define RTE_UART0_DTR_BIT             10
  #define RTE_UART0_DTR_FUNC            PORT_FUNC_MODE_ALT
#else
  #error "Invalid UART0_DTR Pin Configuration!"
#endif
#ifndef   RTE_UART0_DTR_PIN_EN
  #define RTE_UART0_DTR_PIN_EN          1
#endif
//       <o> RI <0=>Not used <1=>PA12
#define   RTE_UART0_RI_ID               0
#if      (RTE_UART0_RI_ID == 0)
  #define RTE_UART0_RI_PIN_EN           0
#elif    (RTE_UART0_RI_ID == 1)
  #define RTE_UART0_RI_PORT             MDR_PORTA
  #define RTE_UART0_RI_BIT              12
  #define RTE_UART0_RI_FUNC             PORT_FUNC_MODE_ALT
#else
  #error "Invalid UART0_RI Pin Configuration!"
#endif
#ifndef   RTE_UART0_RI_PIN_EN
  #define RTE_UART0_RI_PIN_EN           1
#endif
//     </h> Modem Lines
//   </h> Pin Configuration

//	<h> Clock
//		<o> UART0 clock divider
//		<i> UART0 clock divider
//			<0=> Not used
//			<1=> /2
//			<2=> /4
//			<3=> /8
//			<4=> /16
//			<5=> /32
//			<6=> /64
//			<7=> /128
//	</h> Clock
#define RTE_UART0_BRG 0

// </e> UART0 (Universal Asynchronous Receiver Transmitter) [Driver_USART0]

// <e> UART1 (Universal Asynchronous Receiver Transmitter) [Driver_USART1]
#define   RTE_UART1                     0

//   <h> Pin Configuration
//     <o> TX <0=>PD1 <1=>PF1
//     <i> UART1 Serial Output pin
#define   RTE_UART1_TX_ID               0
#if      (RTE_UART1_TX_ID == 0)
  #define RTE_UART1_TX_PORT             MDR_PORTD
  #define RTE_UART1_TX_BIT              1
  #define RTE_UART1_TX_FUNC             PORT_FUNC_MODE_ALT
#elif    (RTE_UART1_TX_ID == 1)
  #define RTE_UART1_TX_PORT             MDR_PORTF
  #define RTE_UART1_TX_BIT              1
  #define RTE_UART1_TX_FUNC             PORT_FUNC_MODE_OVER
#else
  #error "Invalid UART1_TX Pin Configuration!"
#endif
//   <o> RX <0=>PB1 <1=>PD0 <2=>PF0
//   <i> UART1 Serial Input pin
#define   RTE_UART1_RX_ID               0
#if      (RTE_UART1_RX_ID == 0)
  #define RTE_UART1_RX_PORT             MDR_PORTB
  #define RTE_UART1_RX_BIT              1
  #define RTE_UART1_RX_FUNC             PORT_FUNC_MODE_OVER
#elif    (RTE_UART1_RX_ID == 1)
  #define RTE_UART1_RX_PORT             MDR_PORTD
  #define RTE_UART1_RX_BIT              0
  #define RTE_UART1_RX_FUNC             PORT_FUNC_MODE_ALT
#elif    (RTE_UART1_RX_ID == 2)
  #define RTE_UART1_RX_PORT             MDR_PORTF
  #define RTE_UART1_RX_BIT              0
  #define RTE_UART1_RX_FUNC             PORT_FUNC_MODE_OVER
#else
  #error "Invalid UART1_RX Pin Configuration!"
#endif

//     <h> Modem Lines
//       <o> CTS <0=>Not used
#define   RTE_UART1_CTS_ID              0
#if      (RTE_UART1_CTS_ID == 0)
  #define RTE_UART1_CTS_PIN_EN          0
#else
  #error "Invalid UART1_CTS Pin Configuration!"
#endif
#ifndef   RTE_UART1_CTS_PIN_EN
  #define RTE_UART1_CTS_PIN_EN          0
#endif
//       <o> RTS <0=>Not used
#define   RTE_UART1_RTS_ID              0
#if      (RTE_UART1_RTS_ID == 0)
  #define RTE_UART1_RTS_PIN_EN          0
#else
  #error "Invalid UART1_RTS Pin Configuration!"
#endif
#ifndef   RTE_UART1_RTS_PIN_EN
  #define RTE_UART1_RTS_PIN_EN          0
#endif
//       <o> DCD <0=>Not used
#define   RTE_UART1_DCD_ID              0
#if      (RTE_UART1_DCD_ID == 0)
  #define RTE_UART1_DCD_PIN_EN          0
#else
  #error "Invalid UART1_DCD Pin Configuration!"
#endif
#ifndef   RTE_UART1_DCD_PIN_EN
  #define RTE_UART1_DCD_PIN_EN          0
#endif
//       <o> DSR <0=>Not used
#define   RTE_UART1_DSR_ID              0
#if      (RTE_UART1_DSR_ID == 0)
  #define RTE_UART1_DSR_PIN_EN          0
#else
  #error "Invalid UART1_DSR Pin Configuration!"
#endif
#ifndef   RTE_UART1_DSR_PIN_EN
  #define RTE_UART1_DSR_PIN_EN          0
#endif
//       <o> DTR <0=>Not used
#define   RTE_UART1_DTR_ID              0
#if      (RTE_UART1_DTR_ID == 0)
  #define RTE_UART1_DTR_PIN_EN          0
#else
  #error "Invalid UART1_DTR Pin Configuration!"
#endif
#ifndef   RTE_UART1_DTR_PIN_EN
  #define RTE_UART1_DTR_PIN_EN          0
#endif
//       <o> RI <0=>Not used
#define   RTE_UART1_RI_ID               0
#if      (RTE_UART1_RI_ID == 0)
  #define RTE_UART1_RI_PIN_EN           0
#else
  #error "Invalid UART1_RI Pin Configuration!"
#endif
#ifndef   RTE_UART1_RI_PIN_EN
  #define RTE_UART1_RI_PIN_EN           0
#endif
//     </h> Modem Lines
//   </h> Pin Configuration

//	<h> Clock
//		<o> UART1 clock divider
//		<i> UART1 clock divider
//			<0=> Not used
//			<1=> /2
//			<2=> /4
//			<3=> /8
//			<4=> /16
//			<5=> /32
//			<6=> /64
//			<7=> /128
//	</h> Clock
#define RTE_UART1_BRG 0

// </e> UART1 (Universal Asynchronous Receiver Transmitter) [Driver_USART1]

// <e> SSP0 (Synchronous Serial Port 0) [Driver_SPI0]
// <i> Configuration settings for Driver_SPI0 in component ::Drivers:SPI
#define   RTE_SSP0                      1

//   <h> Pin Configuration
//     <o> SSP0_SSEL <0=>Not used <1=>PB12 <2=>PD9 <3=>PE13 <4=>PF2
//     <i> Slave Select for SSP0
#define   RTE_SSP0_SSEL_PIN_SEL         4
#if      (RTE_SSP0_SSEL_PIN_SEL == 0)
#define   RTE_SSP0_SSEL_PIN_EN          0
#elif    (RTE_SSP0_SSEL_PIN_SEL == 1)
  #define RTE_SSP0_SSEL_PORT            MDR_PORTB
  #define RTE_SSP0_SSEL_BIT             12
  #define RTE_SSP0_SSEL_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP0_SSEL_PIN_SEL == 2)
  #define RTE_SSP0_SSEL_PORT            MDR_PORTD
  #define RTE_SSP0_SSEL_BIT             9
  #define RTE_SSP0_SSEL_FUNC            PORT_FUNC_MODE_OVER
#elif    (RTE_SSP0_SSEL_PIN_SEL == 3)
  #define RTE_SSP0_SSEL_PORT            MDR_PORTE
  #define RTE_SSP0_SSEL_BIT             13
  #define RTE_SSP0_SSEL_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP0_SSEL_PIN_SEL == 4)
  #define RTE_SSP0_SSEL_PORT            MDR_PORTF
  #define RTE_SSP0_SSEL_BIT             2
  #define RTE_SSP0_SSEL_FUNC            PORT_FUNC_MODE_ALT
#else
  #error "Invalid SSP0 SSP0_SSEL Pin Configuration!"
#endif
#ifndef   RTE_SSP0_SSEL_PIN_EN
#define   RTE_SSP0_SSEL_PIN_EN          1
#endif
//     <o> SSP0_SCK <0=>PB13 <1=>PD10 <2=>PF1
//     <i> Serial clock for SSP0
#define   RTE_SSP0_SCK_PIN_SEL          2
#if      (RTE_SSP0_SCK_PIN_SEL == 0)
  #define RTE_SSP0_SCK_PORT             MDR_PORTB
  #define RTE_SSP0_SCK_BIT              13
  #define RTE_SSP0_SCK_FUNC             PORT_FUNC_MODE_ALT
#elif    (RTE_SSP0_SCK_PIN_SEL == 1)
  #define RTE_SSP0_SCK_PORT             MDR_PORTD
  #define RTE_SSP0_SCK_BIT              10
  #define RTE_SSP0_SCK_FUNC             PORT_FUNC_MODE_OVER
#elif    (RTE_SSP0_SCK_PIN_SEL == 2)
  #define RTE_SSP0_SCK_PORT             MDR_PORTF
  #define RTE_SSP0_SCK_BIT              1
  #define RTE_SSP0_SCK_FUNC             PORT_FUNC_MODE_ALT
#else
  #error "Invalid SSP0 SSP0_SCK Pin Configuration!"
#endif
//     <o> SSP0_MISO <0=>PB14 <1=>PD11 <2=>PE12 <3=>PF3
//     <i> Master In Slave Out for SSP0
#define   RTE_SSP0_MISO_PIN_SEL         3
#if      (RTE_SSP0_MISO_PIN_SEL == 0)
  #define RTE_SSP0_MISO_PORT            MDR_PORTB
  #define RTE_SSP0_MISO_BIT             14
  #define RTE_SSP0_MISO_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP0_MISO_PIN_SEL == 1)
  #define RTE_SSP0_MISO_PORT            MDR_PORTD
  #define RTE_SSP0_MISO_BIT             11
  #define RTE_SSP0_MISO_FUNC            PORT_FUNC_MODE_OVER
#elif    (RTE_SSP0_MISO_PIN_SEL == 2)
  #define RTE_SSP0_MISO_PORT            MDR_PORTE
  #define RTE_SSP0_MISO_BIT             12
  #define RTE_SSP0_MISO_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP0_MISO_PIN_SEL == 3)
  #define RTE_SSP0_MISO_PORT            MDR_PORTF
  #define RTE_SSP0_MISO_BIT             3
  #define RTE_SSP0_MISO_FUNC            PORT_FUNC_MODE_ALT
#else
  #error "Invalid SSP0 SSP0_MISO Pin Configuration!"
#endif
//     <o> SSP0_MOSI <0=>PB15 <1=>PD12 <2=>PF0
//     <i> Master Out Slave In for SSP0
#define   RTE_SSP0_MOSI_PIN_SEL         2
#if      (RTE_SSP0_MOSI_PIN_SEL == 0)
  #define RTE_SSP0_MOSI_PORT            MDR_PORTB
  #define RTE_SSP0_MOSI_BIT             15
  #define RTE_SSP0_MOSI_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP0_MOSI_PIN_SEL == 1)
  #define RTE_SSP0_MOSI_PORT            MDR_PORTD
  #define RTE_SSP0_MOSI_BIT             12
  #define RTE_SSP0_MOSI_FUNC            PORT_FUNC_MODE_OVER
#elif    (RTE_SSP0_MOSI_PIN_SEL == 2)
  #define RTE_SSP0_MOSI_PORT            MDR_PORTF
  #define RTE_SSP0_MOSI_BIT             0
  #define RTE_SSP0_MOSI_FUNC            PORT_FUNC_MODE_ALT
#else
  #error "Invalid SSP0 SSP0_MOSI Pin Configuration!"
#endif
//   </h> Pin Configuration

//	<h> Clock
//		<o> SSP0 clock divider
//		<i> SSP0 clock divider
//			<0=> Not used
//			<1=> /2
//			<2=> /4
//			<3=> /8
//			<4=> /16
//			<5=> /32
//			<6=> /64
//			<7=> /128
//	</h> Clock
#define RTE_SSP0_BRG 1

// </e> SSP0 (Synchronous Serial Port 0) [Driver_SPI0]

// <e> SSP1 (Synchronous Serial Port 0) [Driver_SPI1]
// <i> Configuration settings for Driver_SPI1 in component ::Drivers:SPI
#define   RTE_SSP1                      0

//   <h> Pin Configuration
//     <o> SSP1_SSEL <0=>Not used <1=>PB12 <2=>PC0 <3=>PC14 <4=>PD3 <5=>PF12
//     <i> Slave Select for SSP1
#define   RTE_SSP1_SSEL_PIN_SEL         1
#if      (RTE_SSP1_SSEL_PIN_SEL == 0)
  #define RTE_SSP1_SSEL_PIN_EN          0
#elif    (RTE_SSP1_SSEL_PIN_SEL == 1)
  #define RTE_SSP1_SSEL_PORT            MDR_PORTB
  #define RTE_SSP1_SSEL_BIT             12
  #define RTE_SSP1_SSEL_FUNC            PORT_FUNC_MODE_OVER
#elif    (RTE_SSP1_SSEL_PIN_SEL == 2)
  #define RTE_SSP1_SSEL_PORT            MDR_PORTC
  #define RTE_SSP1_SSEL_BIT             0
  #define RTE_SSP1_SSEL_FUNC            PORT_FUNC_MODE_OVER
#elif    (RTE_SSP1_SSEL_PIN_SEL == 3)
  #define RTE_SSP1_SSEL_PORT            MDR_PORTC
  #define RTE_SSP1_SSEL_BIT             14
  #define RTE_SSP1_SSEL_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP1_SSEL_PIN_SEL == 4)
  #define RTE_SSP1_SSEL_PORT            MDR_PORTD
  #define RTE_SSP1_SSEL_BIT             3
  #define RTE_SSP1_SSEL_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP1_SSEL_PIN_SEL == 5)
  #define RTE_SSP1_SSEL_PORT            MDR_PORTF
  #define RTE_SSP1_SSEL_BIT             12
  #define RTE_SSP1_SSEL_FUNC            PORT_FUNC_MODE_OVER
#else
  #error "Invalid SSP1 SSP1_SSEL Pin Configuration!"
#endif
#ifndef   RTE_SSP1_SSEL_PIN_EN
#define   RTE_SSP1_SSEL_PIN_EN          1
#endif
//     <o> SSP1_SCK <0=>PB13 <1=>PC1 <2=>PD5 <3=>PF13
//     <i> Serial clock for SSP1
#define   RTE_SSP1_SCK_PIN_SEL          0
#if      (RTE_SSP1_SCK_PIN_SEL == 0)
  #define RTE_SSP1_SCK_PORT             MDR_PORTB
  #define RTE_SSP1_SCK_BIT              13
  #define RTE_SSP1_SCK_FUNC             PORT_FUNC_MODE_OVER
#elif    (RTE_SSP1_SCK_PIN_SEL == 1)
  #define RTE_SSP1_SCK_PORT             MDR_PORTC
  #define RTE_SSP1_SCK_BIT              1
  #define RTE_SSP1_SCK_FUNC             PORT_FUNC_MODE_OVER
#elif    (RTE_SSP1_SCK_PIN_SEL == 2)
  #define RTE_SSP1_SCK_PORT             MDR_PORTD
  #define RTE_SSP1_SCK_BIT              5
  #define RTE_SSP1_SCK_FUNC             PORT_FUNC_MODE_ALT
#elif    (RTE_SSP1_SCK_PIN_SEL == 3)
  #define RTE_SSP1_SCK_PORT             MDR_PORTF
  #define RTE_SSP1_SCK_BIT              13
  #define RTE_SSP1_SCK_FUNC             PORT_FUNC_MODE_OVER
#else
  #error "Invalid SSP1 SSP1_SCK Pin Configuration!"
#endif
//     <o> SSP1_MISO <0=>PB14 <1=>PC2 <2=>PC15 <3=>PD2 <4=>PF14
//     <i> Master In Slave Out for SSP1
#define   RTE_SSP1_MISO_PIN_SEL         0
#if      (RTE_SSP1_MISO_PIN_SEL == 0)
  #define RTE_SSP1_MISO_PORT            MDR_PORTB
  #define RTE_SSP1_MISO_BIT             14
  #define RTE_SSP1_MISO_FUNC            PORT_FUNC_MODE_OVER
#elif    (RTE_SSP1_MISO_PIN_SEL == 1)
  #define RTE_SSP1_MISO_PORT            MDR_PORTC
  #define RTE_SSP1_MISO_BIT             2
  #define RTE_SSP1_MISO_FUNC            PORT_FUNC_MODE_OVER
#elif    (RTE_SSP1_MISO_PIN_SEL == 2)
  #define RTE_SSP1_MISO_PORT            MDR_PORTC
  #define RTE_SSP1_MISO_BIT             15
  #define RTE_SSP1_MISO_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP1_MISO_PIN_SEL == 3)
  #define RTE_SSP1_MISO_PORT            MDR_PORTD
  #define RTE_SSP1_MISO_BIT             2
  #define RTE_SSP1_MISO_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP1_MISO_PIN_SEL == 4)
  #define RTE_SSP1_MISO_PORT            MDR_PORTF
  #define RTE_SSP1_MISO_BIT             14
  #define RTE_SSP1_MISO_FUNC            PORT_FUNC_MODE_OVER
#else
  #error "Invalid SSP1 SSP1_MISO Pin Configuration!"
#endif
//     <o> SSP1_MOSI <0=>PB15 <1=>PC3 <2=>PD6 <3=>PF15
//     <i> Master Out Slave In for SSP1
#define   RTE_SSP1_MOSI_PIN_SEL         0
#if      (RTE_SSP1_MOSI_PIN_SEL == 0)
  #define RTE_SSP1_MOSI_PORT            MDR_PORTB
  #define RTE_SSP1_MOSI_BIT             15
  #define RTE_SSP1_MOSI_FUNC            PORT_FUNC_MODE_OVER
#elif    (RTE_SSP1_MOSI_PIN_SEL == 1)
  #define RTE_SSP1_MOSI_PORT            MDR_PORTC
  #define RTE_SSP1_MOSI_BIT             3
  #define RTE_SSP1_MOSI_FUNC            PORT_FUNC_MODE_OVER
#elif    (RTE_SSP1_MOSI_PIN_SEL == 2)
  #define RTE_SSP1_MOSI_PORT            MDR_PORTD
  #define RTE_SSP1_MOSI_BIT             6
  #define RTE_SSP1_MOSI_FUNC            PORT_FUNC_MODE_ALT
#elif    (RTE_SSP1_MOSI_PIN_SEL == 3)
  #define RTE_SSP1_MOSI_PORT            MDR_PORTF
  #define RTE_SSP1_MOSI_BIT             15
  #define RTE_SSP1_MOSI_FUNC            PORT_FUNC_MODE_OVER
#else
  #error "Invalid SSP1 SSP1_MOSI Pin Configuration!"
#endif
//   </h> Pin Configuration

//	<h> Clock
//		<o> SSP1 clock divider
//		<i> SSP1 clock divider
//			<0=> Not used
//			<1=> /2
//			<2=> /4
//			<3=> /8
//			<4=> /16
//			<5=> /32
//			<6=> /64
//			<7=> /128
//	</h> Clock
#define RTE_SSP1_BRG 0

// </e> SSP1 (Synchronous Serial Port 0) [Driver_SPI1]

#endif  /* __RTE_DEVICE_H */
