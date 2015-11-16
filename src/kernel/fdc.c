//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "fdc.h"
#include "timer.h"
#include "screen.h"
#include "dma.h"
#include "mem.h"

unsigned char reply_buffer[7]={0,0,0,0,0,0,0,0};
unsigned char intr_done=0;

unsigned char track,ntrack=0;
unsigned char head,nhead=0;
unsigned char sector,nsector=1;

unsigned cur_dor=0x0c;

unsigned char motor_state=MOTOR_CLOSED;
unsigned short motor_time=300;

unsigned char *floppy_buf=(unsigned char*)0x3000;

#define st0 (reply_buffer[0])
#define st1 (reply_buffer[1])
#define st2 (reply_buffer[2])
#define st3 (reply_buffer[3])

#define fb_addr ((long)floppy_buf)
unsigned long buf_rw_ptr=0;


void sendbyte(unsigned char byte);
unsigned char getbyte(void);
void motor_on(void);
void motor_off(void);
void motor_timer(void);
void wait_for_intr(void);
void null_fdc_intr(void);
void init_fdc(void);
void reset_intr(void);
void reset_fdc(void);
void reca_seek_intr(void);
void recalibrate_fdc(void);
void seek_fdc(unsigned char c);
void rw_intr(void);
void rw_sector(unsigned char command,unsigned long addr);
void ready_fdc(void);
void free_fdc(void);
void set_chs(unsigned char c,unsigned char h,unsigned char s);
void get_chs(unsigned char *c,unsigned char *h,unsigned char *s);
void read_floppy(unsigned long nsec);
void write_floppy(unsigned long nsec);
void read_buffer(unsigned long addr);
void write_buffer(unsigned long addr,unsigned long size);
void irq6(void);
void (*fdc_intr)(void);

void sendbyte(unsigned char byte){
	unsigned short port=FDC_DATA;
	unsigned char msr;
	while(1){
		msr=inb(FDC_MSR)&0xC0;
		if(msr==0x80){
			__asm__("outb %0,%1"::"a"(byte),"d"(port));
			return;
		}
	}
}

unsigned char getbyte(void){
	unsigned short port=FDC_DATA;
	unsigned char msr;
	while(1){
		msr=inb(FDC_MSR)&0xC0;
		if(msr==0xC0){
			__asm__("inb %1,%0":"=a"(msr):"d"(port));
			return msr;
		}
	}
}

void motor_on(void){
	if((motor_state==MOTOR_CLOSED)||(motor_state==MOTOR_READY_OFF)){
		cur_dor|=DOR_MOTOR1;
		outb(FDC_DOR,cur_dor);
		motor_state=MOTOR_READY;
		motor_time=50;
	}
}

void motor_off(void){
	if((motor_state==MOTOR_OPENED)||(motor_state==MOTOR_READY)){
		cur_dor&=0xF;
		outb(FDC_DOR,cur_dor);
		motor_state=MOTOR_READY_OFF;
		motor_time=300;
	}
}

void motor_timer(void){
	if((motor_time>=0)&&((motor_state==MOTOR_READY)||(motor_state==MOTOR_READY_OFF))){
		if(motor_time>0){
			motor_time--;
		}
		if(motor_time==0){
			if(motor_state==MOTOR_READY)motor_state=MOTOR_OPENED;
			if(motor_state==MOTOR_READY_OFF)motor_state=MOTOR_CLOSED;
		}
	}
}


void wait_for_intr(void){
	while(intr_done==0);
	intr_done=0;
}

void null_fdc_intr(void){
	puts("\n\rError: A null FDC interrupt!\n\r");
	intr_done=1;
}

void init_fdc(void){
	//unsigned long tmpaddr;
	//tmpaddr=allocate(0x100);
	//floppy_buf=(char*)tmpaddr;
	set_interrupt_gate(0x26, 0, (unsigned long)&irq6);
	set_irq_mask(6,0);
	reset_fdc();
	buf_rw_ptr=0;
}

void reset_intr(void){
	sendbyte(FDC_SENSEI);
	st0=getbyte();
	track=getbyte();
	sendbyte(FDC_SPECIFY);
	sendbyte(0xCF);
	sendbyte(6);
	intr_done=1;
}

