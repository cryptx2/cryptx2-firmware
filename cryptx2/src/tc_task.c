/**
 * \file
 *
 * \brief TC Example application for AVR UC3 TC driver.
 *
 * Copyright (c) 2011 Atmel Corporation. All rights reserved.
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
 */

/**
 * \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the TC driver.\n It also gives an example
 * of the usage of the TC module.
 *
 * This example will start a timer/counter and generates "tick" interrupts.
 *
 * The selected timer input clock is the internal clock labelled TC3
 * referred to as TIMER_CLOCK3 in the datasheet. TIMER_CLOCK3 is connected to
 * fPBA / 8 (see datasheet).
 *
 * The 16-bit timer/counter channel will cycle from 0x0000 to RC. RC is
 * initialized to (fPBA / 8) / 100, so that an interrupt will be triggered
 * every 10 ms. Upon interrupt, a GPIO line is toggled thus producing a square
 * signal of frequency 50 Hz.
 *
 *
 * \section files Main Files
 * - tc.c: TC driver
 * - tc.h: TC driver header file
 * - tc_simulator_example1.c: TC example
 * - conf_example.h: TC Configuration
 * - conf_clock.h: Clock configuration for example
 * - conf_board.h: Simulator configuration
 *
 * \section compinfo Compilation Info
 * This software was written for the GCC compiler for AVR32
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a TC module supported in AVR Studio 5 Simulator
 * can be used. This example has been tested with the following board:
 * - AVR Simulator
 *
 * \section setupinfo Setup Information
 * This example has been tested with the following configuration:
 * - AVR Simulator
 * - CPU clock: <i> Internal RC oscillator (about 115200 Hz) </i>.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */

#include <stdint.h>
#include <stdbool.h>


#include "tc.h"
#include "sysclk.h"
#include "board.h"
#include "gpio.h"
#include "intc.h"
#include "conf_example.h"
#include "sha256.h"
#include "Salt.h"


#if !defined(EXAMPLE_TC) || !defined(EXAMPLE_TC_IRQ)
#  error The TC preprocessor configuration to use in this example is missing.
#endif

#if !defined(EXAMPLE_TOGGLE_PIN)
#  error The preprocessor configuration of the GPIO pin to toggle is missing.
#endif
//! @}

#define PB1			AVR32_PIN_PA11
#define PB2			AVR32_PIN_PA12
#define PB3			AVR32_PIN_PA13
#define PB4			AVR32_PIN_PA10
#define PB_ENTER	AVR32_PIN_PB10

#define ENABLED		true
#define DISABLED	false

#define PUSH_BUTTON1	0
#define PUSH_BUTTON2	1
#define PUSH_BUTTON3	2
#define PUSH_BUTTON4	3
#define ENTER_BUTTON	4
#define NO_BUTTON		255

void tc_task(void);

uint8_t button_pressed (void);
bool read_push_button(uint32_t pin, uint8_t *counter);
void store_passcode(uint32_t value);
void push_buttons_init(void);
bool check_all_buttons_high(void);
//! \name Example configuration
//!@{
/**
 * \def CONFIG_SYSCLK_SOURCE
 * \brief Clock source to use
 */
/**
 * \def EXAMPLE_TC
 * \brief Base address of TC module to use.
 */
/**
 * \def EXAMPLE_TC_CHANNEL
 * \brief Channel to use with TC module.
 */
/**
 * \def EXAMPLE_TC_IRQ
 * \brief IRQ Line number for TC module
 */
 /**
 * \def EXAMPLE_TC_IRQ_PRIORITY
 * \brief IRQ Group number for TC module
 */
/**
 * \def EXAMPLE_TOGGLE_PIN
 * \brief GPIO Line to toggle on compare match.
 */
//!@}

// Flag to update the timer value
volatile static bool update_timer = true;
volatile bool enter_pressed = true;
// Variable to contain the time ticks occurred
volatile static uint32_t tc_tick = 0;

volatile bool inter_key_delay = DISABLED;
volatile uint8_t delay_counter = 0;
volatile bool button_released = true;
volatile uint8_t mode_chosen = oxff;
volatile bool mode_selected = false;



unsigned long int CipherKey256_hash[8] = {0};

volatile uint8_t PB1_Counter = 0;
volatile uint8_t PB2_Counter = 0;
volatile uint8_t PB3_Counter = 0;
volatile uint8_t PB4_Counter = 0;
volatile uint8_t PB5_Counter = 0;


/**
 * \brief TC interrupt.
 *
 * The ISR handles RC compare interrupt and sets the update_timer flag to
 * update the timer value.
 */
