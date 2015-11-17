//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "tm.h"
#include "kernel.h"
#include "task.h"
#include "fs.h"
#include "screen.h"

task_info ti[64];
unsigned long cur_task=0;
unsigned long cur_vbuf=0xb8000;
unsigned long cur_kbuf=0;

void init_tm(void){
	unsigned long i;
	for(i=0;i<=64;i++){
		ti[i].kbuf_addr=0;
		ti[i].vbuf_addr=-1;
	}
	cur_task=0;
}

void set_cur_task(unsigned long task_no){
	cur_task=task_no;
	cur_vbuf=ti[cur_task].vbuf_addr;
	if(cur_vbuf==-1)cur_vbuf=0xb8000;
	cur_kbuf=ti[cur_task].kbuf_addr;
}

void set_cur_task_without_task_no(void){
	//puts("Set Task To ");puthex(current);puts("\n\r");
	cur_task=current;
	cur_vbuf=ti[cur_task].vbuf_addr;
	if(cur_vbuf==-1)cur_vbuf=0xb8000;
	cur_kbuf=ti[cur_task].kbuf_addr;
}

void set_cur_vbuf(unsigned long addr){
	if(current==0)return;
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	ti[current].vbuf_addr=baseaddr+addr;
	cur_vbuf=ti[cur_task].vbuf_addr;
	if(cur_vbuf==-1)cur_vbuf=0xb8000;
}

void set_cur_kbuf(unsigned long addr){
	if(current==0)return;
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	ti[current].kbuf_addr=baseaddr+addr;
	cur_kbuf=ti[cur_task].kbuf_addr;
}

unsigned long read_file_for_ring3(unsigned long filenameaddr,unsigned long addr){
	if(current==0)return 0;
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	unsigned long tmpret;
	tmpret=read_file((char*)(baseaddr+filenameaddr),baseaddr+addr);
	return tmpret;
}

unsigned long create_file_for_ring3(unsigned long filenameaddr,unsigned long addr,unsigned long size){
	if(current==0)return 0;
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	unsigned long tmpret;
	tmpret=create_file((char*)(baseaddr+filenameaddr), baseaddr+addr, size);
	return tmpret;
}

unsigned long create_dir_for_ring3(unsigned long filenameaddr,unsigned long addr,unsigned long size){
	if(current==0)return 0;
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	unsigned long tmpret;
	tmpret=create_dir((char*)(baseaddr+filenameaddr), baseaddr+addr, size);
	return tmpret;
}

unsigned long delete_file_for_ring3(unsigned long filenameaddr){
	if(current==0)return 0;
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	unsigned long tmpret;
	tmpret=delete_file((char*)(baseaddr+filenameaddr));
	return tmpret;
}

unsigned long change_dir_for_ring3(unsigned long filenameaddr){
	if(current==0)return 0;
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	unsigned long tmpret;
	tmpret=goto_dic((unsigned char*)(baseaddr+filenameaddr));
	return tmpret;
}

void dir_for_ring3(unsigned long filenameaddr){
	if(current==0)return;
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	dir((unsigned char*)filenameaddr+baseaddr);
}

void task_list_for_ring3(unsigned long filenameaddr){
	if(current==0)return;
	unsigned long baseaddr=((current-1)*64+32)*1024*1024;
	task_list((unsigned char*)filenameaddr+baseaddr);
}

