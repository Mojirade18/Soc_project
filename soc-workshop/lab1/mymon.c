/*
 * mymon.c
 * CBG Generic Monitor
 * (C) 1995-2010 DJ Greaves.
 *
 */

#include "prstdio.h"
// #include "../../spr_defs.h"
// #include "../../socdam-backdoor-defs.h"

#define DB(X) 
#define IHEX_SUPPORT 0


extern char *readcookedline();
extern char **shell_parse();
extern void writes(const char *);
extern void init_mylinein();
extern void emulator_exit_via_backdoor(char *msg, int return_value);


int last_addr;
char g_exiting = 0;
#define MAXARG (16)

#if IHEX_SUPPORT
#define IHEX_MAX_LENGTH 64
char ihex_buffer[IHEX_MAX_LENGTH];
uchar *g_ihex_offset;
#endif


//char *argv[MAXARG];
char *inpoi;

// Why two of these? Please tidy.
int g_errorf = 0;
int g_download_error_flag = 0;


//
#define CODE_BASE       0


#define READ_PID_REG(X)  101 // For now

// Prototypes
int rhex(char *);
int mymon_main();
int mymon_dispatch(char **argv);
void sayprompt();
int main()
{
  printf("Starting on core %i\n", READ_PID_REG(0));
  // Only run the monitor on core 0
  if (READ_PID_REG(0) == 0) mymon_main();
  else 
    {
      while(1) spin_delay(); 
    }
}

  

//
//
//
int mymon_help(char **argv)
{
  printf("MYMON commands are:\n");
  printf("erase  saddr [ eaddr ]         erase flash section\n");
 
  printf("sum [ saddr eaddr ]            print checksum - defaults to region\n");

  printf("md addr [ lines ]              memory display in hex/ascii\n");
  printf("md addr [ lines ]              memory display in hex/ascii\n");
  printf("mdw addr [ lines ]             as md but displays in 32 bit words\n");
  printf("mm b/w addr                    interactive memory update\n");

  printf("ihw addr d                     write one byte (to flash)\n");
  printf("mmr b/w addr [ times ]         memory read loop\n");
  printf("mmw b/w addr [ times ]         memory write loop\n");
  printf("go addr                        jump\n");
#ifdef DISASSEMBLER_CAP    
  printf("dis b n                        disassemble N lines from base\n");
#endif
#if IHEX_SUPPORT  
  printf("\n:<valid mcs line to store>\n");
#endif
  printf("offset [ v ]                   set or display download offset\n");
  printf("clear                          clear download error flag\n");
  printf("quit                           quit from monitor\n");
  return 0;
}


/*
 * Interactive memory modify
 */
int mymon_mm(char **argv)
{
  char m = tolower(argv[1][0]);
  if (m != 'b' && m != 'w') 
  {
    printf("Syntax   %s b addr    for byte access\n", argv[0]);
    printf("Syntax   %s w addr    for word access\n", argv[0]);
    return -2;
  }
  int b = 1; // Correct?
  if (argv[2]) b = rhex(argv[2]);
  while (1)
  {
    volatile uchar *ba = (uchar *) b;
    volatile int *wa = (int *) b;
    printf("%08X ", b);
    if (m == 'b') printf("%02X >", *ba);
    if (m == 'w') printf("%08X >", *wa);
    argv = shell_parse(readcookedline(0));
    if (argv[0][0] == '.' || argv[0][0] == 'q') return 0;
    if (argv[0][0] == '-' || argv[0][0] == '^') 
    {
      if (m == 'b') b -= 1; else b -= 4;
      continue;
    }
    if (strlen(argv[0])) 
    {
      int e = 0;
      int v = rhex(argv[0]);
      if (m == 'b') *ba = v; else *wa = v;
      if (m == 'b' && *ba != v) e = 1;
      if (m == 'w' && *wa != v) e = 1;
      if (e) {  printf("ERROR "); continue; }
    }
    if (m == 'b') b += 1; else b += 4;
  }
  return 0;
}


/*
 * Tight read/write loop
 */
