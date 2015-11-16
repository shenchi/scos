#ifndef _SCREEN_H
#define _SCREEM_H

extern void init_screen(void);
extern void setcharattr(unsigned char data);
extern void refresh(void);
extern void putchar(unsigned char c,unsigned long x,unsigned long y);
extern void putch(unsigned char c);
extern void putstr(char *str);
extern void clear(void);
extern void puthex(unsigned long hex);
extern unsigned char getchar(unsigned long x,unsigned long y);

#endif

