//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "task.h"
#include "kernel.h"
#include "mem.h"
#include "timer.h"
#include "tm.h"
//#include "gdt.h"   at task.h

Task_Control_Block *TCB=(Task_Control_Block*)0x600000;
Task_State_Segment Kernel_TSS;

unsigned char task_state[64];

unsigned short current=0;
unsigned short new_task=0;

unsigned long gloi,gloj,gloreti=0;

void idle(void){
	puts("IDLE Running!\n\r");
	//__asm__("1:mov $65,%ebx;mov $0,%ah;mov $1,%al;int $0x40;mov $0xFFFF,%ecx;2:loop 2b;jmp 1b");
	for(;;);
}

void init_task0(void){
	Task_Control_Block *tmptcb=(Task_Control_Block*)TCB;
	unsigned char *aname="IDLE    ";
	(*tmptcb).CPUTime=5;
	(*tmptcb).counter=0;
	memcpy((long)aname,(long)&((*tmptcb).pname[0]),8);
	(*tmptcb).state=task_running;
	task_state[0]=task_running;
	create_seg_desc((long)&((*tmptcb).LDT.code_seg), 0, 0x2000,1, 1, 3, 0xA);
	create_seg_desc((long)&((*tmptcb).LDT.data_seg), 0, 0x2000,1, 1, 3, 0x2);
	create_seg_desc((long)&((*tmptcb).LDT.stack_seg), 0, 0x2000,1, 1, 3, 0x2);
	create_seg_desc((long)&((*tmptcb).LDT.stack_seg0), 0, 0x2000,1, 1, 0, 0x2);
	set_seg_desc(4, (long)&((*tmptcb).LDT), 32, 0, 0, 0, 2);
	__asm__("lldt %%bx;mov %%cr3,%0":"=a"((*tmptcb).TSS.cr3):"b"(4<<3));
	(*tmptcb).TSS.ds=0xF;
	(*tmptcb).TSS.es=0xF;
	(*tmptcb).TSS.ss=0x17;
	(*tmptcb).TSS.esp=0xFFFF;
	(*tmptcb).TSS.fs=0xF;
	(*tmptcb).TSS.gs=0xF;
	(*tmptcb).TSS.cs=0x7;
	(*tmptcb).TSS.eip=&idle;
	(*tmptcb).TSS.ss0=0x1C;
	(*tmptcb).TSS.esp0=0x9FFFF;
	(*tmptcb).TSS.eflags=0x202|(3<<12);
	(*tmptcb).TSS.ldt_sel=4<<3;
	set_tss_desc(5, (long)&((*tmptcb).TSS), 0);

	__asm__("mov %%cr3,%0":"=a"(Kernel_TSS.cr3):"b"(4<<3));
	Kernel_TSS.cs=0x8;
	Kernel_TSS.ds=0x10;
	Kernel_TSS.es=0x10;
	Kernel_TSS.fs=0x10;
	Kernel_TSS.gs=0x10;
	Kernel_TSS.ss=0x10;
	Kernel_TSS.eflags=0x202|(3<<12);
	Kernel_TSS.ss0=0x10;
	set_tss_desc(3,(long)&Kernel_TSS,0);
	return;
}

void init_task(void){
	Task_Control_Block *tmptcb=(Task_Control_Block*)TCB;
	unsigned long i;
	for(i=0;i<=64;i++){
		memfill((long)tmptcb,144,0);
		(*tmptcb).pid=i;
		(*tmptcb).state=0xFFFF;
		task_state[i]=0xFF;
		tmptcb++;
	}
	init_task0();
}

void create_task(char* filename,unsigned short task_num){
	Task_Control_Block *tmptcb=(Task_Control_Block*)TCB;
	unsigned char tmpstate=task_state[task_num];
	unsigned long startaddr=((task_num-1)*64+32)*1024*1024;
	tmptcb+=task_num;
	if(tmpstate!=0xFF)return;
	read_file(filename, startaddr);
	/*if(task_num==2){
		puthex(*(unsigned long*)startaddr);
		puthex(*(unsigned long*)startaddr+1);
		for(;;);
	}*/
	memcpy((long)&((*TCB).TSS),(long)&((*tmptcb).TSS),104);
	(*tmptcb).CPUTime=5;
	(*tmptcb).counter=0;
	memcpy((long)filename,(long)&((*tmptcb).pname[0]),8);
	create_seg_desc((long)&((*tmptcb).LDT.code_seg), startaddr, 0x4000,1, 1, 3, 0xA);
	create_seg_desc((long)&((*tmptcb).LDT.data_seg), startaddr, 0x4000,1, 1, 3, 0x2);
	create_seg_desc((long)&((*tmptcb).LDT.stack_seg), startaddr, 0x4000,1, 1, 3, 0x2);
	create_seg_desc((long)&((*tmptcb).LDT.stack_seg0), 0, (startaddr>>12)+0x4000,1, 1, 0, 0x2);
	memfill(startaddr+0x3FF8000, 0x8000, 0);
	set_seg_desc(task_num*2+4, (long)&((*tmptcb).LDT), 32, 0, 0, 0, 2);
	__asm__("mov %%cr3,%0":"=a"((*tmptcb).TSS.cr3));
	(*tmptcb).TSS.ds=0xF;
	(*tmptcb).TSS.es=0xF;
	(*tmptcb).TSS.ss=0x17;
	(*tmptcb).TSS.fs=0xF;
	(*tmptcb).TSS.gs=0xF;
	(*tmptcb).TSS.cs=0x7;
	(*tmptcb).TSS.ss0=0x1C;
	(*tmptcb).TSS.esp=0x3FFF+0x3FF8000;
	(*tmptcb).TSS.eip=0;
	(*tmptcb).TSS.esp0=0x3FFF+0x3FFC000+startaddr;
	(*tmptcb).TSS.eflags=0x202|(3<<12);
	(*tmptcb).TSS.ldt_sel=(task_num*2+4)<<3;
	set_tss_desc(task_num*2+5, (long)&((*tmptcb).TSS), 0);
	(*tmptcb).state=task_free;
	task_state[task_num]=task_free;
}

