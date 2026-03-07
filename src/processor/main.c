#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("Welcome to simple processor emulator!\n");
	if(argc < 2){
		printf("Usage: emulator.out <program.asm>\n");
	}

	return 0;
}
