/*
 * conf_sched.h
 *
 *  Created on: May 5, 2019
 *      Author: simark
 */

#ifndef SRC_APP_CONF_SCHED_H_
#define SRC_APP_CONF_SCHED_H_

struct talk {
    const char *title, *names, *abstract, *detailed;
    unsigned char start_h, start_m;
    unsigned char end_h, end_m;
    unsigned char track;
};

struct speaker {
    const char *name, *bio;
};

/* These are defined in conf_sched.inc.  */
extern const struct talk talks_16[];
extern const struct talk talks_17[];
extern const int num_talks_16;
extern const int num_talks_17;

extern const struct speaker speakers[];
extern const int num_speakers;

extern const char *const tracks[];

#endif /* SRC_APP_CONF_SCHED_H_ */
