#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	else
	{
		/* We expect a filename in argv[1] */
		FILE * fd = fopen(argv[1], "rb");
		if (fd == NULL)
		{
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(1);
		}

		/* initialize the exception handlers */
		jailvm_exception_handlers_init();
		jailvm_registers_t registers;
		memset(&registers, 0, sizeof(jailvm_registers_t));
		/* read the file by two bytes - not exactly optimal, but will get the job done for testing purposes */
		uint8_t buf[2];
		assert(SSIZE_MAX > 2); // ;)
		while (1)
		{
			if (fseek(fd, (registers.flags & JAILVM_FLAGS_EXCEPTION ? registers.eip : registers.ip), SEEK_SET) != 0)	/* invalid instruction pointer? */
				jailvm_unexpected_eof();
			if (fread(buf, 2, 1, fd) != 1)	/* reached end-of-file */
				exit 0;
			if (setjmp(registers.jailvm_exception_jump) == 0)
				jailvm_instruction_execute(&registers, buf[0], buf[1]);
			else
			{	/* an exception occured */
				if (registers.flags & JAILVM_FLAGS_EXCEPTION)	/* we don't handle stacked exceptions - bail out */
					jailvm_stacked_exception();
				else	registers.flags |= JAILVM_FLAGS_EXCEPTION;
				if (registers.eip == 0)
					jailvm_unhandled_exception();
				else
				{ /* all is well: there's an exception handler that we can (and will) use */ }
			}
		}
		
	}
}

