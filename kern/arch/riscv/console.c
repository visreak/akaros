#include <arch/console.h>
#include <pmap.h>
#include <atomic.h>

long
fesvr_syscall(long n, long a0, long a1, long a2, long a3)
{
  volatile uint64_t magic_mem[8];

  magic_mem[0] = n;
  magic_mem[1] = a0;
  magic_mem[2] = a1;
  magic_mem[3] = a2;
  magic_mem[4] = a3;

  mb();
  mtpcr(PCR_TOHOST, PADDR(magic_mem));
  while(mfpcr(PCR_FROMHOST) == 0);
  mb();

  return magic_mem[0];
}

void
fesvr_die()
{
	fesvr_syscall(FESVR_SYS_exit, 0, 0, 0, 0);
}

void
cons_init(void)
{
}

// `High'-level console I/O.  Used by readline and cprintf.

void
cputbuf(const char* buf, int len)
{
	fesvr_syscall(FESVR_SYS_write, 1, PADDR((uintptr_t)buf), len, 0);
}

// Low-level console I/O

void
cons_putc(int c)
{
	if(c == '\b' || c == 0x7F)
	{
	#ifdef __CONFIG_PRINTK_NO_BACKSPACE__
		char buf[2] = {'^', 'H'};
		cputbuf(buf, 2);
	#else
		char buf[3] = {'\b', ' ', '\b'};
		cputbuf(buf, 3);
	#endif /* __CONFIG_PRINTK_NO_BACKSPACE__ */
	}
	else
	{
		char ch = c;
		cputbuf(&ch,1);
	}
}

void
cputchar(int c)
{
	char ch = c;
	cputbuf(&ch,1);
}

int
cons_getc()
{
	char ch;
	uintptr_t paddr = PADDR((uintptr_t)&ch);
	long ret = fesvr_syscall(FESVR_SYS_read, 0, paddr, 1, 0);
	if(ch == 0x7F)
		ch = '\b';
	return ret <= 0 ? 0 : ch;
}

int
getchar(void)
{
	int c;

	while ((c = cons_getc()) == 0)
		/* do nothing */;
	return c;
}

int
iscons(int fdnum)
{
	// used by readline
	return 1;
}
