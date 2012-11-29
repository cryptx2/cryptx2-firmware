/*
 * tc_task.h
 *
 * Created: 22/Nov/12 3:16:56 AM
 *  Author: Waqas
 */ 


#ifndef TC_TASK_H_
#define TC_TASK_H_

extern unsigned long int CipherKey256_hash[8];

extern volatile bool enter_pressed;
extern volatile uint8_t mode_chosen;
extern volatile bool mode_selected;

void tc_task (void);



#endif /* TC_TASK_H_ */