int mymon_ma(int write, char **argv)
{
  int b = 0;
  int l = 0;  /* 0 is infinite times */
  char m = tolower(argv[1][0]);
  if (m != 'b' && m != 'w') 
  {
    printf("Syntax   %s b addr [times]    for byte access\n", argv[0]);
    printf("Syntax   %s w addr [times]    for word access\n", argv[0]);
    return -2;
  }
  if (argv[2]) b = rhex(argv[2]);
  if (argv[3]) l = rhex(argv[3]);
  if (l) printf("Loop poll of address %08X, %i times \n", b, l);
  else printf("Loop poll of address %08X forever more\n", b);
  g_console_flags = 0;
  while (l != 1)
  {
    volatile int dummy;
    int *wa = (int *) b;
    uchar *ba = (uchar *) b;
    if (g_console_flags) break;
    if (l > 1) l -= 1;
    if (m == b)
    {
      if (write==0) dummy = *ba; else *ba = dummy++;
    }
    else
    {
      if (write==0) dummy = *wa; else *wa = dummy++;
    }
  }
  printf("Done\n");
  return 0;
}



#if FLASH_SUPPORTED
int mymon_erase(char **argv)
{
  unsigned long int s, e, i;
  printf("Flash erase\n");
  if (argv[1])
    {
      s = rhex(argv[1]);
      if (argv[2]) e = rhex(argv[2]); else e = s+1;

      for (i=s; i<e; i+= 0x20000)
	{
	  if (i >= 0x20000 && i < 0x100000) 
	  {
	    printf("Flash block erase %x\n", s);
	    flash_erase(i); /* does odd and even chips */
	  }
        else 
	  {
	    printf ("Illegal erase address %06X\n", i);
	  break;
	  }
	}
    }
  else printf ("need 1 or 2 args\n");
  return 0;
}
#endif


typedef void (*fptr_t)();
int mymon_go(char **argv)
{
  int a;
  if (argv[1])
  {
    a = rhex(argv[1]);
  }
  else a = CODE_BASE;
  printf("Execute at $%08X\n", a);
  fptr_t f = (fptr_t) a;
  f(argv);
  return 0;
}


int mymon_dis(char **argv)
{
  int b = 0;
#ifdef DISASSEMBLER_CAP    
  int l = 20;
  if (argv[1] && argv[1][0] != '.') b = rhex(argv[1]); else b = last_addr;
  if (argv[2]) l = rhex(argv[2]);  
  while (l-- > 0)
  {
    int *x = (int *) b;
    myadis(*x, b);
    b += 4;
  }
#else
  printf("Disassembler not compiled\n");
#endif
  last_addr = b;
  return 0;
}

    
int mymon_md(char **argv)
{
  int b = 0;
  int l = 1;
  if (argv[1] && argv[1][0] != '.') b = rhex(argv[1]); else b = last_addr;
  if (argv[2]) l = rhex(argv[2]);  
  if (l <= 0) l = 1;
  if (l > 100) l = 100;  
  while (l-- > 0)
  {
    int i;
    uchar *x = (uchar *) b;
    printf("%X  ", b);
    for  (i=0; i<16; i++)
    {
      printf("%02X ", x[i]);
      if ((i & 3) == 3) printf(" ");
    }
    printf("  ");
    for (i=0; i<16; i++)
    {
      uchar t = x[i];

      if (t <32 || t >= 127) t = '.';
      printf("%c", t);
    }
    printf("\n");
    b += 16;
  }
  last_addr = b;
  return 0;
}
    
int mymon_md_w(char **argv)
{
  int b = 0;
  int l = 1;
  if (argv[1] && argv[1][0] != '.') b = rhex(argv[1]); else b = last_addr;
  if (argv[2]) l = rhex(argv[2]);  
  while (l-- > 0)
  {
    int i;
    int *xw = (int *) b;
    uchar *x = (uchar *) b;
    printf("%X  ", b);
    for (i=0; i<4; i++)
    {
      printf("%08X   ", xw[i]);
    }
    for (i=0; i<16; i++)
    {
      uchar t = x[i];
      if (t <32 || t >= 127) t = '.';
      printf("%c", t);
    }
    printf("\n");
    b += 16;
  }
  last_addr = b;
  return 0;
}


