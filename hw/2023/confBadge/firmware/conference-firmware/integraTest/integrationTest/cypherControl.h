/*
 * cypherControl.h
 *
 * Created: 2022-12-21 10:06:11 AM
 *  Author: lambert
 */ 


#ifndef CYPHERCONTROL_H_
#define CYPHERCONTROL_H_

//KEY GENERATOR
uint32_t cypher_getKey(void);
void cypher_getGeneratedMultipliers(uint8_t arrayMultipliers[]);

//KEY VALIDATOR
bool cypher_checkIfValidKey(uint32_t input);
uint8_t cypher_getValidatedMultipliers(uint8_t arrayMultipliers[]);


#endif /* CYPHERCONTROL_H_ */