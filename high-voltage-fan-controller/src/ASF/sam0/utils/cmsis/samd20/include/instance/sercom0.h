/**
 * \file
 *
 * \brief Instance description for SERCOM0
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _SAMD20_SERCOM0_INSTANCE_
#define _SAMD20_SERCOM0_INSTANCE_

/* ========== Register definition for SERCOM0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM0_I2CM_CTRLA     (0x42000800U) /**< \brief (SERCOM0) I2CM Control A */
#define REG_SERCOM0_I2CM_CTRLB     (0x42000804U) /**< \brief (SERCOM0) I2CM Control B */
#define REG_SERCOM0_I2CM_DBGCTRL   (0x42000808U) /**< \brief (SERCOM0) I2CM Debug Control */
#define REG_SERCOM0_I2CM_BAUD      (0x4200080AU) /**< \brief (SERCOM0) I2CM Baud Rate */
#define REG_SERCOM0_I2CM_INTENCLR  (0x4200080CU) /**< \brief (SERCOM0) I2CM Interrupt Enable Clear */
#define REG_SERCOM0_I2CM_INTENSET  (0x4200080DU) /**< \brief (SERCOM0) I2CM Interrupt Enable Set */
#define REG_SERCOM0_I2CM_INTFLAG   (0x4200080EU) /**< \brief (SERCOM0) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM0_I2CM_STATUS    (0x42000810U) /**< \brief (SERCOM0) I2CM Status */
#define REG_SERCOM0_I2CM_ADDR      (0x42000814U) /**< \brief (SERCOM0) I2CM Address */
#define REG_SERCOM0_I2CM_DATA      (0x42000818U) /**< \brief (SERCOM0) I2CM Data */
#define REG_SERCOM0_I2CS_CTRLA     (0x42000800U) /**< \brief (SERCOM0) I2CS Control A */
#define REG_SERCOM0_I2CS_CTRLB     (0x42000804U) /**< \brief (SERCOM0) I2CS Control B */
#define REG_SERCOM0_I2CS_INTENCLR  (0x4200080CU) /**< \brief (SERCOM0) I2CS Interrupt Enable Clear */
#define REG_SERCOM0_I2CS_INTENSET  (0x4200080DU) /**< \brief (SERCOM0) I2CS Interrupt Enable Set */
#define REG_SERCOM0_I2CS_INTFLAG   (0x4200080EU) /**< \brief (SERCOM0) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM0_I2CS_STATUS    (0x42000810U) /**< \brief (SERCOM0) I2CS Status */
#define REG_SERCOM0_I2CS_ADDR      (0x42000814U) /**< \brief (SERCOM0) I2CS Address */
#define REG_SERCOM0_I2CS_DATA      (0x42000818U) /**< \brief (SERCOM0) I2CS Data */
#define REG_SERCOM0_SPI_CTRLA      (0x42000800U) /**< \brief (SERCOM0) SPI Control A */
#define REG_SERCOM0_SPI_CTRLB      (0x42000804U) /**< \brief (SERCOM0) SPI Control B */
#define REG_SERCOM0_SPI_DBGCTRL    (0x42000808U) /**< \brief (SERCOM0) SPI Debug Control */
#define REG_SERCOM0_SPI_BAUD       (0x4200080AU) /**< \brief (SERCOM0) SPI Baud Rate */
#define REG_SERCOM0_SPI_INTENCLR   (0x4200080CU) /**< \brief (SERCOM0) SPI Interrupt Enable Clear */
#define REG_SERCOM0_SPI_INTENSET   (0x4200080DU) /**< \brief (SERCOM0) SPI Interrupt Enable Set */
#define REG_SERCOM0_SPI_INTFLAG    (0x4200080EU) /**< \brief (SERCOM0) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM0_SPI_STATUS     (0x42000810U) /**< \brief (SERCOM0) SPI Status */
#define REG_SERCOM0_SPI_ADDR       (0x42000814U) /**< \brief (SERCOM0) SPI Address */
#define REG_SERCOM0_SPI_DATA       (0x42000818U) /**< \brief (SERCOM0) SPI Data */
#define REG_SERCOM0_USART_CTRLA    (0x42000800U) /**< \brief (SERCOM0) USART Control A */
#define REG_SERCOM0_USART_CTRLB    (0x42000804U) /**< \brief (SERCOM0) USART Control B */
#define REG_SERCOM0_USART_DBGCTRL  (0x42000808U) /**< \brief (SERCOM0) USART Debug Control */
#define REG_SERCOM0_USART_BAUD     (0x4200080AU) /**< \brief (SERCOM0) USART Baud */
#define REG_SERCOM0_USART_INTENCLR (0x4200080CU) /**< \brief (SERCOM0) USART Interrupt Enable Clear */
#define REG_SERCOM0_USART_INTENSET (0x4200080DU) /**< \brief (SERCOM0) USART Interrupt Enable Set */
#define REG_SERCOM0_USART_INTFLAG  (0x4200080EU) /**< \brief (SERCOM0) USART Interrupt Flag Status and Clear */
#define REG_SERCOM0_USART_STATUS   (0x42000810U) /**< \brief (SERCOM0) USART Status */
#define REG_SERCOM0_USART_DATA     (0x42000818U) /**< \brief (SERCOM0) USART Data */
#else
#define REG_SERCOM0_I2CM_CTRLA     (*(RwReg  *)0x42000800U) /**< \brief (SERCOM0) I2CM Control A */
#define REG_SERCOM0_I2CM_CTRLB     (*(RwReg  *)0x42000804U) /**< \brief (SERCOM0) I2CM Control B */
#define REG_SERCOM0_I2CM_DBGCTRL   (*(RwReg8 *)0x42000808U) /**< \brief (SERCOM0) I2CM Debug Control */
#define REG_SERCOM0_I2CM_BAUD      (*(RwReg16*)0x4200080AU) /**< \brief (SERCOM0) I2CM Baud Rate */
#define REG_SERCOM0_I2CM_INTENCLR  (*(RwReg8 *)0x4200080CU) /**< \brief (SERCOM0) I2CM Interrupt Enable Clear */
#define REG_SERCOM0_I2CM_INTENSET  (*(RwReg8 *)0x4200080DU) /**< \brief (SERCOM0) I2CM Interrupt Enable Set */
#define REG_SERCOM0_I2CM_INTFLAG   (*(RwReg8 *)0x4200080EU) /**< \brief (SERCOM0) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM0_I2CM_STATUS    (*(RwReg16*)0x42000810U) /**< \brief (SERCOM0) I2CM Status */
#define REG_SERCOM0_I2CM_ADDR      (*(RwReg8 *)0x42000814U) /**< \brief (SERCOM0) I2CM Address */
#define REG_SERCOM0_I2CM_DATA      (*(RwReg8 *)0x42000818U) /**< \brief (SERCOM0) I2CM Data */
#define REG_SERCOM0_I2CS_CTRLA     (*(RwReg  *)0x42000800U) /**< \brief (SERCOM0) I2CS Control A */
#define REG_SERCOM0_I2CS_CTRLB     (*(RwReg  *)0x42000804U) /**< \brief (SERCOM0) I2CS Control B */
#define REG_SERCOM0_I2CS_INTENCLR  (*(RwReg8 *)0x4200080CU) /**< \brief (SERCOM0) I2CS Interrupt Enable Clear */
#define REG_SERCOM0_I2CS_INTENSET  (*(RwReg8 *)0x4200080DU) /**< \brief (SERCOM0) I2CS Interrupt Enable Set */
#define REG_SERCOM0_I2CS_INTFLAG   (*(RwReg8 *)0x4200080EU) /**< \brief (SERCOM0) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM0_I2CS_STATUS    (*(RwReg16*)0x42000810U) /**< \brief (SERCOM0) I2CS Status */
#define REG_SERCOM0_I2CS_ADDR      (*(RwReg  *)0x42000814U) /**< \brief (SERCOM0) I2CS Address */
#define REG_SERCOM0_I2CS_DATA      (*(RwReg8 *)0x42000818U) /**< \brief (SERCOM0) I2CS Data */
#define REG_SERCOM0_SPI_CTRLA      (*(RwReg  *)0x42000800U) /**< \brief (SERCOM0) SPI Control A */
#define REG_SERCOM0_SPI_CTRLB      (*(RwReg  *)0x42000804U) /**< \brief (SERCOM0) SPI Control B */
#define REG_SERCOM0_SPI_DBGCTRL    (*(RwReg8 *)0x42000808U) /**< \brief (SERCOM0) SPI Debug Control */
#define REG_SERCOM0_SPI_BAUD       (*(RwReg8 *)0x4200080AU) /**< \brief (SERCOM0) SPI Baud Rate */
#define REG_SERCOM0_SPI_INTENCLR   (*(RwReg8 *)0x4200080CU) /**< \brief (SERCOM0) SPI Interrupt Enable Clear */
#define REG_SERCOM0_SPI_INTENSET   (*(RwReg8 *)0x4200080DU) /**< \brief (SERCOM0) SPI Interrupt Enable Set */
#define REG_SERCOM0_SPI_INTFLAG    (*(RwReg8 *)0x4200080EU) /**< \brief (SERCOM0) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM0_SPI_STATUS     (*(RwReg16*)0x42000810U) /**< \brief (SERCOM0) SPI Status */
#define REG_SERCOM0_SPI_ADDR       (*(RwReg  *)0x42000814U) /**< \brief (SERCOM0) SPI Address */
#define REG_SERCOM0_SPI_DATA       (*(RwReg16*)0x42000818U) /**< \brief (SERCOM0) SPI Data */
#define REG_SERCOM0_USART_CTRLA    (*(RwReg  *)0x42000800U) /**< \brief (SERCOM0) USART Control A */
#define REG_SERCOM0_USART_CTRLB    (*(RwReg  *)0x42000804U) /**< \brief (SERCOM0) USART Control B */
#define REG_SERCOM0_USART_DBGCTRL  (*(RwReg8 *)0x42000808U) /**< \brief (SERCOM0) USART Debug Control */
#define REG_SERCOM0_USART_BAUD     (*(RwReg16*)0x4200080AU) /**< \brief (SERCOM0) USART Baud */
#define REG_SERCOM0_USART_INTENCLR (*(RwReg8 *)0x4200080CU) /**< \brief (SERCOM0) USART Interrupt Enable Clear */
#define REG_SERCOM0_USART_INTENSET (*(RwReg8 *)0x4200080DU) /**< \brief (SERCOM0) USART Interrupt Enable Set */
#define REG_SERCOM0_USART_INTFLAG  (*(RwReg8 *)0x4200080EU) /**< \brief (SERCOM0) USART Interrupt Flag Status and Clear */
#define REG_SERCOM0_USART_STATUS   (*(RwReg16*)0x42000810U) /**< \brief (SERCOM0) USART Status */
#define REG_SERCOM0_USART_DATA     (*(RwReg16*)0x42000818U) /**< \brief (SERCOM0) USART Data */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM0 peripheral ========== */
#define SERCOM0_GCLK_ID_CORE        13
#define SERCOM0_GCLK_ID_SLOW        12
#define SERCOM0_INT_MSB             3
#define SERCOM0_PMSB                3

#endif /* _SAMD20_SERCOM0_INSTANCE_ */
