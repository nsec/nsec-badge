#include "application.h"

static application_t application = 0;

void application_clear() { application = 0; }

application_t application_get() { return application; }

void application_set(application_t callback) { application = callback; }
