//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#include "fs.h"
#include "kernel.h"
#include "mem.h"
#include "fdc.h"

unsigned long cmpstr(unsigned char* str1,unsigned char* str2);
unsigned long cmpstrl(unsigned char* str1,unsigned char* str2,unsigned long length);
void strcpy(unsigned char* str1,unsigned char* str2,unsigned long length);
void translate_dicname(unsigned char* fsrc,unsigned char* fdst);
unsigned long goto_dic(unsigned char* path);
unsigned long read_file_for_cd(char* filename,unsigned long addr);
unsigned long read_file_without_name(unsigned short firstcrt);
unsigned long create_file_without_name(unsigned long size);
void update_dir(void);
void update_fat(void);
unsigned short get_free_cul(void);

void dir(unsigned char *ret_buf);
void lba2chs(unsigned short lba);
void TEXT(char* charptr,char* str);

unsigned char* floppy_buffer;
unsigned char *filebuf0,*filebuf1,*filebuf2;
unsigned long fb1_size;
unsigned char* FAT_ADDR;
FILE_TABLE* FDT_ADDR;
FILE_TABLE curdir;

unsigned short* readlist;
unsigned long rdl_ptr=0;

#define fdata_buf (floppy_buffer)

unsigned char cur_pos[0x200];

#define cur_path ((long)&cur_pos[0])

unsigned char filetrack;
unsigned char filehead;
unsigned char filesector;

unsigned short get_num_from_fat(unsigned short index){
	unsigned short tmpshort;
	unsigned long tmpindex=index;
	tmpindex=tmpindex*3;
	tmpindex=tmpindex/2;
	tmpindex+=FAT_ADDR;
	tmpshort=*(short*)tmpindex;
	if(index%2==0){
		tmpshort&=0xFFF;
	}else{
		tmpshort>>=4;
	}
	return tmpshort;
}

void put_num_to_fat(unsigned short index,unsigned short num){
	unsigned short tmpshort=num;
	unsigned long tmpindex=index;
	tmpindex=tmpindex*3;
	tmpindex=tmpindex/2;
	tmpindex+=FAT_ADDR;
	if(index%2==0){
		tmpshort&=0xFFF;
	}else{
		tmpshort<<=4;
	}
	*(short*)tmpindex|=tmpshort;
	return;
}

void init_fs(void){
	unsigned long tmpaddr;
	puts("Init File System........");
	floppy_buffer=allocate(5);
	filebuf0=allocate(4);
	readlist=allocate(3);
	tmpaddr=floppy_buffer;
	read_buffer(0);
	ready_fdc();
	set_chs(0,0,1);
	read_floppy(36);
	free_fdc();
	read_buffer(tmpaddr);
	FAT_ADDR=fdata_buf+0x200;
	FDT_ADDR=(FILE_TABLE*)(fdata_buf+0x2600);
	TEXT((char*)cur_path,"\\");
	goto_dic("\\");
	puts("OK!\n\r");
	return;
}

unsigned long goto_dic(unsigned char* path){
	unsigned char* dicname="A File Name";
	unsigned char* pend;
	unsigned long tmpret;
	while(*path==' ')path++;
	if(cmpstrl(path,"\\",1)==0){
		filebuf1=FDT_ADDR;
		fb1_size=224;
		curdir.filename[0]=0;
		path++;
	}
	pend=path;
	while(*pend!=0){
		pend=path;
		while ((*pend!='\\')&&(*pend!=0)){
			if(*pend==0)return 0;
			pend++;
		}
		if(pend!=path){
			translate_dicname(path, dicname);
			tmpret=read_file_for_cd(dicname,filebuf0);
			if(tmpret==-1){
				return -1;
			}else if(tmpret==3){
				filebuf1=FDT_ADDR;
				fb1_size=224;
				curdir.filename[0]=0;
			}else{
				fb1_size=tmpret*0x10;
				filebuf1=filebuf0;
			}
		}
		path=pend;path++;
	}
	return 0;
}

void translate_dicname(unsigned char * fsrc, unsigned char * fdst){
	unsigned char* src=fsrc;
	unsigned char* dst=fdst;
	unsigned long i;
	for(i=0;i<11;i++){
		if((*src!='\\')&&(*src!=0)){
			if((*src>='a')&&(*src<='z'))*src-=('a'-'A');
			*dst=*src;src++;
		}else{
			*dst=' ';
		}
		dst++;
	}
}

unsigned long read_file_for_cd(char* filename,unsigned long addr){
	FILE_TABLE *ftable=(FILE_TABLE*)filebuf1;
	unsigned char tattr;
	unsigned long i,tmpret;
	for(i=0;i<=fb1_size;i++){
		if(i==fb1_size){
			return -1;
		}
		if(cmpstrl((char*)ftable,(char*)filename,11)==0){
			tattr=(*ftable).attr;
			tattr&=0x10;
			if(tattr == 0x10){
				break;
			}else{
				return -1;
			}
		}
		ftable++;
	}
	curdir=*ftable;
	if(curdir.firstlink==0){
		curdir.filename[0]=0;
		return 3;
	}
	read_buffer(0);
	tmpret=read_file_without_name((*ftable).firstlink);
	read_buffer(addr);
	return tmpret;
}

