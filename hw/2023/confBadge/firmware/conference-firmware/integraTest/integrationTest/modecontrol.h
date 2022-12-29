/*
 * modeControl.h
 *
 * Created: 2022-12-20 2:21:10 PM
 *  Author: lambert
 */ 


#ifndef MODECONTROL_H_
#define MODECONTROL_H_


void mode_currentLoading_assert();
void mode_currentLoading_deassert();
bool mode_getCurrentlyLoading();

void mode_waitForDisconnect_deassert();
void mode_waitForDisconnect_assert();
bool mode_getWaitForDisconnect();


#endif /* MODECONTROL_H_ */