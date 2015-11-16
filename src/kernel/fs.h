//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#ifndef _FS_H
#define _FS_H

typedef volatile struct{
	unsigned char filename[11];
	unsigned char attr;
	unsigned char crttimetenth;
	unsigned short crttime;
	unsigned short crtdate;
	unsigned short lstaccdate;
	unsigned short firstlinkhi;
	unsigned short wrttime;
	unsigned short wrtdate;
	unsigned short firstlink;
	unsigned long filesize;
} FILE_TABLE;

extern void init_fs(void);
extern unsigned long read_file(char* filename,unsigned long addr);
extern unsigned long goto_dic(unsigned char* path);
extern unsigned long create_file(char* filename,unsigned long addr,unsigned long size);
extern unsigned long create_dir(char* filename,unsigned long addr,unsigned long size);
extern unsigned long delete_file(char* filename);
extern void dir(unsigned char *ret_buf);

#endif

