/* includes */
#include <ctype.h> // : iscntrl
#include <errno.h>
#include <stdio.h> // : printf
#include <stdlib.h> // : atexit?
#include <unistd.h> //
#include <termios.h> // : struct termios
/* defines */
#define CTRL_KEY(k) ((k) & 0x1f)

/* data */
struct termios orig_termios;

/* terminal */
void die(const char *s){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);

	perror(s);
	exit(1);
}

void disableRawMode(){
	if	(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1);
		die("tcsetattr");
}

void enableRawMode(){

	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr"); // get attributes of original mode

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
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr"); // apply changes in attributes (TCSAFLUSH is when to apply the changes)
}

char editorReadKey() {
	int nread;
	char c;
	while ((nread) = read(STDIN_FILENO, &c, 1) != 1) {
		if (nread == -1 && errno != EAGAIN) die("read");
	}
	return c;
}

/* output */
void editorDrawRows(){
	for (int y = 0; y < 29; y++){
		write(STDOUT_FILENO, "~\r\n", 3);
	}
}

void editorRefreshScreen(){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	editorDrawRows();
	write(STDOUT_FILENO, "\x1b[H", 3);
}

/* input */
void editorProcessKeypress() {
	char c = editorReadKey();
	switch(c) {
		case CTRL_KEY('q'):
			write(STDOUT_FILENO, "\x1b[2J", 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
	}
}


/* init */
int main()
{
	enableRawMode(); // call raw mode

	while(1){
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;
}
