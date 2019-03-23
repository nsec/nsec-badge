#ifndef APPLICATION_H
#define APPLICATION_H

typedef void (*application_t)(void (*service_callback)());

void application_clear();
application_t application_get();
void application_set(application_t callback);

#endif
