/*****************************************************************************
 *
 * \file
 *
 * \brief AES example driver for AVR32 UC3.
 *
 * This file provides an example for the AES on AVR32 UC3 devices.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables, defines, enums, and
 * typedefs for the AES driver. <BR>It also gives several examples of usage of the
 * AES module: <BR>
 * - ECB ciphering in manual mode with no counter measures,
 * - ECB ciphering in manual mode with all counter measures activated,
 * - demonstration of the URAD security feature,
 * - ECB deciphering in auto mode with no counter measures,
 * - CBC ciphering in auto mode with no counter measures.<BR>
 *
 * \section files Main Files
 * - aes.c : AES driver
 * - aes.h : AES header file
 * - aes_example.c : AES code example
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with an AES module can be used. This example has been tested
 * with the following setup:<BR>
 * - EVK1104 evaluation kit.
 *
 * \section setupinfo Setup Information
 * <BR>CPU speed: <i> 12 MHz </i>
 * - Connect the EVK1104 USB virtual port com to a PC
 * - PC terminal settings:
 *     - 57600 bps,
 *     - 8 data bits,
 *     - no parity bit,
 *     - 1 stop bit,
 *     - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */

#include "board.h"
//#include "print_funcs.h"
#include "gpio.h"
#include "pm.h"
#include "intc.h"
//#include "usart.h"
#include "aes.h"
#include "aes_example.h"
#include "tc_task.h"
#include "sha256.h"


/*! \name Board-Related Example Settings
 */
//! @{
#if BOARD == EVK1104
#  define AES_EXAMPLE_USART               (&AVR32_USART1)
#  define AES_EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define AES_EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define AES_EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define AES_EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define AES_EXAMPLE_USART_BAUDRATE      57600
#  define AES_EXAMPLE_LED1                LED0_GPIO
#  define AES_EXAMPLE_LED2                LED1_GPIO
#  define AES_EXAMPLE_LED3                LED2_GPIO
#  define AES_EXAMPLE_LED4                LED3_GPIO
#endif

#if !defined(AES_EXAMPLE_USART)             || \
    !defined(AES_EXAMPLE_USART_RX_PIN)      || \
    !defined(AES_EXAMPLE_USART_RX_FUNCTION) || \
    !defined(AES_EXAMPLE_USART_TX_PIN)      || \
    !defined(AES_EXAMPLE_USART_TX_FUNCTION) || \
    !defined(AES_EXAMPLE_USART_BAUDRATE)    || \
    !defined(AES_EXAMPLE_LED1)              || \
    !defined(AES_EXAMPLE_LED2)              || \
    !defined(AES_EXAMPLE_LED3)              || \
    !defined(AES_EXAMPLE_LED4)
#  error The USART and LEDs configuration to use for debug on your board is missing
#endif
//! @}

/*! \name Example internal states
 */
//! @{
#define   AES_EXAMPLE_STATE_1             0
#define   AES_EXAMPLE_STATE_2             1
#define   AES_EXAMPLE_STATE_3             2
#define   AES_EXAMPLE_STATE_4             3
#define   AES_EXAMPLE_STATE_5             4
#define   AES_EXAMPLE_STATE_6             5
#define   AES_EXAMPLE_STATE_7             6
#define   AES_EXAMPLE_STATE_8             7
//! @}

#define   AES_EXAMPLE_REFBUF_SIZE         16



// Reference input data.
const unsigned int      CBCRefInputData[AES_EXAMPLE_REFBUF_SIZE] = {
  0x6bc1bee2,
  0x2e409f96,
  0xe93d7e11,
  0x7393172a,
  0xae2d8a57,
  0x1e03ac9c,
  0x9eb76fac,
  0x45af8e51,
  0x30c81c46,
  0xa35ce411,
  0xe5fbc119,
  0x1a0a52ef,
  0xf69f2445,
  0xdf4f9b17,
  0xad2b417b,
  0xe66c3710 };

  // Reference output data.
const unsigned int      CBCRefOutputData[AES_EXAMPLE_REFBUF_SIZE] = {
  0xf58c4c04,
  0xd6e5f1ba,
  0x779eabfb,
  0x5f7bfbd6,
  0x9cfc4e96,
  0x7edb808d,
  0x679f777b,
  0xc6702c7d,
  0x39f23369,
  0xa9d9bacf,
  0xa530e263,
  0x04231461,
  0xb2eb05e2,
  0xc39be9fc,
  0xda6c1907,
  0x8c6a9d1b };

// CipherKey256 array
// the_key = 256'h603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4;
//const unsigned long int CipherKey256[8] = {
  //0x603deb10,
  //0x15ca71be,
  //0x2b73aef0,
  //0x857d7781,
  //0x1f352c07,
  //0x3b6108d7,
  //0x2d9810a3,
  //0x0914dff4
