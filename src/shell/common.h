#ifndef _COMMON_H
#define _COMMON_H

extern void memfill(unsigned long addr,unsigned long size,unsigned char value);
extern void memcpy(unsigned long srcaddr,unsigned long dstaddr,unsigned long size);
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port,unsigned char byte);
extern void sti(void);
extern void cli(void);
extern unsigned long cmpstr(unsigned char* str1,unsigned char* str2);
extern unsigned long cmpstrl(unsigned char* str1,unsigned char* str2,unsigned long length);
extern void strcpyl(unsigned char* str1,unsigned char* str2,unsigned long length);
extern void strcpy(unsigned char* str1,unsigned char* str2);
extern void translate_filename(unsigned char* fsrc,unsigned char* fdst);
extern void translate_dirname(unsigned char * fsrc, unsigned char * fdst);
extern unsigned long str2num(char* str);
extern unsigned long gettasknum(void);
extern void stradd(unsigned char* str1,unsigned char* str2);


#endif

