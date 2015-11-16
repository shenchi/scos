//=======================================================
//Project Name:	S.C. Operating System
//Version:	0.02
//Auther:	Shen Chi
//=======================================================

#ifndef _DMA_H
#define _DMA_H

#define dma_write	0x1
#define dma_read	0x2

extern void set_8bit_dma(unsigned short channel,unsigned long address,unsigned long count,unsigned char rw);

#endif
