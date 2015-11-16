//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "timer.h"
#include "screen.h"
#include "idt.h"
#include "task.h"

unsigned long sys_ticks;

extern void motor_timer(void);
void irq0(void);

void init_timer(unsigned int hz){
	unsigned int tmp=1193180/hz;
	set_interrupt_gate(0x20, 0, (unsigned long)&irq0);
	outb(0x43,0x36);
	outb(0x40,tmp&0xff);
	outb(0x40,(tmp>>8)&0xff);
	set_irq_mask(0, 0);
	sys_ticks=0;
}

void do_timer(unsigned long current_cs){
	sys_ticks++;
	motor_timer();
	if((current_cs&3)!=0)schedule();
}

void delay(unsigned long time){
	unsigned long sttime=sys_ticks;
	unsigned long ettime=sys_ticks-sttime;
	while(ettime*10<=time)ettime=sys_ticks-sttime;
}

