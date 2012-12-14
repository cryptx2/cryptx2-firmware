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
#include <string.h>

#include "tc.h"
#include "sysclk.h"
#include "board.h"
#include "gpio.h"
#include "intc.h"
#include "conf_example.h"
#include "sha256.h"
#include "Salt.h"
#include "tc_task.h"
#include "Utils.h"


#if !defined(EXAMPLE_TC) || !defined(EXAMPLE_TC_IRQ)
#  error The TC preprocessor configuration to use in this example is missing.
#endif

#if !defined(EXAMPLE_TOGGLE_PIN)
#  error The preprocessor configuration of the GPIO pin to toggle is missing.
#endif
//! @}

#define PB1			AVR32_PIN_PA10
#define PB2			AVR32_PIN_PA11
#define PB3			AVR32_PIN_PA12
#define PB4			AVR32_PIN_PA13
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
void Read_button(void);
bool read_push_button(uint32_t pin, uint8_t *counter);
void store_passcode(uint32_t value);
void push_buttons_init(void);
bool check_all_buttons_high(void);
void check_for_mode_selected(void);
bool compare_entered_passwords(void);
bool compare_with_saved_password(void);
uint8_t process_selected_mode(void);
void read_password(void);
void store_sequence(uint8_t value);
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
volatile uint8_t mode_chosen = 0xff;
volatile bool mode_selected = false;
volatile bool normal_mode_password_entered = false;
volatile uint8_t enter_button_status = READY_TO_TRIGGER;

volatile uint8_t normal_mode_chosen = NO_MODE_SELECTED;
volatile uint8_t entry_mode_status = NO_MODE_SELECTED;
volatile uint8_t failed_attempts_counter = 0;

unsigned long int CipherKey256_hash[8] = {0};

volatile uint8_t PB1_Counter = 0;
volatile uint8_t PB2_Counter = 0;
volatile uint8_t PB3_Counter = 0;
volatile uint8_t PB4_Counter = 0;
volatile uint8_t PB5_Counter = 0;

//volatile uint8_t led_on_time[LED_COUNT] = {0};

bool check_programming_mode_entry_sequence(void);
bool check_normal_mode_entry_sequence(void);
bool is_button_released(void);
void update_leds(void);
void check_to_toggle_read_only_mode(void);
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

	if (entry_mode_status == NO_MODE_SELECTED)
	{
		if (check_programming_mode_entry_sequence() == true)
		{
			entry_mode_status = PROGRAMMING_MODE;
		}
		else if (check_normal_mode_entry_sequence() == true)
		{
			entry_mode_status = NORMAL_MODE;
			pass_code = temp_password;
		}
	}
	
	if (entry_mode_status == PROGRAMMING_MODE)
	{
		if (!mode_selected)
		{
			check_for_mode_selected();
		}
		else
		{
			if (process_selected_mode() == SUCCESSFUL)
			{
				entry_mode_status = NO_MODE_SELECTED;
			}
		}
	}

	else if (entry_mode_status == NORMAL_MODE)
	{
		read_password();
	}
	
	check_to_toggle_read_only_mode();

	//if (is_button_released() == true)
	//{
		//Read_button();	
	//}

	update_leds();



	// specify that an interrupt has been raised
	update_timer = true;
	// Toggle the GPIO line
	gpio_tgl_gpio_pin(EXAMPLE_TOGGLE_PIN);
}

void check_to_toggle_read_only_mode(void)
{
	if (stSystemStatus.unlock_password_status == 1)
	{
		uint8_t button_value = button_pressed();
		if (button_value == PUSH_BUTTON1)
		{
			stSystemStatus.read_only_mode_0_status ^= 1;
		}
		else if (button_value == PUSH_BUTTON2)
		{
			stSystemStatus.read_only_mode_1_status ^= 1;
		}
	}
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
			if (entry_mode_status == PROGRAMMING_MODE)
			{
				if (mode_selected)
				{
					enter_button_status++;
				}
			}
			else if (entry_mode_status == NORMAL_MODE && normal_mode_password_entered == false)
			{
				normal_mode_password_entered = true;
			}
			break;
		}
		case NO_BUTTON:
		break;
		
		default:
		{
			if (entry_mode_status == PROGRAMMING_MODE)
			{
				if (mode_selected)
				{
					switch (mode_chosen)
					{
					case _UNLOCK_PASSWORD:
						store_passcode((uint32_t)button_value);
						break;
					case _DEVICE_ID_CONFIRM:
						if (enter_button_status == WAITING_FOR_USER_INPUT)
						{
							store_sequence(button_value);
						}
						else
						{
							store_passcode((uint32_t)button_value);
						}
						break;
					}
					
				}
				else
				{
					mode_chosen = button_value;
					mode_selected = true;
				}
			}
			else if (entry_mode_status == NORMAL_MODE)
			{
				store_passcode((uint32_t)button_value);
			}

		}
	}	

	if (mode_chosen == _UNLOCK_PASSWORD && enter_button_status == WAITING_FOR_USER_INPUT)
	{
		if (var_W_ticks++ > var_W)
		{
			calculate_salt();
		}
	}	
}

