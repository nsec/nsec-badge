/*
 * levelControl.cpp
 *
 * Created: 2022-12-20 1:59:20 PM
 *  Author: lambert
 */ 

#include <Arduino.h>

uint8_t currentLevel = 0;
uint8_t level_getCurrLvl()
{
	return currentLevel;
}

void level_decrement()
{
	currentLevel--;
}

void level_increment()
{
	currentLevel++;
}