//};
//
// InitVector array
// Initial Value 128'h000102030405060708090a0b0c0d0e0f
const unsigned int      InitVectorCBC[4] = {
  0x00010203,
  0x04050607,
  0x08090a0b,
  0x0c0d0e0f
};



volatile unsigned int   OutputData[AES_EXAMPLE_REFBUF_SIZE];      // Output data array

volatile unsigned char  state = 0;          // Example state machine

volatile unsigned int   ccountt0, ccountt1; // Performance evaluation variables

volatile unsigned long  Tempo;

volatile unsigned char  urad_evt = false;

volatile unsigned char  ContinueCBCProcess = true;

//static const gpio_map_t USART_GPIO_MAP =      // USART GPIO map
//{
    //{AES_EXAMPLE_USART_RX_PIN, AES_EXAMPLE_USART_RX_FUNCTION},
    //{AES_EXAMPLE_USART_TX_PIN, AES_EXAMPLE_USART_TX_FUNCTION}
//};
//
//static const usart_options_t USART_OPTIONS =  // USART options.
//{
    //.baudrate     = AES_EXAMPLE_USART_BAUDRATE,
    //.charlength   = 8,
    //.paritytype   = USART_NO_PARITY,
    //.stopbits     = USART_1_STOPBIT,
    //.channelmode  = USART_NORMAL_CHMODE
//};
//
//pm_freq_param_t pm_freq_param =
//{
  //.cpu_f        = AES_EXAMPLE_CPU_FREQ,
  //.pba_f        = AES_EXAMPLE_CPU_FREQ,
  //.osc0_f       = FOSC0,
  //.osc0_startup = OSC0_STARTUP
//};
//
/*! \brief The AES interrupt handler.
 *
 * \note The `__attribute__((__interrupt__))' (under GNU GCC for AVR32) and
 *       `__interrupt' (under IAR Embedded Workbench for Atmel AVR32) C function
 *       attributes are used to manage the `rete' instruction.
 */
#if defined(__GNUC__)
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
__interrupt
#endif
static void aes_int_handler(void)
{
  ccountt1 = Get_system_register(AVR32_COUNT);
  if(AES_EXAMPLE_STATE_1 == state)
  {
    gpio_clr_gpio_pin(AES_EXAMPLE_LED1);
    // Read the output of the encryption process into OutputData[] (this will clear the DATRDY flag).
    //aes_read_outputdata(&AVR32_AES, (unsigned int *)OutputData);
    state++;
  }
  else if(AES_EXAMPLE_STATE_2 == state)
  {
    gpio_clr_gpio_pin(AES_EXAMPLE_LED2);
    // Read the output of the encryption process into OutputData[] (this will clear the DATRDY flag).
    //aes_read_outputdata(&AVR32_AES, (unsigned int *)OutputData);
    state++;
  }
  else if(AES_EXAMPLE_STATE_3 == state)
  {
    Tempo = aes_get_status(&AVR32_AES);
    if(Tempo & AVR32_AES_ISR_URAD_MASK)
    {
      urad_evt = true;
      state++;
      if( AES_URAT_READ_WRITEONLY == ((Tempo & AVR32_AES_ISR_URAT_MASK)>>AVR32_AES_ISR_URAT_OFFSET ))
        gpio_clr_gpio_pin(AES_EXAMPLE_LED3);
      // The URAD and URAT fields of the AES status register can be reset only by
      // performing a sw reset of the AES module.
      aes_swreset(&AVR32_AES);
    }
  }
  else if(AES_EXAMPLE_STATE_4 == state)
  {
    gpio_clr_gpio_pin(AES_EXAMPLE_LED4);
    // Read the output of the decryption process into OutputData[] (this will clear the DATRDY flag).
    //aes_read_outputdata(&AVR32_AES, (unsigned int *)OutputData);
    state++;
  }
  else if((AES_EXAMPLE_STATE_5 <= state) && (AES_EXAMPLE_STATE_8 >= state))
  {
    // Read the output of the decryption process into OutputData[] (this will clear the DATRDY flag).
    //aes_read_outputdata(&AVR32_AES, (unsigned int *)&OutputData[(state - AES_EXAMPLE_STATE_5)*4]);
    ContinueCBCProcess = true;
    state++;
  }
  else
    while(1);
}


/*! \brief Initializes the HSB bus matrix.
 */
static void init_hmatrix(void)
{
  // Set flashc master type to last default to save one cycle for
  // each branch.
  avr32_hmatrix_scfg_t scfg;

  scfg = AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_FLASH];
  scfg.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_FLASH] = scfg;
}


/*! \brief The main function.
 *
 * It sets up the USART module on EXAMPLE_USART. The terminal settings are 57600
 * 8N1.
 * Then it sets up the interrupt handler and waits for a USART interrupt to
 * trigger.
 */
void aes_task (void)
{
  init_hmatrix();
}