void reset_fdc(void){
	unsigned long i;
	intr_done=0;
	fdc_intr=reset_intr;
	cur_dor=0x0C;
	cli();
	outb(FDC_DOR,cur_dor&~0x04);
	for (i=0 ; i<100 ; i++)
		 __asm__("nop");
	outb(FDC_DOR,cur_dor);
	outb(FDC_CCR,0);
	sti();
	wait_for_intr();
}

void reca_seek_intr(void){
	sendbyte(FDC_SENSEI);
	st0=getbyte();
	track=getbyte();
	intr_done=1;
}

void recalibrate_fdc(void){
	fdc_intr=reca_seek_intr;
	motor_on();
	while(motor_state!=MOTOR_OPENED);
	sendbyte(FDC_RECALIBRATE);
	sendbyte(0);
	wait_for_intr();
	motor_off();
}

void seek_fdc(unsigned char c){
	fdc_intr=reca_seek_intr;
	motor_on();
	while(motor_state!=MOTOR_OPENED);
	sendbyte(FDC_SEEK);
	sendbyte((head&1)<<2);
	sendbyte(c);
	wait_for_intr();
	motor_off();
}

void rw_intr(void){
	int i=0;
	unsigned char msr=inb(FDC_MSR)&(MSR_RQM|MSR_DIO|MSR_CB);
	while(i<7){
		if(msr==(MSR_RQM|MSR_DIO|MSR_CB)){
			reply_buffer[i]=inb(FDC_DATA);
			i++;
		}
		msr=inb(FDC_MSR)&(MSR_RQM|MSR_DIO|MSR_CB);
	}
	track=reply_buffer[3];
	head=reply_buffer[4];
	sector=reply_buffer[5];
	intr_done=1;
}

void rw_sector(unsigned char command,unsigned long addr){
	unsigned long i;
	for(i=0;i<3;i++){
		if(ntrack!=track)seek_fdc(ntrack);
		motor_on();
		while(motor_state!=MOTOR_OPENED);
		if(command==0){
			set_8bit_dma(2, addr, 0x200-1, dma_write);
		}else{
			set_8bit_dma(2, addr, 0x200-1, dma_read);
		}
		fdc_intr=rw_intr;
		if(command==0){
			sendbyte(FDC_READ);
		}else{
			sendbyte(FDC_WRITE);
		}
		sendbyte((nhead&1)<<2);
		sendbyte(ntrack);
		sendbyte(nhead);
		sendbyte(nsector);
		sendbyte(2);
		sendbyte(18);
		sendbyte(0x1B);
		sendbyte(0xFF);
		wait_for_intr();
		if((st0&0xC0)==0)return;
		puts("Read Floppy Failed!");
		puts("   st0:");puthex(st0);puts("\n\r");
		puts("   st1:");puthex(st1);puts("\n\r");
		puts("   st2:");puthex(st2);puts("\n\r");
		recalibrate_fdc();
		motor_on();
		while(motor_state!=MOTOR_OPENED);
	}
}

void ready_fdc(void){
	motor_on();
	outb(FDC_CCR,0);
	recalibrate_fdc();
	motor_on();
	while(motor_state!=MOTOR_OPENED);
}

void free_fdc(void){
	motor_off();
}

void set_chs(unsigned char c,unsigned char h,unsigned char s){
	ntrack=c;
	nhead=h;
	nsector=s;
}

void get_chs(unsigned char *c,unsigned char *h,unsigned char *s){
	*c=track;
	*h=head;
	*s=sector;
}

void read_floppy(unsigned long nsec){
	unsigned long i;
	for(i=0;i<nsec;i++){
		rw_sector(0, fb_addr+buf_rw_ptr);
		buf_rw_ptr+=0x200;
		if(nsec>1){
			ntrack=track;
			nhead=head;
			nsector=sector;
		}
	}
}

void write_floppy(unsigned long nsec){
	unsigned long i;
	for(i=0;i<nsec;i++){
		rw_sector(1, fb_addr+buf_rw_ptr);
		buf_rw_ptr+=0x200;
		if(nsec>1){
			ntrack=track;
			nhead=head;
			nsector=sector;
		}
	}
}

void read_buffer(unsigned long addr){
	if(addr!=0){
		memcpy(fb_addr,addr,buf_rw_ptr);
	}
	buf_rw_ptr=0;
}

void write_buffer(unsigned long addr,unsigned long size){
	if((addr!=0)&&(size!=0)){
		if(size%0x200!=0)size+=(0x200-(size%0x200));
		memcpy(addr,fb_addr,size);
	}
	buf_rw_ptr=0;
}
