/*
 * cypherControl.cpp
 *
 * Created: 2022-12-21 10:05:53 AM
 *  Author: lambert
 */ 
#include <Arduino.h>
#include <stdio.h>
#include <math.h>

uint32_t prime1 = 11;
uint32_t prime2 = 79;
uint32_t prime3 = 109;
uint32_t prime4 = 1307;


//---------------------------------------
//KEY GENERATOR
uint8_t key_i = 0;
uint8_t key_j = 0;
uint8_t key_k = 0;
uint8_t key_l = 0;
uint32_t cypher_getKey()
{	
	key_i = random(1.9);
	key_j = random(1,6);
	key_k = random(1,5);
	key_l = random(1,3);
	uint32_t outputKey =	pow(prime1, key_i ) * \
							pow(prime2, key_j ) + \
							pow(prime3, key_k ) - \
							pow(prime4, key_l );
	return outputKey;
}

void cypher_getGeneratedMultipliers(uint8_t arrayMultipliers[])
{
	arrayMultipliers[0] = key_i;
	arrayMultipliers[1] = key_j;
	arrayMultipliers[2] = key_k;
	arrayMultipliers[3] = key_l;
}

//---------------------------------------------
//KEY VALIDATOR
uint8_t cyp_i = 0;
uint8_t cyp_j = 0;
uint8_t cyp_k = 0;
uint8_t cyp_l = 0;
bool cypher_checkIfValidKey(uint32_t input)
{
	cyp_i = 0;
	cyp_j = 0;
	cyp_k = 0;
	cyp_l = 0;
	for(int i=1; i<=8; i++)
	{
		for(int j=1; j<=5; j++)
		{
			for(int k=1; k<=4; k++)
			{
				for(int l=1; l<=2; l++)
				{
					uint32_t result =	pow(prime1, i ) * \
										pow(prime2, j ) + \
										pow(prime3, k ) - \
										pow(prime4, l );
					if(result == input)
					{
						cyp_i = i;
						cyp_j = j;
						cyp_k = k;
						cyp_l = l;
						return true;
					}
				}
			}
		}
	}	
	return false;
}

void cypher_getValidatedMultipliers(uint8_t arrayMultipliers[])
{
	arrayMultipliers[0] = cyp_i;
	arrayMultipliers[1] = cyp_j;
	arrayMultipliers[2] = cyp_k;
	arrayMultipliers[3] = cyp_l;
}