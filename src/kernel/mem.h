//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#ifndef _MEM_H
#define _MEM_H

#define page_free	0x0
#define page_used	0x1
#define page_none	0xF

extern void init_mm(void);
extern unsigned long allocate(unsigned long size);
extern void free(unsigned long page,unsigned long size);
extern void allocate_for_task(unsigned long task_index);
extern void free_for_task(unsigned long task_index);
extern unsigned long get_phy_address(unsigned long laddr);

#endif

