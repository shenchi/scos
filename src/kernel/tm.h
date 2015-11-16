//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#ifndef _TM_H
#define _TM_H

typedef struct{
	unsigned long vbuf_addr;
	unsigned long kbuf_addr;
} task_info;


extern task_info ti[64];
extern unsigned long cur_task;
unsigned long cur_vbuf;
unsigned long cur_kbuf;

extern void init_tm(void);

extern void set_cur_task(unsigned long task_no);

extern void set_cur_task_without_task_no(void);

extern void set_cur_vbuf(unsigned long addr);

extern void set_cur_kbuf(unsigned long addr);

extern unsigned long read_file_for_ring3(unsigned long filenameaddr,unsigned long addr);

extern unsigned long create_file_for_ring3(unsigned long filenameaddr,unsigned long addr,unsigned long size);

extern unsigned long create_dir_for_ring3(unsigned long filenameaddr,unsigned long addr,unsigned long size);

extern unsigned long change_dir_for_ring3(unsigned long filenameaddr);

extern void dir_for_ring3(unsigned long filenameaddr);

extern void task_list_for_ring3(unsigned long filenameaddr);

extern unsigned long delete_file_for_ring3(unsigned long filenameaddr);

#endif

