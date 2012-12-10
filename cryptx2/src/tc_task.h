/*
 * tc_task.h
 *
 * Created: 22/Nov/12 3:16:56 AM
 *  Author: Waqas
 */ 


#ifndef TC_TASK_H_
#define TC_TASK_H_

#define SUCCESSFUL							1
#define FAILED								0


#define NO_MODE_SELECTED					0
#define PROGRAMMING_MODE					1
#define NORMAL_MODE							2

#define DEVICE_ID							3
#define UNLOCK_CRYPTX2						4

#define _UNLOCK_PASSWORD					0
#define _HIDDEN_DATA_UNLOCK_PASSWORD		1
#define _PANIC_MODE							2
#define _DEVICE_ID_CONFIRM					3


#define READY_TO_TRIGGER					0
#define WAITING_FOR_FIRST_PRESS				1
#define FIRST_TIME_PRESSED					2
#define WAITING_FOR_SECOND_PRESS			3
#define SECOND_TIME_PRESSED					4
#define WAITING_FOR_USER_INPUT				5
#define THIRD_TIME_PRESSED					6


extern unsigned long int CipherKey256_hash[8];

extern volatile bool enter_pressed;
extern volatile uint8_t mode_chosen;
extern volatile bool mode_selected;
extern volatile uint8_t enter_button_status;

void tc_task (void);



#endif /* TC_TASK_H_ */