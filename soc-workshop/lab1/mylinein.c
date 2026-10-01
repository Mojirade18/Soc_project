/*
 * CBG myiolib.c 
 * Terminal input and output and splitter.
 * (C) 1995 DJ Greaves.
 */

#include "prstdio.h"

#define MAXLINE (132)
#define HISTORY (4)

char linebuffer[MAXLINE+1];
char lines[HISTORY][MAXLINE+1];
short int histline, pointer, lineidx;


#define DB(X) 

extern char getchar();
extern int putchar(int );
extern void sayprompt();

#define MAXARG (16)
char *argv[MAXARG];

/* Typing completion static variables. */
char *comp_char;
short int comp_line;

char nextch()
{ 
  char r;
  do 
  { 
    r = linebuffer[pointer++]; 
  } while (r == ' ');
/*
  printf("Nextch %x\n", r);
*/
  return r;
}

char nextch1()
{ 
  return linebuffer[pointer++];
}

void unnextch()
{ 
  if (pointer) pointer -= 1;
}

int digit(c) char c;
{ 
  if (c <'0') return -1;
  if (c <= '9') return c - '0';
  if (c < 'A') return -1;
  if (c <= 'F') return c - 'A' + 10;
  if (c < 'a') return -1;
  if (c <= 'f') return c - 'a' + 10;
  return -1;
}



int rhex(p) char *p;
{ 
  int v = digit(*p++);
  int s = 0;
  while (v >= 0)
  {
    s = (s << 4) + v;
    v = digit(*p++);
  }
  return s;
}




/* cbg solos console cooking routines */

char cookcharin()
{
  char c;
  DB(printf("Calling cooked char in.\n"));
  while(1)
  {
    c = getchar() & 0x7F;   /* Read directly from serial port */
    DB(printf("Calling cooked char in %x.\n", c));
    return c;
  }
}


void cookedchar (c) char c;
{ 
  if (c != '\n')
  {
    putchar(c);
    return;
  }
/*
  if ((p0lflag & LITOUT) == 0) sysputc(13);
*/
  putchar(c);
  return;
 
#ifdef MOREMODE
  if (lineidx > height) return;
  if (lineidx == height)
  {
    printf("--MORE--%c", 13);
    while (1) switch(sgetc(1))
    { case ' ':  lineidx = 0; return;
      case 13: return;
      case 'K':
      case 'k': lineidx = height + 1; return;
      case 'q': case 'Q':
         raise(SIGINT);
    }
  }
  lineidx++;
#endif
}




void lbcopy(src, dest, count) int count; char *src, *dest;
{
  while (count-- > 0)
  {
    *dest++ = *src++;
  }
}

int backlines(int v)
{
  v = (v==0) ? HISTORY-1 : v - 1;
  lbcopy(lines[v], linebuffer, MAXLINE+1);
  return v;
}

int forwardlines(int v)
{
  v = (v==(HISTORY-1)) ? 0 : v + 1;
  lbcopy(lines[v], linebuffer, MAXLINE+1);
  return v;
}

void init_mylinein()
{ 
  int i;
  pointer = 0;
  lineidx = 0;
  for (i=0; i<HISTORY; i++) lines[i][0] = (char) 0;
  histline = 0;
  comp_line = 0;
  comp_char = linebuffer;
}

/* Find a typing completion for tag */
char *findcomp(tag) char *tag;
{
  char *p, *q;
  int start_search_line = comp_line;
  while(1)
  {
    while (*comp_char > ' ') comp_char++;
    while (*comp_char == 0)
    {
      comp_line = (comp_line >= HISTORY-1) ? 0 : comp_line + 1;
      if (comp_line == start_search_line) return ""; /* nothing found */
      comp_char = lines[comp_line];
    }
    while (*comp_char <= ' ') comp_char++; /* advance to start of hist item */
    p = tag;
    q = comp_char;
    while(1)
    {
      if (*p <= ' ') return q;  /* return characters following the tag */
      if (*p != *q) break;
      p++; q++;
    }
  }
}


void shellecho(c) char c;
{
  if ((c >= 32) && (c < 127))
  {
    putchar(c);
    return;
  }
  printf("^%X", c);
}

#ifdef USEIOCTL
struct termio oldtt, newtt;
#endif

