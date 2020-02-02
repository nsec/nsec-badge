//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef identity_h
#define identity_h

#include <string.h>

void init_identity_service(void);
struct VendorService *nsec_identity_get_service(void);

#endif /* identity_h */
