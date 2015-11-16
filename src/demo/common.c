#include "common.h"

void memfill(unsigned long addr,unsigned long size,unsigned char value);
void memcpy(unsigned long srcaddr,unsigned long dstaddr,unsigned long size);
unsigned char inb(unsigned short port);
void outb(unsigned short port,unsigned char byte);
void sti(void);
void cli(void);
unsigned long cmpstr(unsigned char* str1,unsigned char* str2);
unsigned long cmpstrl(unsigned char* str1,unsigned char* str2,unsigned long length);
void strcpy(unsigned char* str1,unsigned char* str2,unsigned long length);
void translate_filename(unsigned char* fsrc,unsigned char* fdst);
unsigned long str2num(char* str);
unsigned long gettasknum(void);

void sti(void){
	__asm__("sti");
}

void cli(void){
	__asm__("cli");
}

void memfill(unsigned long addr,unsigned long size,unsigned char value){
	__asm__("1:stosb;loop 1b"::"c"(size),"D"(addr),"a"(value));
	return;
}

void memcpy(unsigned long srcaddr,unsigned long dstaddr,unsigned long size){
	__asm__("1:movsb;loop 1b"::"c"(size),"S"(srcaddr),"D"(dstaddr));
	return;
}

unsigned char inb(unsigned short port){
	unsigned char tmp;
	__asm__("inb %1,%0":"=a"(tmp):"d"(port));
	return tmp;
}
void outb(unsigned short port,unsigned char byte){
	__asm__("outb %0,%1"::"a"(byte),"d"(port));
	__asm__("jmp 1f\n\t1:jmp 1f\n\t1:");
	return;
}

unsigned char *tmpstr1;
unsigned char *tmpstr2;
unsigned long cmpstr(unsigned char* str1,unsigned char* str2){
	tmpstr1=str1;tmpstr2=str2;
	while((*tmpstr1!=0)&&(*tmpstr2!=0)){
		if((*tmpstr1)!=(*tmpstr2))return 1;
		tmpstr1++;tmpstr2++;
	}
	if((*tmpstr1==0)&&(*tmpstr2==0)){
		return 0;
	}else{
		return 1;
	}
}
unsigned long i;
unsigned long cmpstrl(unsigned char* str1,unsigned char* str2,unsigned long length){
	tmpstr1=str1;tmpstr2=str2;
	for(i=0;i<length;i++){
		if((*tmpstr1)!=(*tmpstr2))return 1;
		tmpstr1++;tmpstr2++;
	}
	return 0;
}

void strcpy(unsigned char* str1,unsigned char* str2,unsigned long length){
	//putstr("Copy ");puthex(length);putstr(" bytes from ");puthex((unsigned long)str2);putstr(" to ");puthex((unsigned long)str1);putstr("\n");
	asm("rep movsb"::"S"((unsigned long)str2),"D"((unsigned long)str1),"c"(length));
}

void translate_filename(unsigned char* fsrc,unsigned char* fdst){
	unsigned char* src=fsrc;
	unsigned char* dst=fdst;
	unsigned long i;
	for(i=0;i<8;i++){
		if(*src!='.'){
			if((*src>='a')&&(*src<='z'))*src-=('a'-'A');
			*dst=*src;src++;
		}else{
			*dst=' ';
		}
		dst++;
	}
	if(*src=='.')src++;
	for(i=0;i<3;i++){
		if((*src>='a')&&(*src<='z'))*src-=('a'-'A');
		*dst=*src;
		dst++;src++;
	}
}


unsigned long str2num(char* str){
	unsigned long tmpre=0;
	while(*str!=0){
		if((*str>='0')&&(*str<='9'))tmpre=(tmpre*10)+(*str-'0');
		str++;
	}
	return tmpre;
}
unsigned long gettasknum(void){
	unsigned long tmptasknum;
	asm("mov $0x201,%%eax;int $0x40":"=a"(tmptasknum));
	return tmptasknum;
}

