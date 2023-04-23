#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int main(void)
{
	struct termios term, term_old;
	char password[20] = "my_secret_password";
	char input[20] = {0};

	errno = 0;

	/* get current terminal settings */
	if (tcgetattr(STDIN_FILENO, &term) < 0) {
		perror("tcgetattr");
		return 1;
	}

	term_old = term;  /* save old terminal settings to use later */
	/* turn-off echo */
	term.c_lflag &= ~(ECHO);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
		perror("tcsetattr");
		return 1;
	}

	/* read password */
	fprintf(stderr, "Enter password: ");

	if (!fgets(input, sizeof(input), stdin)) {
		perror("fgets");
		goto restore;
	}

	if (!strncmp(password, input, strlen(password)))
		printf("\nhello!\n");
	else
		printf("\n");

restore:
	/* restore original terminal settings */
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term_old) < 0) {
		perror("tcsetattr");
		return 1;
	}

	return 0;
}
