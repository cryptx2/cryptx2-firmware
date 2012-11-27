/*
 * conf_user_settings.h
 *
 * Created: 11/20/2012 6:45:27 PM
 *  Author: mwaqas
 */ 


#ifndef CONF_USER_SETTINGS_H_
#define CONF_USER_SETTINGS_H_

#define SD_CARD_SLOT_0		ENABLE			//!< SD/MMC Card over MCI Slot 0.
#define SD_CARD_SLOT_1		ENABLE			//!< SD/MMC Card over MCI Slot 1.

#define UDI_MSC_GLOBAL_VENDOR_ID            \
	'C', 'R', 'Y', 'P', 'X', '2', ' ', ' '
#define UDI_MSC_GLOBAL_PRODUCT_VERSION            \
	'0', '.', '6', '9'


#define SD_CARD_0_NAME		"\"SD Card Slot 0\""
#define SD_CARD_1_NAME		"\"SD Card Slot 1\""

#define VENDOR_ID			0x03EB			//!< Assigned to ATMEL from USB org.
#define MANUFACTURE_NAME	"CryptX2"
#define PRODUCT_NAME		"CryptX2"
#define SERIAL_NUMBER		"696969696969"	// Disk SN for MSC

#endif /* CONF_USER_SETTINGS_H_ */