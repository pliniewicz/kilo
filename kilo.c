#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void disableRawMode(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode(){

	tcgetattr(STDIN_FILENO, &orig_termios); // get attributes of original mode

	atexit(disableRawMode); // when exiting the program, call disable function

	struct termios raw = orig_termios; // create a struct from termios && copy orig into the raw struct

	tcgetattr(STDIN_FILENO, &raw); // read terminal attributes

	/*******************************************************************************************************************
	*  bitwise NOT (~) on echo, which is a bitwise map and then bitwise AND with flags to change only the fourth bit 	 *
	********************************************************************************************************************/

	raw.c_lflag &= ~(ECHO); // modify not to print, what user is typing; c_lflag is "local flag" or "miscellaneous flag"

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // apply changes in attributes (TCSAFLUSH is when to apply the changes)
}

int main()
{
	enableRawMode(); // call raw mode
	char c;
	while	(read(STDIN_FILENO, &c, 1) == 1 && c != 'q'); // listen to input until 'q' char appears, otherwise load it into 'c' variable
	return 0;
}