unsigned long read_file(char* filename,unsigned long addr){
	FILE_TABLE *ftable=(FILE_TABLE*)filebuf1;
	unsigned long i;
	for(i=0;i<=fb1_size;i++){
		if(i==fb1_size){
			puts("Can't found the file!\n\r");
			return -1;
		}
		if(cmpstrl((char*)ftable,(char*)filename,11)==0)break;
		ftable++;
	}
	read_buffer(0);
	read_file_without_name((*ftable).firstlink);
	read_buffer(addr);
	return 0;
}

unsigned long delete_file(char* filename){
	FILE_TABLE *ftable=(FILE_TABLE*)filebuf1;
	unsigned short firstcrt;
	unsigned long i;
	for(i=0;i<=fb1_size;i++){
		if(i==fb1_size){
			return -1;
		}
		if(cmpstrl((char*)ftable,(char*)filename,11)==0)break;
		ftable++;
	}
	firstcrt=(*ftable).firstlink;
	unsigned short tmp2,tmplba=firstcrt;
	unsigned long tmpret;
	tmp2=get_num_from_fat(tmplba);
	if(tmplba!=0){
		put_num_to_fat(tmplba,0);
		tmplba=tmp2;
	}
	while(tmplba<0xFF8){
		if(tmplba==0)break;
		tmp2=get_num_from_fat(tmplba);
		put_num_to_fat(tmplba,0);
		tmplba=tmp2;
	}
	(*ftable).firstlink=0;
	(*ftable).filename[0]=0xe5;
	update_fat();
	update_dir();
	return 0;
}

unsigned long create_file(char* filename,unsigned long addr,unsigned long size){
	FILE_TABLE *ftable=(FILE_TABLE*)filebuf1;
	unsigned long i,founded=-1;
	for(i=0;i<=fb1_size;i++){
		if(i==fb1_size){
			if(founded==-1){
				return -1;
			}else{
				break;
			}
		}
		if(cmpstrl((char*)ftable,(char*)filename,11)==0){
			return -1;
		}
		if((*(char*)ftable==0)||(*(char*)ftable==0xe5)){
			if(founded==-1)founded=i;
		}
		ftable++;
	}
	ftable=(FILE_TABLE*)filebuf1;
	ftable+=founded;
	write_buffer(addr,size);
	(*ftable).firstlink=create_file_without_name(size);
	(*ftable).filesize=size;
	strcpy((*ftable).filename, filename, 11);
	update_dir();
	update_fat();
	return 0;
}

unsigned long create_dir(char* filename,unsigned long addr,unsigned long size){
	FILE_TABLE tmpft;
	FILE_TABLE *ftable=(FILE_TABLE*)filebuf1;
	unsigned long i,founded=-1;
	for(i=0;i<=fb1_size;i++){
		if(i==fb1_size){
			if(founded==-1){
				return -1;
			}else{
				break;
			}
		}
		if(cmpstrl((char*)ftable,(char*)filename,11)==0){
			return -1;
		}
		if((*(char*)ftable==0)||(*(char*)ftable==0xe5)){
			if(founded==-1)founded=i;
		}
		ftable++;
	}
	ftable=(FILE_TABLE*)filebuf1;
	ftable+=founded;
	strcpy(tmpft.filename, ".          ", 11);
	tmpft.attr=0x10;
	tmpft.firstlink=get_free_cul();
	memcpy((long)&tmpft,addr,0x20);
	strcpy(tmpft.filename, "..         ", 11);
	tmpft.firstlink=curdir.firstlink;
	memcpy((long)&tmpft,addr+0x20,0x20);
	write_buffer(addr,size);
	(*ftable).firstlink=create_file_without_name(size);
	(*ftable).filesize=size;
	(*ftable).attr=0x10;
	strcpy((*ftable).filename, filename, 11);
	update_dir();
	update_fat();
	return 0;
}

void update_fat(void){
	write_buffer((long)FAT_ADDR, 0x1200);
	set_chs(0, 0, 2);
	ready_fdc();
	write_floppy(9);
	free_fdc();
}

void update_dir_not_root(void){
	unsigned short tmplba=curdir.firstlink;
	unsigned long tmpret;
	unsigned long i;
	rdl_ptr=0;
	*(readlist+rdl_ptr)=tmplba;
	rdl_ptr++;
	tmplba=get_num_from_fat(tmplba);
	while(tmplba<0xFF8){
		*(readlist+rdl_ptr)=tmplba;
		rdl_ptr++;
		if(tmplba==0)for(;;);
		tmplba=get_num_from_fat(tmplba);
	}
	write_buffer((unsigned long)filebuf1,fb1_size*0x20);
	ready_fdc();
	for(i=0;i<rdl_ptr;i++){
		tmplba=(*(readlist+i))+31;
		lba2chs(tmplba);
		set_chs(filetrack, filehead, filesector);
		write_floppy(1);
	}
	free_fdc();
}


