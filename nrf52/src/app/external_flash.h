#ifndef SRC_APP_EXTERNAL_FLASH_H_
#define SRC_APP_EXTERNAL_FLASH_H_

#if defined(NSEC_FLAVOR_CTF) && !defined(SOLDERING_TRACK)
#include "external_flash_ctf.h"
#elif defined(NSEC_FLAVOR_CTF) && defined(SOLDERING_TRACK)
#include "external_flash_soldering.h"
#elif defined(NSEC_FLAVOR_SPEAKER)
#include "external_flash_speaker.h"
#elif defined(NSEC_FLAVOR_ADMIN)
#include "external_flash_admin.h"
#elif defined(NSEC_FLAVOR_CONF)
#include "external_flash_conf.h"
#elif defined(NSEC_FLAVOR_TRAINER)
#include "external_flash_trainer.h"
#elif defined(NSEC_FLAVOR_BAR)
#include "external_flash_bar_beacon.h"
#endif

#endif /* SRC_APP_EXTERNAL_FLASH_H_ */
