/*
 *  prlibc.c - minimal C library - please instead use uLIBC (as in the splash benchmark demo) for all larger examples. 
 *
 *
 * (C) 1995 DJ Greaves.
 *
 */

#include  "prstdio.h"

// extern int main(int argc, const char *argv[]);

extern void cbg_uart_wrch(char);
extern char cbg_uart_rdch();

#define craft_wrch(C) cbg_uart_wrch(C)
#define craft_rdch  cbg_uart_rdch

// #include "../../socdam-backdoor-defs.h"
// #include "../../spr_defs.h"

#define TRC(X)
#define SKIP(X)

FILE *stderr, *stdout, *stdin;
int errno;  /* provided as part of stdio library */
char g_deb;

#if 0
int _crt0()
{
  errno = 0;
#if SUPPORT_CMDARGS
  int argc = READ_ARGC(0);
  //int proc_id = READ_PID_REG(0);
  return main(argc, READ_ARGV(0));
#else
  return main(0, 0);
#endif
}
#endif

#ifdef UDP_CONSOLE
extern OutputDevice output_device;
extern void udp_telnet_flush(void);
#else
#define Serial 0
#define output_device Serial
#endif


int putchar(int c)
{
  switch (output_device)
    {
    case Serial:
      _sa_wrch(c);
      break;
#ifdef UDP_CONSOLE
    case UDPIP:
      _sa_wrch(c);
      udp_telnet_flush();
      break;
#endif
    }
  return c;
}

#if 1
void exit(int c)
{
  while (1) continue;
}
#endif

#if 0
void memcpy(char *b2, char *b1, int length)
{
  while(--length >= 0) *(b2++) = *(b1++);
}
#endif

void bcopy(void *src0, void *dest0, int length)
{
  char *src = src0, *dest = dest0;
  while(--length >= 0) *(dest++) = *(src++);
}

int bcmp(void *b10, void *b20, int length)
{
  char *b1 = b10, *b2 = b20;
  while(--length >= 0) if ( *(b1++) != *(b2++)) return 1;
  return 0;
}

void* memset(void* ss, int cc, size_t count)
{
  char* p = (char*)ss;
  while (count)
    {
      *p++ = cc;
      count--;
    }
  return ss;
}

void bzero(void *b10, int length)
{
  char *b1 = b10;
  while(--length >= 0) *(b1++) = (char) 0;
}


// nasty - non-reentrant implementation: cannot be used with threads.
char *spf;
#define pfputc(X) if (spf) { *spf++ = ((char)(X)); } else putchar(X)


/* ARM : */
/*#define UPDATEPOI_ARM(X) { argsused += 1; if (argsused <3) poi-=1; else if (argsused==3) poi = poi+8; else poi += 1; }*/
// #define VA_DECL_T_ARM  __builtin_va_list 
//#define STARTPOI_ARM(V, L)    __builtin_va_start(V, L); /* V = (int)((&L)+3) */
#define STARTPOI_ARM(V, L)  { V = (int*)((&L)+1); }
#define UPDATEPOI_ARM(X) { argsused +=1; poi++; }
//  if (argsused <3) poi-=1; else if (argsused==3) poi = poi+8; else poi += 1; }

// OR1K
#define UPDATEPOI_OR1K(X) { poi += 1; }
#define STARTPOI_OR1K(V, L)  V = (void *) ((&L)+3)

#ifdef OR1K
#define UPDATEPOI(X) UPDATEPOI_OR1K(X)
#define STARTPOI(V, L) STARTPOI_OR1K(V, L)
#else
#define UPDATEPOI(X) UPDATEPOI_ARM(X)
#define STARTPOI(V, L) STARTPOI_ARM(V, L)
#endif


int strcmp(const char *s1, const char *s2)
{
  signed char rr;
  while (1)
    {
      if ((rr = *s1 - *s2++) != 0 || !*s1++)  break;
    }
  return rr;
}


void prstring(const char *s, int prefield, int field)
{
  if (prefield >= 0)  /* prefield positive implies field is MAX length */
  {
    while(*s && field>0) 
    { 
      pfputc(*s++);
      field--; 
      prefield--; 
    }
    while(prefield>0)
    {
      pfputc(' '); prefield--; 
    }
  }
  else
  {
    while(*s)
    {
      pfputc(*s++); field--;
    }
    while(field>0) 
    {
      pfputc(' '); field--;
    }
  }
}

static char printi_zeroflag; // Not very thread safe!

void shi(unsigned int *p, unsigned int d)
{ 
  char r = 0;
  while (*p >= d)
    { r ++;
      *p = *p - d;
    }
  if (r) { printi_zeroflag = 1; }

  if (printi_zeroflag) { pfputc(r + '0'); }
}

void printu(unsigned int x)
{
  if (x == 0)
    { 
      pfputc('0');
      return;
    }
  printi_zeroflag = 0;

   shi(&x, 1000000000); // 10^10 is sufficient for 2^32.
   shi(&x, 100000000);
   shi(&x, 10000000);
   shi(&x, 1000000);
   shi(&x, 100000);
   shi(&x, 10000);
   shi(&x, 1000);
   shi(&x, 100);
   shi(&x, 10);
   shi(&x, 1);
 }

void printi(int x)
{
  if (x < 0)
  {
     pfputc('-');
     if (x == 0x80000000) 
     { 
       prstring("2147483648", 0, 100);
       return;
     }
     x = -x;
  }
  printu(x);
}


void hex1(int x)
{
  x = x & 15;
  pfputc( x>9 ? x+('A'-10): x+'0');
}


void hex2(int x)
{
  x = x & 255;
  hex1(x >> 4);
  hex1(x);
}

