//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#ifndef _IDT_H
#define _IDT_H

typedef volatile struct{
	unsigned short offset0;
	unsigned short segsel;
	unsigned short attr;
	unsigned short offset1;
} IDT_GATE;

extern void set_trap_gate(unsigned short index,unsigned short dpl,unsigned long intfun);
extern void set_interrupt_gate(unsigned short index,unsigned short dpl,unsigned long intfun);
extern void init_interrupt();
extern void set_irq_mask(unsigned short index,unsigned char flag);
extern void setup_sys_call(void);

#endif

