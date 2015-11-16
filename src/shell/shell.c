#include "common.h"
#include "screen.h"
#include "keyboard.h"

void shell_main(void);
void do_command(void);
void dohelp(void);
void phrase_path(unsigned char* path);
void copy_file(char* commandline);

void main(void){
	shell_main();
}

unsigned char key;
unsigned char cmd_buf[1023];
unsigned char path_buf[1023];
unsigned short cbuf_ptr;

void shell_main(void){
	init_screen();
	setcharattr(0x1f);
	clear();
	putstr("Command Shell for SCOS 0.02\n");
	dohelp();
	refresh();
	init_keyboard();
	for(cbuf_ptr=0;cbuf_ptr<1024;cbuf_ptr++){
		cmd_buf[cbuf_ptr]=0;
		path_buf[cbuf_ptr]=0;
	}
	path_buf[0]='\\';
	putstr(path_buf);putstr(">");
	cbuf_ptr=0;
	while(1){
		key=get_char();
		if((key==0x08)&&(cbuf_ptr>0)){
			putch(key);
			cbuf_ptr--;
		}else if((key=='\r')||(key=='\n')){
			putstr("\n");
			cmd_buf[cbuf_ptr]=0;
			do_command();
			cbuf_ptr=0;
		}else if(key==0x09){
			cmd_buf[cbuf_ptr]=' ';
			putch(' ');
			cbuf_ptr++;
		}else if(key>=' '){
			cmd_buf[cbuf_ptr]=key;
			putch(key);
			cbuf_ptr++;
		}
		refresh();
	}
	for(;;);
}

void add_sym(unsigned char* str){
	while(*str!=0)str++;
	if(*(str-1)!='\\'){
		*str='\\';
		*(str+1)=0;
	}
}

unsigned char textbuf[0xFFF];

void do_command(void){
	unsigned char *cbuf_addr,*pbuf_addr;
	unsigned char* filename="a file name";
	unsigned long i;
	cbuf_addr=(unsigned char*)&cmd_buf[0];
	pbuf_addr=(unsigned char*)&path_buf[0];
	if(cmpstr(cbuf_addr,"ver")==0){
		putstr("S.C. Operating System 0.02\nBy Shen Chi\n");
	}else if(cmpstr(cbuf_addr,"cls")==0){
		clear();
		putstr("Command Shell for SCOS 0.02");
	}else if(cmpstrl(cbuf_addr,"type ",5)==0){
		cbuf_addr+=5;
		//putstr("cbuf_addr:");puthex((unsigned long)cbuf_addr);putstr("\n");
		translate_filename(cbuf_addr,filename);
		//putstr(filename);putstr("\n");
		//putstr("cbuf_addr:");puthex((unsigned long)cbuf_addr);putstr("\n");
		for(i=0;i<0x1000;i++)textbuf[i]=0;
		asm("mov $0x400,%%eax;int $0x40":"=a"(i):"b"((unsigned long)filename),"c"((unsigned long)(&textbuf[0])));
		if(i==0)putstr((char*)&textbuf[0]);
		cbuf_addr-=5;
	}else if(cmpstrl(cbuf_addr,"del ",4)==0){
		cbuf_addr+=4;
		translate_filename(cbuf_addr,filename);
		asm("mov $0x404,%%eax;int $0x40":"=a"(i):"b"((unsigned long)filename));
		if(i!=0)putstr("Delete Failed!\n");
		cbuf_addr-=4;
	}else if(cmpstrl(cbuf_addr,"cp ",3)==0){
		cbuf_addr+=3;
		copy_file(cbuf_addr);
		cbuf_addr-=3;
	}else if(cmpstrl(cbuf_addr,"rd ",3)==0){
		cbuf_addr+=3;
		translate_dirname(cbuf_addr,filename);
		asm("mov $0x404,%%eax;int $0x40":"=a"(i):"b"((unsigned long)filename));
		if(i!=0)putstr("Delete Failed!\n");
		cbuf_addr-=3;
	}else if(cmpstrl(cbuf_addr,"md ",3)==0){
		cbuf_addr+=3;
		translate_dirname(cbuf_addr,filename);
		for(i=0;i<0x200;i++)textbuf[i]=0;
		asm("mov $0x405,%%eax;int $0x40":"=a"(i):"b"((unsigned long)filename),"c"((unsigned long)(&textbuf[0])),"d"(0x1ff));
		if(i!=0)putstr("Create Failed!\n");
		cbuf_addr-=3;
	}else if(cmpstrl(cbuf_addr,"switch ",7)==0){
		cbuf_addr+=7;
		i=str2num(cbuf_addr);
		if(i>0){
			asm("mov $0x303,%%eax;int $0x40"::"b"(i));
		}else{
			putstr("Task 0 is System IDLE task!\n");
		}
		cbuf_addr-=7;
	}else if(cmpstrl(cbuf_addr,"kill ",5)==0){
		cbuf_addr+=5;
		i=str2num(cbuf_addr);
		if(i>1){
			asm("mov $0x202,%%eax;int $0x40"::"b"(i));
		}else{
			putstr("This is System task!\n");
		}
		cbuf_addr-=5;
	}else if(cmpstrl(cbuf_addr,"run ",4)==0){
		cbuf_addr+=4;
		translate_filename(cbuf_addr,filename);
		asm("mov $0x200,%%eax;int $0x40"::"b"((unsigned long)filename));
		cbuf_addr-=4;
	}else if(cmpstrl(cbuf_addr,"cd ",3)==0){
		cbuf_addr+=3;i=0;
		asm("mov $0x401,%%eax;int $0x40":"=a"(i):"b"((unsigned long)cbuf_addr));
		if(i==0){
			add_sym(cbuf_addr);
			if(*cbuf_addr=='\\'){
				strcpy(cbuf_addr,pbuf_addr);
			}else{
				stradd(cbuf_addr,pbuf_addr);
			}
			phrase_path(path_buf);
		}else{
			putstr("CD command failed!\n");
		}
		cbuf_addr-=3;
	}else if(cmpstr(cbuf_addr,"dir")==0){
		for(i=0;i<0x1000;i++)textbuf[i]=0;
		asm("mov $0x403,%%eax;int $0x40"::"b"((unsigned long)(&textbuf[0])));
		putstr((char*)&textbuf[0]);
	}else if(cmpstr(cbuf_addr,"tl")==0){
		for(i=0;i<0x1000;i++)textbuf[i]=0;
		asm("mov $0x203,%%eax;int $0x40"::"b"((unsigned long)(&textbuf[0])));
		putstr((char*)&textbuf[0]);
	}else if(cmpstr(cbuf_addr,"help")==0){
		dohelp();
	}else{
		putstr("\"");putstr(cbuf_addr);putstr("\" was not a command!\n");
	}
	putstr("\n");putstr(path_buf);putstr(">");
}

