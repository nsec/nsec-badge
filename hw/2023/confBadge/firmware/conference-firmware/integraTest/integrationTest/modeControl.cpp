/*
 * modeControl.cpp
 *
 * Created: 2022-12-20 2:14:03 PM
 *  Author: lambert
 */ 

bool currentlyLoading = false;
void mode_currentLoading_assert()
{
	currentlyLoading = true;
}

void mode_currentLoading_deassert()
{
	currentlyLoading = false;
}

bool mode_getCurrentlyLoading()
{
	return currentlyLoading;
}



bool waitingForDisconnect = false;
void mode_waitForDisconnect_deassert()
{
	waitingForDisconnect = false;	
}

void mode_waitForDisconnect_assert()
{
	waitingForDisconnect = true;
}

bool mode_getWaitForDisconnect()
{
	return waitingForDisconnect;
}