void update_dir_root(void){
	write_buffer((long)FDT_ADDR, 0x1e00);
	ready_fdc();
	set_chs(0,1,2);
	write_floppy(15);
	free_fdc();
}

void update_dir(void){
	if(curdir.filename[0]==0){
		update_dir_root();
	}else{
		update_dir_not_root();
	}
	read_file_for_cd(".          ",filebuf0);
}

unsigned long read_file_without_name(unsigned short firstcrt){
	unsigned short tmplba=firstcrt;
	unsigned long tmpret;
	*(readlist+rdl_ptr)=tmplba;
	rdl_ptr++;
	//puts("C Nums ");
	//puthex(tmplba);puts(" ");
	tmplba=get_num_from_fat(tmplba);
	while(tmplba<0xFF8){
		*(readlist+rdl_ptr)=tmplba;
		rdl_ptr++;
		//puthex(tmplba);puts(" ");
		if(tmplba==0)for(;;);
		tmplba=get_num_from_fat(tmplba);
	}
	
	ready_fdc();
	
	unsigned long i;
	//puts("cul num:");puthex(rdl_ptr);puts("\n\r");
	for(i=0;i<rdl_ptr;i++){
		tmplba=(*(readlist+i))+31;
		lba2chs(tmplba);
		set_chs(filetrack,filehead,filesector);
		read_floppy(1);
	}
	free_fdc();
	tmpret=rdl_ptr;
	rdl_ptr=0;
	return tmpret;
}

unsigned short get_free_cul(void){
	unsigned long i,tmpi;
	for(i=2;i++;i<0x600){
		tmpi=get_num_from_fat(i);
		if(tmpi==0)return i;
	}
	return -1;
}

unsigned long create_file_without_name(unsigned long size){
	unsigned long culnum=size/0x200;
	unsigned long i,nowcul,firstcul,oldcul;
	if(size%0x200!=0)culnum+=1;
	firstcul=get_free_cul();
	oldcul=firstcul;
	if(firstcul==-1)return -1;
	*(readlist+rdl_ptr)=firstcul;
	rdl_ptr++;put_num_to_fat(firstcul, 0xff8);
	if(culnum>1){
		for(i=1;i<culnum;i++){
			nowcul=get_free_cul();
			if(nowcul==-1)return -1;
			*(readlist+rdl_ptr)=nowcul;
			rdl_ptr++;
			put_num_to_fat(oldcul, nowcul);
			oldcul=nowcul;
			put_num_to_fat(oldcul, 0xff8);
		}
	}
	put_num_to_fat(oldcul,0xFFF);

	ready_fdc();
	for(i=0;i<rdl_ptr;i++){
		nowcul=(*(readlist+i))+31;
		lba2chs(nowcul);
		set_chs(filetrack, filehead, filesector);
		write_floppy(1);
	}
	free_fdc();
	rdl_ptr=0;

	return firstcul;
}

void stradd(unsigned char* str1,unsigned char* str2){
	while(*str2!=0)str2++;
	asm("2:\
		lodsb;\
		or %%al,%%al;\
		jz 1f;\
		stosb;\
		jmp 2b;\
		1:mov $0,%%al;stosb"::"S"((unsigned long)str1),"D"((unsigned long)str2));
}

void dir(unsigned char *ret_buf){
	FILE_TABLE *ftable=(FILE_TABLE*)filebuf1;
	unsigned char tattr;
	unsigned char* tmpfilename="A File Name\n";
	unsigned long i,filetotal=0;
	for(i=0;i<fb1_size;i++){
		if(cmpstrl((char*)ftable,"\0",1)!=0){
			strcpy(tmpfilename,(unsigned char*)ftable, 11);
			if(*tmpfilename!=0xe5){
				filetotal++;
				tattr=(*ftable).attr;
				tattr&=0x10;
				if(tattr == 0x10){
					strcpy((unsigned char*)&(*(tmpfilename+8)), "DIR", 3);
				}
				stradd(tmpfilename,ret_buf);
			}
		}
		ftable++;
	}
	
}


void lba2chs(unsigned short lba){
	unsigned long tmpesp;
	filesector=(lba%18)+1;
	filehead=(lba/18);
	filetrack=filehead/2;
	filehead=filehead%2;
}

void TEXT(char* charptr,char* str){
	while(*(str-1)!=0){
		*charptr=*str;
		charptr++;
		str++;
	}
}


unsigned long cmpstr(unsigned char* str1,unsigned char* str2){
	unsigned char *tmpstr1;
	unsigned char *tmpstr2;
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

unsigned long cmpstrl(unsigned char* str1,unsigned char* str2,unsigned long length){
	unsigned char *tmpstr1;
	unsigned char *tmpstr2;
	unsigned long i;
	tmpstr1=str1;tmpstr2=str2;
	for(i=0;i<length;i++){
		if((*tmpstr1)!=(*tmpstr2))return 1;
		tmpstr1++;tmpstr2++;
	}
	return 0;
}

void strcpy(unsigned char* str1,unsigned char* str2,unsigned long length){
	asm("rep movsb"::"S"((unsigned long)str2),"D"((unsigned long)str1),"c"(length));
}

