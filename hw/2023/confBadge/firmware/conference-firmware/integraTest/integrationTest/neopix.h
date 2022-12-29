/*
 * neopix.h
 *
 * Created: 2022-12-09 2:22:56 AM
 *  Author: Lam
 */ 


#ifndef NEOPIX_H_
#define NEOPIX_H_

void neopix_init(void);
void neopix_idle(void);
void neopix_connectLeft(void);
void neopix_connectRight(void);
void incrementLoadingBar(void);
void neopix_success(void);
void neopix_resetLoadingBar(void);

#endif /* NEOPIX_H_ */