void debhex8(int x)
{
  if (g_deb) return;
  hex2(x >> 24);
  hex2(x >> 16);
  hex2(x >> 8);
  hex2(x >> 0);
  pfputc(' ');
}


void printx(unsigned int x, int field)
{
  int i;
  unsigned int r = x >> 8;
#if 0
  if (field > 10)
    {
      debhex8(field);
      pfputc('^');
      field = 8;
    }
#endif
  if (r) printx(r, field-2);
  else for (i=2; i<field; i++) pfputc('0');
  hex2(x);
}

static void dof(const char *s, int *poi)
{
  int i = 0;
  //int zeropad = 0;
  char c = s[0];
  int argsused = 0;
  int prefield;
  int field;
  //  puts("HERE"); putchar(c ? '1':'0'); putchar ('\n');
  while (c)
   { 
     if (c !='%')
     {
       pfputc(c);
       c = s[++i];
       continue;
     }
     field = 0;
     prefield = -1;
 sol:
     {
       uchar t; 
       int tc;
       t = s[++i];
       if (t >= 'a') t = t - 32;
       if (t == 'L') goto sol;

       if (t == '%')
	 {
	   pfputc('%');
	 }
       else
	 {switch(t)
	    { 
	    case 'C': tc = *poi; pfputc(tc);
	      break;
	    case 'S': prstring((char *)(*poi), prefield, field);
	      break;
	    case 'X': printx(*poi, field);
	      break;
	      
	    case '*': field = *poi; UPDATEPOI(poi); goto sol;
	      
	    case 'I':
	    case 'D':
	      printi(*poi);
	      break;
	    case 'U':
	      printu(*poi);
	      break;
	      
	    case '0':
	      //if (field==0) zeropad = 1;
	    case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
	      field = field*10 + t - '0';
	      goto sol;
	      
	    case '.': prefield = field; field = 0; goto sol;
	       
	    default: printf("Bad printf selector '%c' \n", t);
	      
	    }
	  c = s[++i];
	  UPDATEPOI(poi);
	}
     }
   }
}

int fprintf(FILE *f, const char *s,  ...)
{
  spf = NULL;
  int *poi;
  STARTPOI(poi, s);
  dof(s, poi);
  // todo f is ignored
  return 0;
}

// nasty - non-reentrant implementation
int printf(const char *s,  ...)
{
  spf = NULL;
  int *poi;
  STARTPOI(poi, s);
  dof(s, poi);
  return 0;
}


// nasty - non-reentrant implementation
int sprintf(char *string, const char *s, ...)
{
  spf = string;
  int *poi;
  STARTPOI(poi, s);
  dof(s, poi);
  *spf = (char) 0;
  return 0;
}

// ctype.h functions.

int isupper (int c)
{
  if ((c >='A') && (c <= 'Z')) return 1;
  return 0;
}

int isdigit (int c)
{
  if ((c >='0') && (c <= '9')) return 1;
  return 0;
}

int isprint (int c)
{
  if ((c >=' ') && (c <= 126)) return 1;
  return 0;
}



int tolower (int c)
 { if ((c >='A') && (c <= 'Z')) return c+32;
   return c;
 }

int toupper (int c)
{
  if ((c >='a') && (c <= 'z')) return c-32;
   return c;
}

int islower(int c)
 { if ((c >= 'a') && (c <= 'z')) return 1;
   return 0;
 }

int isalnum(int c)
{
   if (c >= 'a' && c <= 'z') return 1;
   if (c >= 'A' && c <= 'Z') return 1;
   if (c >= '0' && c <= '9') return 1; 
   return 0;
}

int isalpha(int c)
{
   if (c >= 'a' && c <= 'z') return 1;
   if (c >= 'A' && c <= 'Z') return 1;
   return 0;
}

int atoi(char *s)
{
  int r = 0;
  int base = 10;
  if (s == NULL) return 0;
  while (*s == ' ' || *s == 0) s++;  /* Skip leading whities */
  while (1)
   { char c = *(s++);
     if (c >= '0' && c <= '9')
      { r = r*base + c - '0';
        continue;
      }
     if (c >= 'a') c = c - 32;
     if (c =='X')
      { base = 16;
        continue;
      }
     if (base >= 10 && c >= 'A' && c <= 'F')
      {
	r = r*base + c - 55;
        continue;
      }
     break;
   }
  return r;
}


#ifdef NOT_USED
int write(int fd, char *buf, int len)
{
  TRC(_sa_wrch(*buf));
  while (len >0)
    {
      _sa_wrch(*buf++);
      len -= 1;
    }
  return 0;
}
#endif


int sa_write(char *buf, int len)
{
  //unsigned char checksum='O';
  TRC(_sa_wrch(*buf));
  while (len >0)
    {
      char c=*buf++;
      if (c == '\n')
	{
	  _sa_wrch(13);
	}
      _sa_wrch(c);
      len -= 1;
    }
#ifdef UDP_CONSOLE
  if (output_device==UDPIP)
    udp_telnet_flush();
#endif
  return 0;
}


#ifdef NOT_USED
int read(int f, char *buf, int len)
{
  char c = _polled_sa_rdch();
  *buf = c;
  return 1;
}
#endif


char getchar()
{
  return _sa_rdch(0);
}

int puts(const char *s)
{
  while (*s)
    {
      char c = *s++;
      putchar(c);
    }
  putchar('\n');
  return 0;
}

void writes(char *s)
{
  while(*s) 
    {
      //craft_wrch('!');
      craft_wrch(*s++);
    }
}




unsigned char *heapbase = (unsigned char *)HEAPBASE;

void *malloc(int size)
{
  size = (size + 3) & (~3);
  void *rr = heapbase;
  if (rr > (void *)HEAPEND) return 0;

  heapbase += size;
  return rr;
}




// end of cbg prlibc.c