void switchto(unsigned long seg_seg);

unsigned long select_new_task(void){
	gloreti=0;
	if(current==0){
		gloj=1;
	}else{
		gloj=current;
	}
	for(gloi=0;gloi<=64;gloi++){
		if((task_state[gloj]==0xf0)&&(gloj!=current)){
			exit_task(gloj);
			task_state[gloj]=0xff;
		}
		if(task_state[gloj]==0){
			gloreti=gloj;
			break;
		}
		gloj++;
		if(gloj>64)gloj=1;
	}
	//puts("selected!!\n\r");
	return gloreti;
}

void schedule(void){
	Task_Control_Block* tmptcb=(Task_Control_Block*)TCB;
	tmptcb+=current;
	(*tmptcb).counter++;
	if((*tmptcb).counter>=(*tmptcb).CPUTime){
		(*tmptcb).counter=0;
		//puts("\n\rSelecting new task...\n\r");
		new_task=select_new_task();
		if(new_task==current)return;
		if(task_state[current]!=0xf0){
			(*tmptcb).state=task_free;
			task_state[current]=task_free;
		}
		tmptcb=(Task_Control_Block*)TCB;
		tmptcb+=new_task;
		if(new_task!=0){
			(*tmptcb).state=task_running;
			task_state[new_task]=task_running;
		}
		current=new_task;
		//puts("switch to ");puthex(new_task);puts(" task_NO is ");puthex((new_task*2+5)<<3);puts("\n\r");
		switchto((new_task*2+5)<<3);
	}
}


void create_task_for_ring3(unsigned long filenameaddr){
	unsigned long i=0;
	Task_Control_Block* tmptcb=(Task_Control_Block*)TCB;
	if(current==0)return;
	for(i=0;i<=65;i++){
		if(i==65)return;
		if(task_state[i]==0xFF)break;
		tmptcb++;
	}
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	allocate_for_task(i);
	create_task((char*)(baseaddr+filenameaddr),i);
}

void exit_task_for_ring(unsigned long task_num){
	task_state[task_num]=0xf0;
}

void exit_task(unsigned long task_num){ 
	if(task_num<=1)return;
	Task_Control_Block *tmptcb=(Task_Control_Block*)TCB;
	task_state[task_num]=0xff;
	tmptcb+=task_num;
	(*tmptcb).state=0xffff;
	free_for_task(task_num);
	ti[task_num].kbuf_addr=0;
	ti[task_num].vbuf_addr=0xb8000;
	memfill((unsigned long)tmptcb, sizeof(Task_Control_Block), 0); 
}

void thex(unsigned long hex,unsigned char *rebuf){
	unsigned char tmp;
	unsigned char i;
	for(i=0;i<8;i++){
		tmp=(hex&0xf0000000)>>28;
		if((tmp<=9)&&(tmp>=0))tmp+=48;
		if((tmp>=10)&&(tmp<=15))tmp+=55;
		*rebuf=tmp;rebuf++;
		hex<<=4;
	}
	*rebuf=0;
}

void task_list(unsigned char* ret_buf){
	Task_Control_Block *tmptcb=(Task_Control_Block*)TCB;
	unsigned char* tmptaskname="A Task N   \n";
	unsigned char* numbuf="        ";
	unsigned long i;
	for(i=0;i<64;i++){
		if((task_state[i]==0)||(task_state[i]==1)){
			stradd("Task ", ret_buf);
			thex(i, numbuf);
			stradd(numbuf, ret_buf);stradd(": ", ret_buf);
			strcpy(tmptaskname,(*tmptcb).pname,8);
			stradd(tmptaskname, ret_buf);
		}
		tmptcb++;
	}
}