void code_checksum(uchar *s, uchar *e)
{
  int sum = 0;
  while (s <= e)
    {
      sum += *s++;
    }
  printf ("Checksum = 0x%08X\n", sum);
}


int mymon_sum(char **argv)
{
#if CODE_BASE
  long int s = CODE_BASE;
  long int e = CODE_END;
#else
  long int s = 0;
  long int e = 0;
#endif

  if (argv[1])
    {
      s =  rhex(argv[1]);
      if (argv[2]) e =  rhex(argv[2]); else e=s+511;
    }
  code_checksum((uchar *) s, (uchar *)e);
  return 0;
}



#if IHEX_SUPPORT
int mymon_offset(char **argv)
{
  if (argv[1]) g_ihex_offset = (uchar *) rhex(argv[1]);
  printf("Offset for hex download 0x%08X\n", (unsigned int)g_ihex_offset);
  return 0;
}

int iread(int len)
{
  int r = 0;
  int i;
  for (i=0; i < len; i++)
  {
    char c = toupper(*inpoi++);
    while (c == ' ') c = toupper(*inpoi++);
    c = (c <= '9') ? c-'0': c-('0'+7);
    r = (r<<4) + c;
  }
  return r;
}



/*
 * Return '*' for ok.
 */
char handle_ihex(char *src)
{
  int llen, laddr, baddr, ltype;
  uchar lsum;
  int wq;
  inpoi = src+1; /* Skip colon */
  llen = iread(2);
  laddr = iread(4);
  baddr = laddr;
  ltype = iread(2);
  if (ltype == 1 && llen == 0) return '@'; /* end of file indication */

  if (ltype == 4 && llen == 2 && laddr == 0) /* offset record */
    {
      uchar d0 = iread(2);
      uchar d1 = iread(2);
      long int o = (d0 << 8) | d1;
      lsum = llen + ltype + d0 + d1;
      lsum += iread(2);
      if (lsum != 0) 
	{ 
	  printf("Checksum error in offset line %s, residue 0x%X (%X)\n", src, lsum, llen);
	  g_errorf = 1;
	  return '3';
	}
      g_ihex_offset = (uchar *) (o << 16);
      return '*';
    }

  if (ltype != 0 || llen > IHEX_MAX_LENGTH)
  {
    g_errorf = 1;
    return '2';
  }
  lsum = llen + laddr + ltype + (laddr >> 8);
  for (wq=0; wq<llen; wq++)
  {
    uchar data = iread(2);
    lsum = lsum + data;
    ihex_buffer[wq] = data;
  }
  lsum += iread(2);
  if (lsum != 0) 
    { 
      printf("Checksum error in line %s, residue 0x%X (%X)\n", src, lsum, llen);
      g_errorf = 1;
      return '3';
    }
  ihex_write(g_ihex_offset+baddr, ihex_buffer, llen); 
  if (ihex_verify(g_ihex_offset+baddr, ihex_buffer, llen))
    {
      g_errorf = 1;
      return '0';
    }

  return '*';
}


int ihex_w_test(char **argv)
{
  long int a = rhex(argv[1]);
  char d = rhex(argv[2]);
  ihex_buffer[0] = d;
  if (a >= CODE_BASE) ihex_write((char *) a, ihex_buffer, 1); 
  else printf("Address error\n");
  return 0;
}

int intel_hex(char *t)
{
  if (*t != ':') return -1;
  if (g_errorf == 0) handle_ihex(t);
  return 0;
} 
#endif