char *readcookedline(int prompt)
{
  int tab = 0;
  int t;
  int length = 0;
  int recalpos = histline;
  pointer = 0;

  DB(printf("Calling readcookedline.\n"));
#ifdef USEIOCTL
  ioctl(idev, TCGETA, &oldtt);
  ioctl(idev, TCGETA, &newtt);
  newtt.c_lflag |= RAW;
  ioctl(idev, TCSETAF, &newtt);   
#endif

  lineidx = 0;
  if (prompt) sayprompt();
/*
  printf("%i %i ", histline, recalpos);
*/
  while(1)
  {
    unsigned char c = cookcharin();
    DB(printf("Readcookedline %x.\n", c));
    if (c==27)                     
    {
       do
       {
         c = cookcharin();
       } while (c == 'O' || c == '[');
       if (c == 'D') c = 8;       /* backspace */
       if (c == 'C') c = 9+128;   /* horizontal tab */
       if (c == 'A') c = 11;      /* vertical tab */
       if (c == 'B') c = 10+128;  /* meta-linefeed */
     }
    

    DB(printf("%x ", c));

     switch(c)
     { 
     case ' ':
       if (length == 0) break;  /* ignore leading spaces - helps serial download since you can put in lots of spaces before each line to reduce speed of download */
       
     default:
       if (c < ' ') break;
       DB(printf("shellechoing %x ", c));
       shellecho(c);
       DB(printf("shellechoed ", linebuffer, tab));
       { 
	 int i;
	 for(i=tab; i<length; i++)  /* If typing in line, shuffle up */
	 { 
	   putchar(linebuffer[i]);
	 }
	 for(i=length; i>tab; i--)
	 { 
	   linebuffer[i] = linebuffer[i-1];
	   putchar(8);            /* Restore cursor position */
	 }
       }
       DB(printf("storing at %x[%x] ", linebuffer, tab));
       linebuffer[tab] = c;
       DB(printf("stored %x ", c));
       if (tab < MAXLINE) { tab++; length++; }
       break;
       
     case 11:  case 10+128:       /* vertical tab up or down */
       linebuffer[tab] = 0;
       putchar(0x0D);
       sayprompt();
       
       if (c==11) recalpos = backlines(recalpos);
       else recalpos = forwardlines(recalpos);
       for(t=0; t < MAXLINE; t++)
       { 
	 c = linebuffer[t];
	 if (c) shellecho(c);
	 else break;
       }
       tab = t;
       if (t > length) length = t;
       else while (t < length)       /* space off rest of old line (if any) */
       {
	 t++;
	 putchar(' ');
       }
       while (length > tab)     /* backspace to end of current */
       {
           putchar(8);
           length --;
         }
       break;
       
     case (']' & 31):  /* word and filename completion */
       
     case 9:
     {
       char *eot = &linebuffer[tab];
       char *tag = eot;
       char *completion;
       int j = tab;
       tag--;
       while (*tag > ' ' && j > 0) {  tag--; j--; }
       tag++;
       *eot = (char) 0;
       completion = findcomp(tag);
       while(*completion > ' ')
       {
	 char x = *(completion++);
	 shellecho(x);
	 linebuffer[tab] = x;
	 if (tab < MAXLINE) tab++;
	 if (tab > length) length = tab;
       }
       break;
     }
       
     case 3:
       linebuffer[0] = 0;
       printf("\\\n");
       return linebuffer;
       
       
     case 127:                   /* Delete a character */
       if (tab==0) break;
       putchar(8);
       tab--; length--;
       { int i, backs = length-tab;
	 for (i=tab; i<length; i++)
	 { 
	   linebuffer[i] = linebuffer[i+1];
	   putchar(linebuffer[i]);
	 }
	 putchar(32); /* Space away last char*/
	 for (i=backs; i>=0; i--) putchar(8); /* move cursor back to tab */
       }
       break;
       
     case 8:
       if (tab==0) break;
       putchar(8);
       tab--;
       break;
       
     case 9+128:                 /* forward space h tab */
       c = linebuffer[tab];
       if (tab >= length)       /* If beyond old end of line */
       {
	 c = ' ';               /* to a space. */
	 linebuffer[tab] = c;
       }
       putchar(c);
       if (tab < MAXLINE) tab++;
       if (tab > length) length = tab;
       break;
       
     case 13: 
     case 10:
       /* 
	*/
       /* printf("Last char %x\n", c); */
       linebuffer[length] = (char) 0;
       putchar(10);
       putchar(13);
       if (length)  /* Only save contentful old lines */
       {
	 lbcopy(linebuffer, lines[histline], MAXLINE+1);
	 histline ++;
	 if (histline == HISTORY) histline = 0;
       }
       
       /*
	 ioctl(idev, TCSETAF, &oldtt);
	 */
       return linebuffer;
     }
  }
}


//
// Split up a line 
//
char **shell_parse(char *p)
{
  int i;
/*
  printf(">%s<\n", p);
*/
  for (i=0; i<MAXARG; i++) argv[i] = NULL;
  i = 0;
  while(1)
  {
    char *start;
    while (*p == ' ') p++;
    start = p;
    while (*p != ' ' && *p) p++;
    argv[i] = start;
    i++;
    if (*p == 0) break;
    *p = 0;
    p++;
  }

  //  printf("Parsed >%s< >%s<\n", argv[0], argv[1]);

  return argv;
}



/* End of cbg mylinein.c */
 
