//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#ifndef _GDT_H
#define _GDT_H

typedef volatile struct{
	unsigned short limit0;
	unsigned short baseaddr0;
	unsigned char baseaddr1;
	unsigned char attr0;
	unsigned char attr1;
	unsigned char baseaddr2;
} SEG_DESC;

typedef volatile struct {
	unsigned short limit;
	unsigned long baseaddr;
} GDT_48;

extern void set_seg_desc(unsigned short index,unsigned long baseaddr,unsigned long limit,unsigned short gflag,unsigned short sflag,unsigned short dpl,unsigned short type);
extern void set_tss_desc(unsigned short index,unsigned long baseaddr,unsigned short dpl);
extern void create_seg_desc(unsigned long sdaddr,unsigned long baseaddr,unsigned long limit,unsigned short gflag,unsigned short sflag,unsigned short dpl,unsigned short type);
extern void init_gdt(void);

#endif