uint8_t button_pressed (void)
{
	
	is_button_released();

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

	if ((gpio_get_pin_value(PB1) == 1)
		&& (gpio_get_pin_value(PB2) == 1)
		&& (gpio_get_pin_value(PB3) == 1)
		&& (gpio_get_pin_value(PB4) == 1)
		&& (gpio_get_pin_value(ENTER_BUTTON) == 1))
	{
		if (button_released_iteration++ > 4)
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
			//if (enter_pressed)
			//{
				//enter_pressed = false;	
				//Start_W_timer();
			//}
			*counter = 0;
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
	if (++passcode_byte_index >= 128)
	{
		passcode_byte_index = 0;
	}
	//inter_key_delay = ENABLED;
}

void store_sequence(uint8_t value)
{
	static uint8_t frame_number = 0;
	uint8_t temp1, temp2, temp3;
	uint8_t shift_value = value * 2;
	uint8_t button_bit_mask = 0x03 << shift_value;
	uint8_t increment_bit_mask = 0x01 << shift_value;
	
	switch (value)
	{
	case 0:
	case 1:
	case 2:
		temp1 = Stored_values_ram.device_id_sequence[frame_number] & button_bit_mask;
		temp1 = temp1 + increment_bit_mask;
		temp1 = temp1 & button_bit_mask;
		LED_Off(0x0000000f);
		if (temp1)
		{
			uint32_t led_bit_mask = 0;
			led_bit_mask = ((uint32_t)1L << (value * 3)) << ((temp1 >> shift_value) - 1);
			LED_On(led_bit_mask);
		}				
		temp2 = Stored_values_ram.device_id_sequence[frame_number] & ~button_bit_mask;
		//temp2 = ~temp2;
		temp3 = temp1 | temp2;
		Stored_values_ram.device_id_sequence[frame_number] = temp3;
		break;
	case 3:
		Stored_values_ram.device_id_sequence[frame_number++] |= 0xC0;
		break;
	}	
}

bool check_programming_mode_entry_sequence(void)
{
	static uint16_t programming_mode_sequence_counter = 0;
	if (gpio_get_pin_value(PB1) == 0
		&& gpio_get_pin_value(PB3) == 0
		&& gpio_get_pin_value(PB4) == 0
		&& gpio_get_pin_value(PB2) == 1
		&& gpio_get_pin_value(ENTER_BUTTON) == 1)
	{
		if (programming_mode_sequence_counter++ >= 100)
		{
			programming_mode_sequence_counter = 0;
			button_released = false;
			return true;
		}
	}
	else
	{
		programming_mode_sequence_counter = 0;
	}
	return false;
}

bool check_normal_mode_entry_sequence(void)
{
	static uint8_t normal_mode_device_id_sequence_counter = 0;
	static uint8_t normal_mode_unlock_device_sequence_counter = 0;
	
	if (gpio_get_pin_value(ENTER_BUTTON) == 0 
		&& gpio_get_pin_value(PB4) == 0
		&& gpio_get_pin_value(PB1) == 1
		&& gpio_get_pin_value(PB2) == 1
		&& gpio_get_pin_value(PB3) == 1)
	{
		normal_mode_unlock_device_sequence_counter = 0;
		if (normal_mode_device_id_sequence_counter++ >= 200)
		{
			normal_mode_device_id_sequence_counter = 0;
			button_released = false;
			normal_mode_chosen = DEVICE_ID;
			return true;
		}
	}
	else if (gpio_get_pin_value(ENTER_BUTTON) == 0
			&& gpio_get_pin_value(PB2) == 0
			&& gpio_get_pin_value(PB1) == 1
			&& gpio_get_pin_value(PB3) == 1
			&& gpio_get_pin_value(PB4) == 1)
	{
		normal_mode_device_id_sequence_counter = 0;
		if (normal_mode_unlock_device_sequence_counter++ >= 200)
		{
			normal_mode_unlock_device_sequence_counter = 0;
			button_released = false;
			normal_mode_chosen = UNLOCK_CRYPTX2;
			return true;
		}
	}
	else
	{
		normal_mode_device_id_sequence_counter = 0;
		normal_mode_unlock_device_sequence_counter = 0;
	}
	return false;
}

void check_for_mode_selected(void)
{
	Read_button();
}

uint8_t process_selected_mode(void)
{
	uint8_t return_value = FAILED;
	
	
	
	if (enter_button_status == READY_TO_TRIGGER)
	{
		pass_code = temp_password;
		passcode_byte_index = 0;
		enter_button_status = WAITING_FOR_FIRST_PRESS;
	}
	else if (enter_button_status == FIRST_TIME_PRESSED)
	{
		store_passcode(3L);
		pass_code = temp_password1;
		passcode_byte_index = 0;
		enter_button_status = WAITING_FOR_SECOND_PRESS;
	}
	else if (enter_button_status == SECOND_TIME_PRESSED)
	{
		store_passcode(3L);
		if (compare_entered_passwords() == true)
		{
			memset((uint8_t *)temp_password, 0, 32);
			pass_code = temp_password;
			passcode_byte_index = 0;
			enter_button_status = WAITING_FOR_USER_INPUT;
			switch (mode_chosen)
			{
			case _UNLOCK_PASSWORD:
				Start_W_timer();			
				break;
			case _HIDDEN_DATA_UNLOCK_PASSWORD:
				break;
			case _PANIC_MODE:
				break;
			case _DEVICE_ID_CONFIRM:
				memset((uint8_t *)Stored_values_ram.device_id_sequence, 0, 32);
				break;	
			}

		}
	}
	else if (enter_button_status == THIRD_TIME_PRESSED)
	{
		
		switch (mode_chosen)
		{
		case _UNLOCK_PASSWORD:
			store_passcode(3L);
			calculate_salt();
			break;
		case _HIDDEN_DATA_UNLOCK_PASSWORD:
			break;
		case _PANIC_MODE:
			break;
		case _DEVICE_ID_CONFIRM:
			save_sequence_to_mcu();
			break;
		}
		
		return_value = SUCCESSFUL;
	}

	Read_button();
	
	return return_value;
}

bool compare_entered_passwords(void)
{
	uint8_t i = 0;
	//uint32_t *temp_entered_password = &password_block[PASSWORD_SIZE * mode_chosen];
	while (i < 8)
	{
		if (temp_password[i] != temp_password1[i])
		{
			return false;
		}
		i++;
	}
	return true;
}

void read_password(void)
{
	Read_button();
	if (normal_mode_password_entered == true)
	{
		if (compare_with_saved_password() == true)
		{
			stSystemStatus.unlock_password_status = 1;
		}
	}
}

bool compare_with_saved_password(void)
{
	uint8_t i = 0; 
	uint32_t temp_digest[8], *temp_stored_password;

	if (normal_mode_chosen == DEVICE_ID)
	{
		hash_the_password(temp_password, temp_digest);
		temp_stored_password = Stored_values_ram.device_id_confirm;
	}
	else if (normal_mode_chosen == UNLOCK_CRYPTX2)
	{
		encrypt_password(temp_password, temp_digest);
		temp_stored_password = (uint32_t *)Stored_values_ram.unlock_password;
	}
	i = 0;
	while (i < 8)
	{
		if (temp_stored_password[i] != temp_digest[i])
		{
			return false;
		}
		i++;
	}
	return true;
}

#define		SD1				0
#define		MODE			1
#define		SD2				2

void update_leds(void)
{
	static uint8_t led_on_time = 0;
	static uint8_t led_off_time = 0;
	static uint8_t led_bit_mask = 0;
	static uint8_t current_bit_mask = 3;
	static uint8_t device_id_byte = 0;
	static uint8_t device_id_byte_index = 0;
	
	if (led_on_time && led_off_time)
	{
		LED_On(led_bit_mask);
		led_on_time--;
	}
	else if (led_on_time == 0 && led_off_time)
	{
		LED_Off(led_bit_mask);
		led_off_time--;
	}
	else
	{
		led_bit_mask = 0;
		switch (current_bit_mask)
		{
			case SD1:
				if (device_id_byte & 0x03)
				{
					led_bit_mask = 1 << ((device_id_byte & 0x03) + 0);
					led_on_time = 50;
					led_off_time = 10;		
				}
				current_bit_mask++;			
				break;
			case MODE:
				device_id_byte >>= 2;
				if (device_id_byte & 0x03)
				{
					led_bit_mask = 1 << ((device_id_byte & 0x03) + 3);
					led_on_time = 50;
					led_off_time = 10;
					current_bit_mask++;
				}				
				break;
			case SD2:
				device_id_byte >>= 4;
				if (device_id_byte & 0x03)
				{
					led_bit_mask = 1 << ((device_id_byte & 0x03) + 6);
					led_on_time = 50;
					led_off_time = 10;					
					current_bit_mask++;
				}
				break;
			default:
			{
				device_id_byte = Stored_values_ram.device_id_sequence[device_id_byte_index++];
				current_bit_mask = SD1;
			}
		}
	}
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
	int temp_value;
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
		// Internal source clock 3, connected to fPBA / 32.
		.tcclks   = TC_CLOCK_SOURCE_TC4
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
	temp_value = tc_write_rc(tc, EXAMPLE_TC_CHANNEL, 20625 /*(sysclk_get_pba_hz() / 8 / 100)*/);
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
//	uint32_t timer = 0;
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

    calculate_hash((uint32_t *)CipherKey256, 8, CipherKey256_hash);
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
