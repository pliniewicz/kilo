#include <ctype.h> // : iscntrl
#include <stdio.h> // : printf
#include <stdlib.h> // : atexit?
#include <unistd.h> //
#include <termios.h> // : struct termios

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
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP); // IXON disables C-S and C-Q freezes; input flags!; ICRNL disables translating carriage \r into newline \n, in which case C-M and C-J AND enter give the same signal (10); BRKINT disables break condition causing SIGINT; INPCK is parity checking, doenst apply in modern emulators; ISTRIP stripes last bit of each byte into 0, prob already disabled
	raw.c_oflag &= ~(OPOST); // OPOST turns off output processing (like \n into \r\n but that requires addition of \r in printf)
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); // modify not to print, what user is typing; c_lflag is "local flag" or "miscellaneous flag"; also disables canonical mode via ICANON; ISIG disables C-C and C-Z signals;  IEXTEN disables C-V
	raw.c_cflag |= (CS8);  // sets char size to 8 bits/byte WITH BITWISE OR OPERATOR

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // apply changes in attributes (TCSAFLUSH is when to apply the changes)
}

int main()
{
	enableRawMode(); // call raw mode


	char c;
	while	(read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){  // listen to input until 'q' char appears, otherwise load it into 'c' variable
		if (iscntrl(c)){ // check is 'c' is a control char (0-31&127 ascii)
			printf("%d\r\n", c);
		} else{
			printf("%d ('%c')\r\n", c, c);
		}
	}
	return 0;
}
