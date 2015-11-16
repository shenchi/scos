#include "common.h"
#include "screen.h"
#include "keyboard.h"

void demo_main(void);

void main(void){
	demo_main();
}

unsigned long i=0;

void demo_main(void){
	init_screen();
	clear();
	putstr("This is demo!\n");
	i=gettasknum();
	putstr("Task ");puthex(i);putstr("\n");
	while(1){
		putstr("Counter:");
		puthex(i);
		setcursor(1,3);
		i++;
		refresh();
	}
	for(;;);
}

