//
//  identity.h
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2017-04-24.
//
//

#ifndef identity_h
#define identity_h

#include <string.h>

#define NSEC_IDENTITY_AVATAR_WIDTH 32
#define NSEC_IDENTITY_AVATAR_HEIGHT 32

void nsec_identitiy_init(void);
void nsec_identity_draw(void);
void nsec_identity_get_unlock_key(char * data, size_t length);
void nsec_identity_update_nearby(void);

#endif /* identity_h */