__attribute__((__interrupt__))
static void tc_irq(void)
{
	// Increment the ms seconds counter
	tc_tick++;
	/*
	 * TODO: Place a breakpoint here and watch the update of tc_tick variable
	 * in the Watch Window.
	 */

	
	// Clear the interrupt flag. This is a side effect of reading the TC SR.
	tc_read_sr(EXAMPLE_TC, EXAMPLE_TC_CHANNEL);

	if (is_button_released() == true)
	{
		Read_button();	
	}

	



	// specify that an interrupt has been raised
	update_timer = true;
	// Toggle the GPIO line
	gpio_tgl_gpio_pin(EXAMPLE_TOGGLE_PIN);
}

bool is_button_released(void)
{
	if ((check_all_buttons_high() == true) && (button_released == false))
	{
		button_released = true;
	}
	return button_released;
}

void Read_button(void)
{
	uint8_t button_value = 0;
	
	button_value = button_pressed();
	switch (button_value)
	{
		case ENTER_BUTTON:
		{
			if (device_unlocked == true)
			{
				enter_pressed = true;
				calculate_salt();	
			}
			else
			{
				mode_selected = true;
			}
			
			break;
		}
		case NO_BUTTON:
		break;
		
		default:
		{
			if (device_unlocked == true)
			{
				store_passcode((unsigned long int)button_value);	
			}
			else
			{
				mode_chosen = button_value;
			}
		}
	}	

	if (!enter_pressed)
	{
		if (var_W_ticks++ > var_W)
		{
			calculate_salt();
		}
	}	
}

uint8_t button_pressed (void)
{
	if (button_released)
	{		
		if (read_push_button(PB1, (uint8_t *)&PB1_Counter))
		{
			LED_On(LED0);
			LED_Off(LED1 | LED2 | LED3);
			//store_passcode(0L);
			return PUSH_BUTTON1;
		}
		else if (read_push_button(PB2, (uint8_t *)&PB2_Counter))
		{
			LED_On(LED1);
			LED_Off(LED0 | LED2 | LED3);
			//store_passcode(1L);
			return PUSH_BUTTON2;
		}
		else if (read_push_button(PB3, (uint8_t *)&PB3_Counter))
		{
			LED_On(LED2);
			LED_Off(LED0 | LED1 | LED3);
			//store_passcode(2L);
			return PUSH_BUTTON3;
		}
		else if (read_push_button(PB4, (uint8_t *)&PB4_Counter))
		{
			LED_On(LED3);
			LED_Off(LED0 | LED1 | LED2);
			//store_passcode(3L);
			return PUSH_BUTTON4;
		}
		else if (read_push_button(PB_ENTER, (uint8_t *)&PB5_Counter))
		{
			device_unlocked = true;
			return ENTER_BUTTON;
		}
	}
	return NO_BUTTON;
}

bool check_all_buttons_high(void)
{
	static uint8_t button_released_iteration = 0;

	if ((gpio_get_pin_value(PB1) == 1) && (gpio_get_pin_value(PB2) == 1) && (gpio_get_pin_value(PB3) == 1) && (gpio_get_pin_value(PB4) == 1))
	{
		if (button_released_iteration++ > 10)
		{
			button_released_iteration = 0;
			return true;
		}
	}
	else
	{
		button_released_iteration = 0;
	}

	return false;
}

bool read_push_button(uint32_t pin, uint8_t *counter)
{
	bool button_status = false;

	if (gpio_get_pin_value(pin) == 0)
	{
		(*counter)++;
		if (*counter >= 3)
		{
			button_status = true;
			if (enter_pressed)
			{
				enter_pressed = false;	
				Start_W_timer();
			}
			
			button_released = false;
		}
	}
	else
	{
		button_status = false;
		*counter = 0;
	}
	return button_status;	
}

void push_buttons_init(void)
{
	gpio_configure_pin(PB1,GPIO_DIR_INPUT);
	gpio_configure_pin(PB2,GPIO_DIR_INPUT);
	gpio_configure_pin(PB3,GPIO_DIR_INPUT);
	gpio_configure_pin(PB4,GPIO_DIR_INPUT);
	gpio_configure_pin(PB_ENTER,GPIO_DIR_INPUT);
}

void store_passcode(uint32_t value)
{
	unsigned char index = 0;
	index = passcode_byte_index >> 4;
	pass_code[index] = pass_code[index] | (value << ((passcode_byte_index & 0x0000000F) << 1));
	if (passcode_byte_index++ > 128)
	{
		passcode_byte_index = 0;
	}
	//inter_key_delay = ENABLED;
}
/**
 * \brief TC Initialization
 *
 * Initializes and start the TC module with the following:
 * - Counter in Up mode with automatic reset on RC compare match.
 * - fPBA/8 is used as clock source for TC
 * - Enables RC compare match interrupt
 * \param tc Base address of the TC module
 */