const unsigned long int Init_Vector[4] =
{
	0x00FAAC24,	// NONCE
	0xC1585EF1,	// AES-CTR IV (BYTE1)
	0x5A43D875,	// AES-CTR IV (BYTE2)
	0x00000001	// COUNTER
};

void aes_init (unsigned char Processing_Mode)
{
	  aes_config_t    AesConf;        // AES config structure
	  // Configure the AES.
	  AesConf.ProcessingMode = Processing_Mode;   // Cipher
	  AesConf.ProcessingDelay = 0;                // No delay: best performance
	  AesConf.StartMode = AES_START_MODE_AUTO;    // Manual mode
	  AesConf.KeySize = AES_KEY_SIZE_256;         // 256bit cryptographic key
	  AesConf.OpMode = AES_CBC_MODE;              // CTR cipher mode
	  AesConf.LodMode = 0;                        // LODMODE == 0 : the DATRDY flag
	  // is cleared when one of the output data registers is read (because we're in
	  // manual mode).
	  AesConf.CFBSize = 0;                        // Don't-care because we're using the CTR mode.
	  AesConf.CounterMeasureMask = 0;             // Disable all counter measures.
	  aes_configure(&AVR32_AES, &AesConf);
	  aes_set_key(&AVR32_AES, CipherKey256);
	  // Set the cryptographic key.
	  // Key from FIPS-197 = 00010203 04050607 08090A0B 0C0D0E0F
	  //CipherKey[0] = 0x00010203;
	  //CipherKey[1] = 0x04050607;
	  //CipherKey[2] = 0x08090A0B;
	  //CipherKey[3] = 0x0C0D0E0F;
	  //CipherKey256
	  //aes_set_key(&AVR32_AES, CipherKey256);
	  //aes_set_initvector(&AVR32_AES, (unsigned long int *)InitVectorCBC);

	  // The initialization vector is not used by the ECB cipher mode.

	  // Write the data to be ciphered to the authorized Input Data registers.
	  // Plaintext from FIPS-197 = 00112233 44556677 8899AABB CCDDEEFF
	  //InputData[0] = 0x00112233;
	  //InputData[1] = 0x44556677;
	  //InputData[2] = 0x8899AABB;
	  //InputData[3] = 0xCCDDEEFF;
	  //aes_write_inputdata(&AVR32_AES, InputData);

	  //ccountt0 = Get_system_register(AVR32_COUNT);
	  // Start encryption.
	  //aes_start(&AVR32_AES);

}
uint32_t aes_out_data[128] = {0};

void apply_aes_encryption (volatile avr32_aes_t *aes, uint32_t *aes_in_data_t, uint32_t sector_id)
{
	unsigned char i = 0;
	unsigned long int temp;
	
	temp = CipherKey256_hash[0];

	CipherKey256_hash[0] = CipherKey256_hash[0] ^ sector_id;	// chunk variable is the SHA-2 of CipherKey256
							// its least significant DWORD i.e. chunk.a is XORed with the sector address
							 
	//aes_set_initvector(&AVR32_AES, (unsigned long int *)InitVectorCBC);	
	aes_set_initvector(&AVR32_AES, (unsigned long int *)CipherKey256_hash);	
	
	CipherKey256_hash[0] = temp;
		
	for (; i < 128; i+=4)
	{
		aes_init(AES_PMODE_CIPHER);
			
		aes_write_inputdata(aes, &aes_in_data_t[i]);

		while (!(aes->isr & 0x00000001));

		aes_read_outputdata(aes, &aes_out_data[i]);
		
		//aes_set_initvector(&AVR32_AES, &aes_out_data[i]);
	}
	for (i = 0; i < 128; i++)
	{
		aes_in_data_t[i] = aes_out_data[i];
	}

}


void apply_aes_decryption (volatile avr32_aes_t *aes, uint32_t *aes_in_data_t, uint32_t sector_id)
{
	
	unsigned char i = 0;
	unsigned long int temp;

	temp = CipherKey256_hash[0];

	CipherKey256_hash[0] = CipherKey256_hash[0] ^ sector_id;	// chunk variable is the SHA-2 of CipherKey256
							// its least significant DWORD i.e. chunk.a is XORed with the sector address
	
	//aes_set_initvector(&AVR32_AES, (unsigned long int *)InitVectorCBC);
	aes_set_initvector(&AVR32_AES, (unsigned long int *)CipherKey256_hash);
	
	CipherKey256_hash[0] = temp;

	for (; i < 128; i+=4)
	{
		aes_init(AES_PMODE_DECIPHER);

		aes_write_inputdata(aes, &aes_in_data_t[i]);

		while (!(aes->isr & 0x00000001));

		aes_read_outputdata(aes, &aes_out_data[i]);
		
		//aes_set_initvector(&AVR32_AES, &aes_in_data_t[i]);
	}
	for (i = 0; i < 128; i++)
	{
		aes_in_data_t[i] = aes_out_data[i];
	}

}
