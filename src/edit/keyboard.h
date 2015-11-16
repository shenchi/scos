#ifndef _KEYBOARD_H
#define _KEYBOARD_H

extern void init_keyboard(void);
extern unsigned char get_char(void);
extern unsigned char get_scancode(void);
extern unsigned long translate_scancode(unsigned long scancode);


#endif

