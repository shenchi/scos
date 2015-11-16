#include "common.h"
#include "screen.h"
#include "keyboard.h"

void edit_main(void);
void command_mode(void);
void phrase_new(void); 

void main(void){
	edit_main();
}

unsigned char filebuf[0x100000-1];
unsigned long cbuf_ptr=0;
unsigned char enterbuf[0x100000-1];
unsigned long ebuf_ptr=0;

unsigned char lrow_bak[0xa0];

void edit_main(void){
	unsigned long i,tmprow,tmpcul;
	unsigned char key,skey;
	init_screen();
	setcharattr(0x70);
	init_keyboard();
	for(i=0;i<0x50000;i++){
		filebuf[i]=0;
		enterbuf[i]=0;
	}
	cbuf_ptr=0;
	ebuf_ptr=0;
	clear();
	setcursor(1,1);
	refresh();
	while(1){
		skey=get_scancode();
		key=translate_scancode(skey);
		if(key==27){
			for(i=0;i<0xa0;i++)lrow_bak[i]=getchar(i,25);
			command_mode();
			for(i=0;i<0xa0;i++)putchar(lrow_bak[i],i,25);
		}
		if((key==0x08)&&(cbuf_ptr>0)){
			getcursor(&tmpcul, &tmprow);
			cbuf_ptr--;
			if(tmpcul>1){
				putch(key);
			}else{
				if(filebuf[cbuf_ptr]=='\n'){
					cbuf_ptr--;
					tmprow--;ebuf_ptr--;
					tmpcul=enterbuf[ebuf_ptr];
					putchar(' ', tmpcul, tmprow);
					setcursor(tmpcul,tmprow);
				}else{
					tmprow--;
					tmpcul=80;
					putchar(' ', tmpcul, tmprow);
					setcursor(tmpcul,tmprow);
				}
			}
		}else if((key=='\r')||(key=='\n')){
			getcursor(&tmpcul,&tmprow);
			enterbuf[ebuf_ptr]=tmpcul;
			putstr("\n");
			filebuf[cbuf_ptr]='\r';
			filebuf[cbuf_ptr+1]='\n';
			cbuf_ptr+=2;
			ebuf_ptr++;
		}else if(key==0x09){
			filebuf[cbuf_ptr]=' ';
			putch(' ');
			cbuf_ptr++;
		}else if(key>=' '){
			filebuf[cbuf_ptr]=key;
			putch(key);
			cbuf_ptr++;
		}
		refresh();
	}
}

void do_command(void);

unsigned char cmd_buf[0xA0-1];
unsigned long cmd_ptr=0;

unsigned long usrx,usry;
void command_mode(void){
	unsigned long i;
	unsigned char key,skey;
	for(i=0;i<0xa0;i++)cmd_buf[i]=0;
	cmd_ptr=0;
	getcursor(&usrx,&usry);
	setcursor(1,25);
	putstr("Command:");
	while(1){
		skey=get_scancode();
		key=translate_scancode(skey);
		if(key==27)break;
		if((key==0x08)&&(cmd_ptr>0)){
			putch(key);
			cmd_ptr--;
		}else if((key=='\r')||(key=='\n')){
			cmd_buf[cmd_ptr]=0;
			do_command();
			cmd_ptr=0;
			break;
		}else if(key==0x09){
			cmd_buf[cmd_ptr]=' ';
			putch(' ');
			cmd_ptr++;
		}else if(key>=' '){
			cmd_buf[cmd_ptr]=key;
			putch(key);
			cmd_ptr++;
		}
		refresh();
	}
	setcursor(usrx,usry);
}

void do_command(void){
	unsigned char* fn="A File Name";
	unsigned char* cbuf_addr=&cmd_buf[0];
	unsigned long i;
	if(cmpstr(cbuf_addr,"exit")==0){
		asm("mov $0x303,%%eax;int $0x40"::"b"(1));
		i=gettasknum();asm("mov $0x202,%%eax;int $0x40"::"b"(i));
	}else if(cmpstrl(cbuf_addr,"save ",5)==0){
		cbuf_addr+=5;
		translate_filename(cbuf_addr, fn);
		asm("mov $0x404,%%eax;int $0x40"::"b"((long)fn));
		asm("mov $0x402,%%eax;int $0x40"::"b"((long)fn),"c"((long)&filebuf[0]),"d"(cbuf_ptr));
		cbuf_addr-=5;
	}else if(cmpstrl(cbuf_addr,"open ",5)==0){
		cbuf_addr+=5;
		for(i=0;i<0x10000;i++){
			filebuf[i]=0;
			enterbuf[i]=0;
		}
		translate_filename(cbuf_addr, fn);
		asm("mov $0x400,%%eax;int $0x40":"=a"(i):"b"((unsigned long)fn),"c"((unsigned long)(&filebuf[0])));
		phrase_new();
		cbuf_addr-=5;
	}
	return;
}


void phrase_new(){
	unsigned long i,tmprow,tmpcul;
	unsigned char tmpchar;
	cbuf_ptr=0;ebuf_ptr=0;
	clear();
	setcursor(1,1);
	while(1){
		tmpchar=filebuf[cbuf_ptr];
		if(tmpchar==0)break;
		if(tmpchar=='\r'){
			getcursor(&tmpcul,&tmprow);
			enterbuf[ebuf_ptr]=tmpcul;
			putstr("\n");
			cbuf_ptr+=2;
			ebuf_ptr++;
		}else if(tmpchar=='\n'){
			// empty
		}else if(tmpchar==0x09){
			putch(' ');
			cbuf_ptr++;
		}else if(tmpchar>=' '){
			putch(tmpchar);
			cbuf_ptr++;
		}
	}
	refresh();
	getcursor(&usrx,&usry);
}

