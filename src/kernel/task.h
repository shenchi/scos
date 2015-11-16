//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "gdt.h"

#ifndef _TASK_H
#define _TASK_H

#define task_none		0xFFFF;
#define task_free		0x0;
#define task_running	0x1;
#define task_sleep		0x2;

typedef struct{
	unsigned long backlink;
	unsigned long esp0;
	unsigned long ss0;
	unsigned long esp1;
	unsigned long ss1;
	unsigned long esp2;
	unsigned long ss2;
	unsigned long cr3;
	unsigned long eip;
	unsigned long eflags;
	unsigned long eax;
	unsigned long ecx;
	unsigned long edx;
	unsigned long ebx;
	unsigned long esp;
	unsigned long ebp;
	unsigned long esi;
	unsigned long edi;
	unsigned long es;
	unsigned long cs;
	unsigned long ss;
	unsigned long ds;
	unsigned long fs;
	unsigned long gs;
	unsigned long ldt_sel;
	unsigned long iomap;
} Task_State_Segment;

typedef struct{
	SEG_DESC code_seg;
	SEG_DESC data_seg;
	SEG_DESC stack_seg;
	SEG_DESC stack_seg0;
} Local_Desc_Table;

typedef struct{
	unsigned short	pid;
	unsigned char	pname[7];
	unsigned short	state;
	unsigned short	CPUTime;
	unsigned short	counter;
	Task_State_Segment	TSS;
	Local_Desc_Table	LDT;
} Task_Control_Block;

extern void init_task(void);
extern void create_task(char* filename,unsigned short task_num);
extern void schedule(void);
extern void create_task_for_ring3(unsigned long filenameaddr);
extern void exit_task_for_ring(unsigned long task_num);

extern void task_list(unsigned char* ret_buf);

extern unsigned short current;

#endif

