#ifndef SHELL_H
#define SHELL_H

void shell_print_help(void);
void shell_set_prompt(char *str);
void shell_on_char_in(char c);
void shell_on_data_in(uint8_t *data, uint16_t size);
void shell_init(void);

#endif