int mymon_dispatch(char **argv)
{
  //printf("Dispatch %p %s\n", argv[0], argv[0]);
  if (*argv == NULL || strlen(*argv)==0) return 0;
  if (strcmp(*argv, "?")==0  || strcmp(*argv, "help")==0) return mymon_help(argv);
  if (strcmp(*argv, "md")==0) return mymon_md(argv);
  if (strcmp(*argv, "mdw")==0) return mymon_md_w(argv);
  if (strcmp(*argv, "mmw")==0) return mymon_ma(1, argv);
#if IHEX_SUPPORT
  if (strcmp(*argv, "ihw")==0) return ihex_w_test(argv);
  if (strcmp(*argv, "offset")==0) return mymon_offset(argv);
#endif

/*
  if (strcmp(*argv, "debug")==0) return cmd_set_debug(argv);
*/

#if FLASH_SUPPORTED
  if (strcmp(*argv, "erase")==0) return mymon_erase(argv);
#endif
  if (strcmp(*argv, "sum")==0) return mymon_sum(argv);
  if (strcmp(*argv, "mmr")==0) return mymon_ma(0, argv);
  if (strcmp(*argv, "mm")==0) return mymon_mm(argv);
  if (strcmp(*argv, "go")==0) return mymon_go(argv);
  if (strcmp(*argv, "dis")==0) return mymon_dis(argv);
  if (strcmp(*argv, "clear")==0) { g_errorf = 0; g_download_error_flag = 0; return 0; }
  if (strcmp(*argv, "quit")==0) { emulator_exit_via_backdoor("mymon", 0); g_exiting = 1; return 0; }
/*
  if (strcmp(*argv, "find")==0) return cmd_find(argv);
*/

#ifdef OST_VERSION
  if (strcmp(*argv, "p")==0) return mymon_atmsend(argv);
  if (strcmp(*argv, "g")==0) return mymon_atmget(argv);
  if (strcmp(*argv, "s")==0) return mymon_status(argv);
  if (strcmp(*argv, "cw")==0) return mymon_control(argv);
#endif
  sayprompt(); printf("Possibly an error >%s< >%s<\n", argv[0], argv[1]?argv[1]:"");
  return 0;
}



void sayprompt()
{
  printf("mymon:");
}


char **g_environment = 0;

int mymon_main(char *msg)
{
#if 0
  // Check byte-enables are working.
  *(volatile unsigned short *)0x20 = 0xDEAD;
  *(volatile unsigned short *)0x22 = 0xBEEF;


  *(volatile unsigned char *)0x10 = 0xDD;
  *(volatile unsigned char *)0x11 = 0xEE;
  *(volatile unsigned char *)0x12 = 0xAA;
  *(volatile unsigned char *)0x13 = 0x00;
#endif
  writes("mymon_starting:");
  printf(">>> %i %i %i %i    %i %i %i %i<<<\n", 1, 2, 3, 4, 5, 6, 7, 8);
  printf("S2 20:%x; 10:%x T1=%x T2=%x\n", *(int *)0x20, *(int *)0x10, 0x22345678, 0x32345678);

#if 0
  if (  *(volatile unsigned char *)0x10 != 0xDD) writes("F1\n");
  if (  *(volatile unsigned short *)0x20 != 0xDEAD) 
    {
      writes("F2\n");
      printf("Short load failed %x\n",   *(volatile short *)0x20);
    }
#endif
  int prompting_mode = 1;
  g_exiting = 0;
  g_errorf = 0;


#if IHEX_SUPPORT
  g_ihex_offset = 0;
#endif
  printf("Mixerton ST.  CBG mymon/mixbug low-level monitor.\n");
  printf("Running on core %i\n", READ_PID_REG(0));
#if 0
  while(1)
    {
      char c = getchar();
      printf("Got %X \n", c);
    }
#endif
  g_download_error_flag = 1;
  init_mylinein();
  while(!g_exiting)
  {
    char *cmd, **argv;
    cmd = readcookedline(prompting_mode);
#if IHEX_SUPPORT
    if (cmd[0] == ':')
    {
      prompting_mode = 0;
      intel_hex(cmd);
      continue;
    }
#endif
    prompting_mode = 1;
    if (g_errorf) printf("\n\nDownload error\n\n");
    DB(printf(">%s<\n", cmd));
    argv = shell_parse(cmd);
    mymon_dispatch(argv);
  }
  return 0;
}

/* End of cbg mymon.c */

