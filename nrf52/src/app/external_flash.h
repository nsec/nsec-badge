#ifndef SRC_APP_EXTERNAL_FLASH_H_
#define SRC_APP_EXTERNAL_FLASH_H_

#if defined(NSEC_FLAVOR_CTF) && !defined(SOLDERING_TRACK)
#include "external_flash_ctf.h"
#elif defined(NSEC_FLAVOR_CTF) && defined(SOLDERING_TRACK)
#include "external_flash_soldering.h"
#elif defined(NSEC_FLAVOR_CONF)
#include "external_flash_conf.h"
#elif defined(NSEC_FLAVOR_BAR_BEACON)
#include "external_flash_bar_beacon.h"
#endif

#endif /* SRC_APP_EXTERNAL_FLASH_H_ */
