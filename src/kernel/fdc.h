//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#ifndef _FDC_H
#define _FDC_H

#define FDC_DOR		0x3f2
#define FDC_MSR		0x3f4
#define FDC_DATA 	0x3f5
#define FDC_DIR 	0x3f7
#define FDC_CCR 	0x3f7

#define DOR_MOTOR1	0x10
#define DOR_INT_DMA 0x08
#define DOR_RESET	0x04

#define MSR_RQM 0x80
#define MSR_DIO 0x40
#define MSR_NDM 0x20
#define MSR_CB  0x10

#define MOTOR_CLOSED	0
#define MOTOR_OPENED	3
#define MOTOR_READY		1
#define MOTOR_READY_OFF	2

#define FDC_RECALIBRATE 0x07
#define FDC_SEEK		0x0F
#define FDC_READ		0xE6
#define FDC_WRITE		0xC5
#define FDC_SENSEI		0x08
#define FDC_SPECIFY		0x03


extern void motor_on(void);
extern void motor_off(void);
extern void motor_timer(void);
extern void init_fdc(void);
extern void reset_fdc(void);
extern void recalibrate_fdc(void);
extern void seek_fdc(unsigned char c);
extern void rw_sector(unsigned char command,unsigned long addr);
extern void ready_fdc(void);
extern void free_fdc(void);
extern void set_chs(unsigned char c,unsigned char h,unsigned char s);
extern void get_chs(unsigned char *c,unsigned char *h,unsigned char *s);
extern void read_floppy(unsigned long nsec);
extern void write_floppy(unsigned long nsec);
extern void read_buffer(unsigned long addr);
extern void write_buffer(unsigned long addr,unsigned long size);


#endif