void dohelp(void){
	unsigned char* filename="HELP    TXT";
	unsigned long i;
	for(i=0;i<0x1000;i++)textbuf[i]=0;
	asm("mov $0x400,%%eax;int $0x40":"=a"(i):"b"((unsigned long)filename),"c"((unsigned long)(&textbuf[0])));
	if(i==0)putstr((char*)&textbuf[0]);
}

void phrase_path(unsigned char* path){
	unsigned char *tmpp,*pst=path;
	while(*path==' ')path++;
	while(*path!=0){
		if((*path=='.')&&(*(path+1)=='.')){
			tmpp=path;tmpp-=2;
			while((tmpp>=pst)&&(*tmpp!='\\'))tmpp--;
			tmpp++;
			strcpy((char*)(path+3),tmpp);
		}else if(*path=='.'){
			strcpy((char*)(path+2),path);
		}
		path++;
	}
}


unsigned char tmp_path_buf[1023];
unsigned char tmp_file_name[20];
unsigned char filebuf[100000];
unsigned char fname[11];
void copy_file(char* commandline){
	unsigned char* cl=commandline;
	unsigned char *cl2,*p;
	unsigned long i,j,filesize;
	fname[10]=0;fname[11]=0;
	j=0;i=0;
	while(*cl!=' '){
		j++;
		if(*cl=='\\')i=j;
		cl++;
	}
	cl=commandline;
	strcpyl(cl,tmp_path_buf,i);
	cl+=i;
	strcpyl(cl,tmp_file_name,j-i+1);
	translate_filename(tmp_file_name,fname);
	cl+=j-i+1;
	if(i!=0)asm("mov $0x401,%%eax;int $0x40"::"b"((unsigned long)tmp_path_buf));
	for(i=0;i<0x1000;i++)filebuf[i]=0;
	asm("mov $0x400,%%eax;int $0x40":"=a"(i):"b"((unsigned long)fname),"c"((unsigned long)(&filebuf[0])));
	if(i!=0){
		putstr("Read file failed!\n");
	}
	p=filebuf;
	filesize=0;
	while(*p!=0){
		filesize++;
		p++;
	}
	cl2=cl;
	i=0;j=0;
	while(*cl!=' '){
		j++;
		if(*cl=='\\')i=j;
		cl++;
	}
	cl=cl2;
	strcpyl(cl,tmp_path_buf,i);
	cl+=i;
	strcpyl(cl,tmp_file_name,j-i+1);
	translate_filename(tmp_file_name,fname);
	cl+=j-i;
	if(i!=0)asm("mov $0x401,%%eax;int $0x40"::"b"((unsigned long)tmp_path_buf));
	asm("mov $0x404,%%eax;int $0x40"::"b"((unsigned long)fname));
	asm("mov $0x402,%%eax;int $0x40"::"b"((unsigned long)fname),"c"((unsigned long)filebuf),"d"(filesize));
	asm("mov $0x401,%%eax;int $0x40"::"b"((unsigned long)path_buf));
}

