//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "gdt.h"

#define gdt_addr 0

void set_seg_desc(unsigned short index,unsigned long baseaddr,unsigned long limit,unsigned short gflag,unsigned short sflag,unsigned short dpl,unsigned short type);
void set_tss_desc(unsigned short index,unsigned long baseaddr,unsigned short dpl);
void create_seg_desc(unsigned long sdaddr,unsigned long baseaddr,unsigned long limit,unsigned short gflag,unsigned short sflag,unsigned short dpl,unsigned short type);
void init_gdt(void);

void set_seg_desc(unsigned short index,unsigned long baseaddr,unsigned long limit,unsigned short gflag,unsigned short sflag,unsigned short dpl,unsigned short type)
{
	SEG_DESC seg_desc;
	SEG_DESC *psd=(SEG_DESC*)(gdt_addr+index*8);
	seg_desc.limit0=limit&0xFFFF;
	seg_desc.baseaddr0=baseaddr&0xFFFF;
	seg_desc.baseaddr1=(baseaddr>>16)&0xFF;
	seg_desc.attr0=0x80 | ((dpl&3)<<5) | ((sflag&1)<<4) | (type&0xF);
	seg_desc.attr1=0x40 | ((limit>>16)&0xF) | ((gflag&1)<<7);
	seg_desc.baseaddr2=(baseaddr>>24)&0xFF;
	*psd=seg_desc;
}

void set_tss_desc(unsigned short index,unsigned long baseaddr,unsigned short dpl)
{
	unsigned long limit=104;
	SEG_DESC seg_desc;
	SEG_DESC *psd=(SEG_DESC*)(gdt_addr+index*8);
	seg_desc.limit0=limit&0xFFFF;
	seg_desc.baseaddr0=baseaddr&0xFFFF;
	seg_desc.baseaddr1=(baseaddr>>16)&0xFF;
	seg_desc.attr0=0x80 | ((dpl&3)<<5) | 9;
	seg_desc.attr1=(limit>>16)&0xF;
	seg_desc.baseaddr2=(baseaddr>>24)&0xFF;
	*psd=seg_desc;
}

void create_seg_desc(unsigned long sdaddr,unsigned long baseaddr,unsigned long limit,unsigned short gflag,unsigned short sflag,unsigned short dpl,unsigned short type){
	SEG_DESC seg_desc;
	SEG_DESC *psd=(SEG_DESC*)sdaddr;
	seg_desc.limit0=limit&0xFFFF;
	seg_desc.baseaddr0=baseaddr&0xFFFF;
	seg_desc.baseaddr1=(baseaddr>>16)&0xFF;
	seg_desc.attr0=0x80 | ((dpl&3)<<5) | ((sflag&1)<<4) | (type&0xF);
	seg_desc.attr1=0x40 | ((limit>>16)&0xF) | ((gflag&1)<<7);
	seg_desc.baseaddr2=(baseaddr>>24)&0xFF;
	*psd=seg_desc;
}

void init_gdt(void){
	unsigned long i;
	char *potr=(char)gdt_addr;
	for(i=0;i<0x800;i++){
		*(potr+i)=0;
	}
	set_seg_desc(1,0,0xFFFFF,1,1,0,0xA);
	set_seg_desc(2,0,0xFFFFF,1,1,0,0x2);
	return;
}