static void tc_init(volatile avr32_tc_t *tc)
{

	// Options for waveform generation.
	static const tc_waveform_opt_t waveform_opt = {
		// Channel selection.
		.channel  = EXAMPLE_TC_CHANNEL,
		// Software trigger effect on TIOB.
		.bswtrg   = TC_EVT_EFFECT_NOOP,
		// External event effect on TIOB.
		.beevt    = TC_EVT_EFFECT_NOOP,
		// RC compare effect on TIOB.
		.bcpc     = TC_EVT_EFFECT_NOOP,
		// RB compare effect on TIOB.
		.bcpb     = TC_EVT_EFFECT_NOOP,
		// Software trigger effect on TIOA.
		.aswtrg   = TC_EVT_EFFECT_NOOP,
		// External event effect on TIOA.
		.aeevt    = TC_EVT_EFFECT_NOOP,
		// RC compare effect on TIOA.
		.acpc     = TC_EVT_EFFECT_NOOP,
		/* RA compare effect on TIOA.
		 * (other possibilities are none, set and clear).
		 */
		.acpa     = TC_EVT_EFFECT_NOOP,
		/* Waveform selection: Up mode with automatic trigger(reset)
		 * on RC compare.
		 */
		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		// External event trigger enable.
		.enetrg   = false,
		// External event selection.
		.eevt     = 0,
		// External event edge selection.
		.eevtedg  = TC_SEL_NO_EDGE,
		// Counter disable when RC compare.
		.cpcdis   = false,
		// Counter clock stopped with RC compare.
		.cpcstop  = false,
		// Burst signal selection.
		.burst    = false,
		// Clock inversion.
		.clki     = false,
		// Internal source clock 3, connected to fPBA / 8.
		.tcclks   = TC_CLOCK_SOURCE_TC3
	};

	// Options for enabling TC interrupts
	static const tc_interrupt_t tc_interrupt = {
		.etrgs = 0,
		.ldrbs = 0,
		.ldras = 0,
		.cpcs  = 1, // Enable interrupt on RC compare alone
		.cpbs  = 0,
		.cpas  = 0,
		.lovrs = 0,
		.covfs = 0
	};
	// Initialize the timer/counter.
	tc_init_waveform(tc, &waveform_opt);

	/*
	 * Set the compare triggers.
	 * We configure it to count every 10 milliseconds.
	 * We want: (1 / (fPBA / 8)) * RC = 10 ms, hence RC = (fPBA / 8) / 100
	 * to get an interrupt every 10 ms.
	 */
	tc_write_rc(tc, EXAMPLE_TC_CHANNEL, (sysclk_get_pba_hz() / 8 / 100));
	// configure the timer interrupt
	tc_configure_interrupts(tc, EXAMPLE_TC_CHANNEL, &tc_interrupt);
	// Start the timer/counter.
	tc_start(tc, EXAMPLE_TC_CHANNEL);
}

/**
 * \brief Main function
 *
 * Main function performs the following:
 * - Configure the TC Module
 * - Register the TC interrupt
 * - Configure, enable the CPCS (RC compare match) interrupt,
 * - and start a TC channel in waveform mode
 */
void tc_task (void)
{
	volatile avr32_tc_t *tc = EXAMPLE_TC;
	uint32_t timer = 0;
	/**
	 * \note the call to sysclk_init() will disable all non-vital
	 * peripheral clocks, except for the peripheral clocks explicitly
	 * enabled in conf_clock.h.
	 */
	//sysclk_init();
	// Enable the clock to the selected example Timer/counter peripheral module.
	sysclk_enable_peripheral_clock(EXAMPLE_TC);
	// Disable the interrupts
	cpu_irq_disable();
	// Initialize interrupt vectors.
	//INTC_init_interrupts();
	// Register the RTC interrupt handler to the interrupt controller.
	INTC_register_interrupt(&tc_irq, EXAMPLE_TC_IRQ, EXAMPLE_TC_IRQ_PRIORITY);
	// Enable the interrupts
	cpu_irq_enable();
	// Initialize the timer module
	tc_init(tc);
	// Initialize push buttons
	push_buttons_init();

    calculate_hash(CipherKey256, 8, CipherKey256_hash);
	//while(1) {
		//// Update the timer every 10 milli second.
		//if ((update_timer)) {
			//timer++;
			///*
			 //* TODO: Place a breakpoint here and watch the update of timer
			 //* variable in the Watch Window.
			 //*/
			 //// Reset the timer update flag to wait till next timer interrupt
			//update_timer = false;
		//}
	//}
}
