//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "idt.h"

#define idt_addr 0x1000

void set_trap_gate(unsigned short index,unsigned short dpl,unsigned long intfun);
void set_interrupt_gate(unsigned short index,unsigned short dpl,unsigned long intfun);
void init_interrupt();
void set_irq_mask(unsigned short index,unsigned char flag);
void setup_sys_call(void);

void isr0(void);
void isr1(void);
void isr2(void);
void isr3(void);
void isr4(void);
void isr5(void);
void isr6(void);
void isr7(void);
void isr8(void);
void isr9(void);
void isr10(void);
void isr11(void);
void isr12(void);
void isr13(void);
void isr14(void);
void isr15(void);
void isr16(void);
void isr17(void);
void isr18(void);
void isr19(void);
void irq1(void);
void isr0x40(void);

void set_trap_gate(unsigned short index,unsigned short dpl,unsigned long intfun){
	IDT_GATE trap_gate;
	IDT_GATE* pgate=(IDT_GATE*)(idt_addr+index*8);
	trap_gate.attr=0x8F00|(dpl<<13);
	trap_gate.offset0=intfun&0xFFFF;
	trap_gate.offset1=(intfun>>16)&0xFFFF;
	trap_gate.segsel=8;
	*pgate=trap_gate;
	return;
}

void set_interrupt_gate(unsigned short index,unsigned short dpl,unsigned long intfun){
	IDT_GATE intr_gate;
	IDT_GATE* pgate=(IDT_GATE*)(idt_addr+index*8);
	intr_gate.attr=0x8E00|(dpl<<13);
	intr_gate.offset0=intfun&0xFFFF;
	intr_gate.offset1=(intfun>>16)&0xFFFF;
	intr_gate.segsel=8;
	*pgate=intr_gate;
	return;
}

void init_interrupt(){
	set_interrupt_gate(0,0,(unsigned long)&isr0);
	set_interrupt_gate(1,0,(unsigned long)&isr1);
	set_interrupt_gate(2,0,(unsigned long)&isr2);
	set_interrupt_gate(3,0,(unsigned long)&isr3);
	set_interrupt_gate(4,0,(unsigned long)&isr4);
	set_interrupt_gate(5,0,(unsigned long)&isr5);
	set_interrupt_gate(6,0,(unsigned long)&isr6);
	set_interrupt_gate(7,0,(unsigned long)&isr7);
	set_interrupt_gate(8,0,(unsigned long)&isr8);
	set_interrupt_gate(9,0,(unsigned long)&isr9);
	set_interrupt_gate(10,0,(unsigned long)&isr10);
	set_interrupt_gate(11,0,(unsigned long)&isr11);
	set_interrupt_gate(12,0,(unsigned long)&isr12);
	set_interrupt_gate(13,0,(unsigned long)&isr13);
	set_interrupt_gate(14,0,(unsigned long)&isr14);
	set_interrupt_gate(15,0,(unsigned long)&isr15);
	set_interrupt_gate(16,0,(unsigned long)&isr16);
	set_interrupt_gate(17,0,(unsigned long)&isr17);
	set_interrupt_gate(18,0,(unsigned long)&isr18);
	set_interrupt_gate(19,0,(unsigned long)&isr19);
	set_interrupt_gate(0x21,0,(unsigned long)&irq1);
	set_irq_mask(1,0);
	return;
}

void set_irq_mask(unsigned short index,unsigned char flag){
	unsigned char tmp;
	unsigned short t2;
	if(index<=7){
		t2=index;
		tmp=inb(0x21);
	}else{
		t2=index-8;
		tmp=inb(0xA1);
	}
	if(flag==1)tmp|=(1<<t2);
	if(flag==0)tmp&=~(1<<t2);
	if(index<=7){
		outb(0x21,tmp);
	}else{
		outb(0xA1,tmp);
	}
}

void setup_sys_call(void){
	set_trap_gate(0x40,3,(unsigned long)&isr0x40);
}


