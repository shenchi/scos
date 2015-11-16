//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "mem.h"
#include "screen.h"
#include "kernel.h"

#define dic_table_addr 0x2000
#define page_table_addr 0x200000

unsigned long base_size;
unsigned long extend_size;
unsigned long mem_size;
unsigned long kernel_mem=32*1024*1024;

unsigned char *page_map=(unsigned char*)0x100000;


unsigned long globali;

void init_mm(void){
	unsigned long i,tmpitem;
	unsigned short* sizeptr=(unsigned short*)0x90000;
	base_size=*sizeptr;
	base_size=base_size*1024+0x100000;
	sizeptr++;
	extend_size=*sizeptr;
	extend_size=extend_size*1024*64;
	mem_size=base_size+extend_size;
	puts("Base Memory Size:    ");puthex(base_size);puts(" Bytes\n\r");
	puts("Extend Memory Size:  ");puthex(extend_size);puts(" Bytes\n\r");
	if(mem_size<32*1024*1024){
		puts("Memory is too small to run os!\n\r");
		for(;;);
	}
	puts("Paging Memory...");
	putchar('.');
	memfill((long)page_map,0x603,page_used);
	putchar('.');
	memfill((long)(page_map+0x603),(mem_size>>12)-0x603,page_free);
	putchar('.');
	memfill((long)(page_map+0x603+((mem_size>>12)-0x602)),0xFFFFF-(mem_size>>12)+0x604,page_none);
	putchar('.');
	for(i=0;i<=0x602;i++){
		tmpitem=(i<<12)|0x107|(1<<9);
		memcpy((long)&tmpitem,page_table_addr+(i*4),4);
	};
	putchar('.');
	for(i=0x603;i<=0xFFFFF;i++){
		tmpitem=(i<<12)|6;
		memcpy((long)&tmpitem,page_table_addr+(i*4),4);
	}
	putchar('.');
	for(i=0x200;i<=0x602;i++){
		tmpitem=(i<<12)|7;
		memcpy((long)&tmpitem,dic_table_addr+((i-0x200)*4),4);
	}
	putchar('.');
	*(page_map+0x1F00)=page_used;
	tmpitem=(0x1F00<<12)|0x107|(1<<9);
	memcpy((long)&tmpitem,page_table_addr+(0x1F00*4),4);
	putchar('.');
	__asm__("mov %0,%%cr3;mov %%cr0,%%eax;or $0x80000000,%%eax;mov %%eax,%%cr0"::"a"(dic_table_addr));
	__asm__("jmp 1f\n\t1: jmp 1f\n\t1:");
	puts("OK!\n\r");
}

unsigned long get_free_physics_page(void){
	for(globali=0x603;globali<=(mem_size>>12);globali++){
		if(*(page_map+globali)==page_free)break;
	}
	if(*(page_map+globali)!=page_free)return 0;
	return globali;
}

unsigned long get_free_page(unsigned long size){
	unsigned long firsti,lasti;
	unsigned long* tmpptr=(unsigned long*)page_table_addr;
	firsti=0;lasti=0;
	for(globali=0x603;globali<=0xFFFFF;globali++){
		if((*(tmpptr+globali)&0xE00)==0){
			if(firsti==0){
				firsti=globali;lasti=globali;
				if(lasti+1-firsti==size)break;
			}else{
				if(lasti+1==globali){
					lasti=globali;
					if(lasti+1-firsti==size)break;
				}else{
					firsti=0;
				}
			}
		}
	}
	if(lasti+1-firsti==size)return firsti;
	return 0;
}

void free_page(unsigned long page){
	memfill(page<<12,0x1000,0);
}

void no_page(unsigned long address,unsigned long errcode){
	unsigned long tmpaddr=address>>12;
	unsigned long* tmpptr=(unsigned long*)page_table_addr;
	if((address<0x100000)&&((errcode&1)==0)){
		puts("Memory Paging Error!!!");puthex(address);puts(" ");puthex(errcode);puts("\n\r");
		for(;;);
	}
	if((errcode&1)==1){
		puts("err at:");puthex(address);puts(" errcode ");puthex(errcode);puts("\n\r");
		for(;;);
	}
	if((*(tmpptr+tmpaddr)&0xE00)==0)return;
	//puts("A page fualt happened\n\r");
	unsigned long fr_p=get_free_physics_page();
	//puts("Put addr ");puthex(address);puts(" to page ");puthex(fr_p<<12);puts("\n\r");
	if(fr_p==0){
		puts("No Memory!! Sorry,SCOS 0.02 does not support virtual memory...\n\r");
		return;
	}
	*(page_map+fr_p)=(char)page_used;
	*(tmpptr+tmpaddr)=(*(tmpptr+tmpaddr)&0xFFF)|1|(fr_p<<12);
	//puts("set page from ");puthex(address&0xFFFFF000);puts(" to ");puthex(fr_p<<12);puts("\n\r");
	return;
}

unsigned long allocate(unsigned long size){
	unsigned long addr;
	unsigned long* tmpptr=(unsigned long*)page_table_addr;
	unsigned long i;
	//puts("Allocate ");puthex(size);puts(" page(s) ");
	addr=get_free_page(size);
	if((addr<<12)>=kernel_mem)return 0;
	if(addr==0){
		return 0;
	}
	for(i=addr;i<addr+size;i++){
		*(tmpptr+i)|=(1<<9);
	}
	addr<<=12;
	return addr;
}

void free(unsigned long page,unsigned long size){
	if(page<0x100)return;
	unsigned long* tmpptr=(unsigned long*)page_table_addr;
	for(globali=0;globali<size;globali++){
		*(tmpptr+page+globali)&=0x1FE;
		*(page_map+page+globali)=page_free;
		//free_page(page+globali);
	}
	return;
}

void allocate_for_task(unsigned long task_index){
	unsigned long tmpaddr=(task_index-1)*0x4000+(kernel_mem>>12);
	unsigned long* tmpptr=(unsigned long*)page_table_addr;
	if(task_index==0)return;
	for(globali=0;globali<0x4000;globali++){
		*(tmpptr+tmpaddr+globali)|=(1<<9);
	}
}

void free_for_task(unsigned long task_index){
	unsigned long i;
	unsigned long tmpaddr=(task_index-1)*0x4000+(kernel_mem>>12);
	if(task_index==0)return;
	for(i=0;i<0x4000;i++){
		free(tmpaddr+i,1);
	}
}

unsigned long get_phy_address(unsigned long laddr){
	unsigned long offset=laddr&0x1000;
	unsigned long paddr=(*((unsigned long*)(page_table_addr+(laddr>>12))))&0xFFFFF000;
	return (paddr+offset);
}

