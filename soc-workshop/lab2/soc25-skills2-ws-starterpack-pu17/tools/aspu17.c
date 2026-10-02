/* CBG slider  aspu17 assembler:  Apr 10th 1999.  */

/*
 * This file: aspu17.c
 * (C) 1998, DJ Greaves, University of Cambridge, Computer Laboratory.
 * No responsibility can be accepted for loss or consequential loss.
 * Not for commercial use without license.  
 *
 * 
 */


char *assembler = "PU17a";
 
#include <stdio.h>

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* values of type in an STNODE */

#define EXTERN      5
#define RELOCATABLE 12
#define ABSOLUTE    16
#define DMTO        100
#define UNDEFINED   101
#define SFBIT       22
#define GPREG       23
#define BSFR        24
#define SFR         25
#define BYTEREG     26



#define O_TEXT 1
#define O_BSS 2

#define NEW(X) (X *)(malloc(sizeof(X)))

typedef struct locchain 
{ 
  struct locchain *next; int val; 
} LOCCHAIN;

#define HASHSIZE 4096

typedef struct stnode 
{ 
  int type, value, equatef, blong, bshort;
  char *textual;
  struct stnode *stnext;
  struct locchain *schain;
} STNODE;



typedef struct ip_s
{
  char *name;
  int i, bversion;
  struct ip_s *next;
  void (*execute)();
} IP;


#define m_reg   1
#define m_immed 2
#define m_abs  4
#define m_pre 5
#define m_post  6

#define m_sfr 8

extern void lok();       // Two entry points to 68k specific code.
extern void initspecific();

#define  OBJMAX  10000 /* IHEX max line */
#define  labmax  25  /* Max label length */
#define  linemax 400 /* longest input line */


void objword();
void objbyte();
int eval3();
int verbosef = 0;
int binaryf = 0;
int marmitef = 0;  /* Set this for a.out style output to use with linker */
int rawhexf = 0;

char lable[OBJMAX], inst[OBJMAX], token[OBJMAX], etoken[OBJMAX];
int cards;        /* Total lines read */
int src_line_no = -1;      /* line number */
char *currentsrc, *oldsrc;

static int copyf; /* Zero during null'd parts of conditional assembly */
int baddr[2]  ;      /* object address vector */
int seclen[2];       /* length of text and bss sections */
int object[OBJMAX];      /* bcpl string containing object bytes */
int pass;        /* 1 or 2 */

char *jobname;   /* string containing filename */
int eos, running;/* boolean flags end of src */
int errors    ;  /* error counter */
char *error_type;  /* zero for Ok */
int tab;  /* index for inst array */
int tvalue;  /* type of instruction */
char ech;  /* as ch but for eval */
int imed;  /* boolean set by # sign */
int oldline;     /* line in primary src file */
char * gnem; /* Current nemonic info line */
int readonlyopt = 0; /* Set if object file must not update itself */

struct stnode *wordnode, **spoi, **stab_start, *lhsnode;

void writeline(FILE *ss);


FILE  *srccis, *oldcis, *objcis, *listcis;

struct
{
  int iv;
  char *ss;
  int tab ;  /* point of error */
} gaserror_sup;  /* Additional error information - pre vprintf! */

int xaddr     ;  /* memory allocation skip */
int equate    ;  /* boolean flag for address print */

int relorigin ;  /* boolean flag for relocatable text */
int bss_relorigin ;  /* boolean flag for relocatable bss */
int equateval ;  /* Value assigned in an equate */
int output = O_TEXT;  /* Type of code being generated */


void breakpoint()
{
  ;
}


void gaserror(char *e)
{ 
  /*
  writeline(stdout);
  printf("e %s\n", e);
  */
  if (!error_type)
    {
      error_type = e;
      gaserror_sup.tab = tab;
      gaserror_sup.ss = 0;
    }
}

void gaserror_i(char *e, int iv)
{ 
  if (!error_type)
    {
      error_type = e;
      gaserror_sup.tab = tab;
      gaserror_sup.iv = iv;
    }
}

void gaserror_s(char *e, char *ss)
{ 
  if (!error_type)
    {
      error_type = e;
      gaserror_sup.tab = tab;
      gaserror_sup.ss = ss;
    }
}

char *inpoi;
char inbuffer[linemax];

char mrdch()
{
  char c = *(inpoi++);
  if (c == 9) c = ' ';
  return c;
}


void readline() 
{
  int i;
  char ch;
  if (feof(srccis))
    {
      eos = 1;
      return;
    }
  fgets(inbuffer, linemax, srccis);
  inpoi = inbuffer;
  ch = mrdch();
  cards ++;
  src_line_no += 1;
  lable[0] = (char) 0;
  inst[0] = (char) 0;
  if (ch == '\n')  
    {
      return;
    }
  
/* read lable */
  if (ch !=' ' && ch != ';' && ch != '\n' && ch != '*')
    {  
	i = 0;
        while (isalnum(ch) || ch == '_')
	  {         
	    if (ch != (char) 13) lable[i++] = ch;
            ch = mrdch(); 
	    if (i >= labmax)  
	      {
		gaserror("label too long");
		break;
	      }
          }
        lable[i++] = (char) 0;
    }
  if (ch == ':') ch = mrdch();
  
  while (ch==' ')  ch = mrdch();  /* read instruction or comment */
  
  i = 0;
  while (ch !='\n' && ch != EOF)
    {  
      if (ch != (char) 13) inst[i++] = ch;
      ch = mrdch();
      if (i > OBJMAX)  break;
    }
  inst[i++] = (char) 0;
}

void newline() 
{ 
  printf("\n");
}

void writeline(FILE *ss) 
{
  int i;
  fprintf(ss, "%4i ", src_line_no);
  
  if (lable[0] ==';')
    { fprintf(ss, "%s %s\n", lable, inst);
    return;
    }
  
  if (object[0] > 0)  
    { 
      int j = (object[0] > 4) ? 4 : object[0];
      fprintf(ss, "  %04X ", baddr[output]);
      
      for (i=1; i<=j; i++) fprintf(ss, "%02X", object[i]);
      for (i=j; i<=4; i++) fprintf(ss, "  ");
      fprintf(ss, " ");
    }
  else if (equate)  fprintf(ss, "=%08X         ", equateval);
  else fprintf(ss, "              ");
  
  fprintf(ss, "%25s", lable);
  fprintf(ss, "  %s\n", inst);
  
  if (object[0] > 4)
    {
      fprintf(ss, "            ");
      for (i=5; i <= object[0]; i++)
	{
	  fprintf(ss, "%02X", object[i]);
	}
      fprintf(ss, "\n");
    }
}


int hash(char *s)
{ 
  int r=0;
  while(*s)
  {
    r = r + r + *s;
    s++;
  }
  return r & (HASHSIZE-1);
}

int vlookup(char *lab, int insert)               /* search symbol table */
{  
  int h = hash(lab);
  wordnode = stab_start[h];
  while (wordnode)
    { 
      /*
       printf("<%s> <%s> %i %i \n", lab, wordnode->textual, hash(lab), hash(wordnode->textual));  
       */
      if (strcmp(lab, wordnode->textual)==0)  
	{
	  /* printf("Found %s", wordnode->textual);
	   */
	  return 1;
	}
      wordnode = wordnode->stnext;
    }
  if (insert)
    {
      wordnode = NEW(STNODE);
      wordnode->stnext = stab_start[h];
      stab_start[h] = wordnode;
      wordnode -> textual = strdup(lab);
    }
  return 0;
}


void say_gaserror(FILE *ss)
{ 
  int i;
  errors = errors + 1;
  fprintf(ss, "ERROR------------------------------------------");
  for (i=1; i<= gaserror_sup.tab; i++)  putc('-', ss);
  fprintf(ss, "^\n'%s': ", currentsrc);
  if (gaserror_sup.ss) fprintf(ss, error_type, gaserror_sup.ss);
  else fprintf(ss, error_type, gaserror_sup.iv);
  fprintf(ss, "\n");
}

char nextch(int skip) 
{
  ech = inst[tab++];
  if (skip && (ech == ' ' || ech == (char) 9))  nextch(1);       /* skip leading spaces (recursively) */
  return ech;
}

void ignblank()
{
  while (ech == ' ' || ech == (char) 9) nextch(1);
}

void chcomma() 
{
  char line[2];
  line[0] = ech;
  line[1] = 0;
  if (ech!=',')  gaserror_s("Comma expected not `%s`", strdup(line));
  nextch(1);
}

void checkfor(char c)
{
  char p[2];
  if (ech != c)  
    {
      p[0] = c;
      p[1] = 0;
      gaserror_s(">%c< expected.", strdup(p));
  }
  nextch(1);
}

int aschex(char cch)
{ 
  int ch = toupper(cch);
  return  (ch < '0') ? -1:
    (ch > 'F') ? -1:
    (ch <= '9') ? ch - '0':
    (ch >= 'A') ? ch - 'A' + 10: -1;
}



/* Recursive descent expression evaluation */
int eval9(int dummy) 
{
  if (ech == '$')                 /* HEX CONSTANT */
    { 
      int s=0;
      int df = 0;
      nextch(1);
      while (aschex(ech) >= 0)
	{ 
	  df = 1;
	  s = (s<<4) + aschex(ech);
	  nextch(1);
	}
      if (df == 0) gaserror("Bad char in $ hex constant");
      return s;
    }
  
  if (isdigit(ech))                  /* decimal constant - unless suffixed */
    { 
      int s = 0; 
      int base = 10;
      int starttab = tab;
      while (aschex(ech) >= 0)
	{  
	  s = s*base + aschex(ech);
	  nextch(0);
	  if (ech == 'X' || ech == 'x') 
	    { 
	      base = 16; nextch(1); 
	    }
	  
	}
      if (ech == 'H')  /* H as suffix, means redo again as base 16 from start */
	{
	  s = 0;
	  tab = starttab-1;
	  nextch(1);
	  while (aschex(ech) >= 0)
	    {  
	      s = s*16 + aschex(ech);
	      nextch(1);
	    }
	  nextch(1); /* skip H */
	}
      ignblank();
      return s;
    }
  
  if (ech == '-')                 /* monadic negate */
    {
      nextch(1);
      return 0-eval9(tab);
    }
  
  if (ech == '~')                 /* one's complement */
    {
      nextch(1);
      return -1-eval9(tab);
    }

  if (ech == '.')   /*assembly point for ARX is denoted with dot */
    {
      nextch(1);
      return baddr[output];
    }

    if (ech == '(')                /* expression in parens */
      {
	int r;
        nextch(1);
        r = eval3();
        checkfor(')');
        return r;
      }

    if (ech == '\'')                /* ascii quote char */
      { 
	int r = nextch(0);
        if (nextch(1) != '\'')  gaserror("Mismatched quotes.");
        nextch(1);
        return r;
      }
    
    
    if (ech== '#' || ech ==  '=')       /* immediate addressing flag */
      {
	int js;
	nextch(1);
        js = eval9(tab);
        imed = 1;
        return js;
      }


    if ((isalpha(ech)) || (ech=='_') )       /* symbol look up */
      {
	int i = 0;
        while (isalnum(ech) || ech == '_')
	  {
	    token[i++] = ech;
	    nextch(0);
	  }
        token[i++] = (char) 0;
        ignblank();
        if (vlookup(token, 0))
	  { 
	    int j = wordnode->type;   /* symbol found */
	    if (j==RELOCATABLE || j==EXTERN)
	      {
		if (*spoi) gaserror_s("Doubly relocation for %s", token);
		*spoi = wordnode;
	      }
	    if (j == EXTERN) return 0;   /* Do not give BSS offsets, even if known, since ld adds them */
	    return wordnode->value;      /* return value */
          }
        else
	  { 
	    strcpy(etoken, token);
	    gaserror_s("Symbol <%s> not defined.", etoken);
	    return 0xFFFF;
	  }
      }
    gaserror_s("Unknown thing in expression %s", strdup(inst+tab-1));
    breakpoint();
    return 0xFFFF;
}


int eval5() 
{ 
  return  eval9(tab);
}

int eval4() 
{
  int s = eval5();
  while (ech == '*')  
     {  nextch(1) ;  s = s * eval5();  }
  return s;
 }


int eval3() 
{
  int s = eval4();
  while (ech == '+' || ech == '-')  
  { 
    char k = ech;
    nextch(1); 
    if (k == '+') s = s + eval4();  
    else 
      {
	/* Two relocations cancell in a subtract */
	struct stnode  *old_spoi = *spoi;
	*spoi = NULL;
	s = s - eval4();
	if (*spoi)
	  {
	    if (old_spoi == NULL) gaserror("inverse relocation needed");
	    else *spoi = NULL;
	  }
	else *spoi = old_spoi;
      }
  }
  return s;
}

/* Find data value encoding size */
int sexmeasure(int data) 
{
  int k = (data  >= 0) ? data: -1- data;
  return (k<128) ? 0:              /* byte result */
    (k<32768) ? 1:           /* word */
    2;                       /* long word */
}


int eval(STNODE **lspoi)
{  
  int res;
  spoi = lspoi;
  *spoi = NULL;
  res = eval3();
  return res;
}


void objword(unsigned int w)
{ 
  /* This PU17 is a little-endian beastie */
  objbyte(w);
  objbyte(w>>8);
}

void objbyte(unsigned int b)
{
  int x = object[0];
  object[++x] = b & 0xFF;
  if (x < OBJMAX)
    object[0] = x;
  else gaserror("Too much object code from this single line.");
}


/* Note that an external or relocatable symbol has been used */
void symbnote(struct stnode *wn, int xvalue) 
{ 
  struct locchain *r;
  if (pass!=2) return;
  r = (struct locchain *) malloc(sizeof(struct locchain));
  r->next = wn->schain;
  wn->schain = r;
  r->val = xvalue;
}


void org(int a)
{

  if (output == O_BSS) bss_relorigin = 0; else relorigin = 0;
  baddr[output] = a;
}

void padorg(int a)
{
  int d = a - baddr[output];
  if (d < 0) gaserror("pad end has already been passed.");
  while(d > 0)
    {
      objword(0xC0C0);
      d -= 2;
    }
}

/* put a lable into symbol table */
void lable_handle(char sf)  
{
  if (vlookup(lable, 1))
    { 
      if (pass==1 && sf != 'e') 
      {
	if (wordnode->type == EXTERN)   /* extern actually defined here */
	  { 
	    if (output == O_TEXT)
	      { 
		wordnode->value = baddr[O_TEXT];
		wordnode->type = (relorigin) ? RELOCATABLE: ABSOLUTE;
	      }
	    else /* Convert extern to local bss */
	      {
		wordnode->value = baddr[O_BSS];
		wordnode->equatef = EXTERN;
		wordnode->type = EXTERN;
		
		if (objcis && marmitef) fprintf(objcis, "-B %s %08X\n",  lable, baddr[O_BSS]);
	      }
	  }
	else wordnode-> type  = DMTO;   /* found twice on first pass, dmto */
      }
      else if (pass == 2)
	{
	  if (wordnode->type==DMTO)  gaserror_s("Label defined more than once: %s", lable);
	  if (wordnode->value != baddr[output] && wordnode->equatef == 0)
	    {
	      printf("Label value changed for %s, old=%X new=%X\n", lable, wordnode->value, baddr[output]);
	      printf("type = %i, pass =%i, equatef=%i\n", wordnode->type, pass,
		     wordnode->equatef);
	    }
	}
    }
  else /* Not found before */
  { 
    if (sf == 'e') 
      {
	wordnode->value = 0;
	wordnode->type = EXTERN;
	wordnode->equatef = EXTERN;
      }    
    else if (output == O_TEXT)
      {
	wordnode->value = baddr[O_TEXT];         /* initially set up as code lable */
	wordnode->equatef = 0;           /* So clear equate flag           */
	wordnode->type = (relorigin) ? RELOCATABLE: ABSOLUTE;
      }
    else if (output == O_BSS)
      {
	wordnode->value = baddr[O_BSS];
	wordnode->equatef = 0;
	wordnode->type = (bss_relorigin) ? RELOCATABLE: ABSOLUTE;
	/*
	  if (objcis && !binaryf) fprintf(objcis, "-B %s %08X\n",  lable, baddr[O_BSS]);
	  */
      }
    
    wordnode->schain = NULL;
    wordnode->textual = (char *) malloc(strlen(lable)+1);
    strcpy(wordnode->textual, lable);
  }
}

void objhex(unsigned char c)
{
  char c1 = c >> 4;
  if (c1 > 9) c1 += 7;
  putc(c1 + '0', objcis);
  c1 = c & 15;
  if (c1 > 9) c1 += 7;
  putc(c1 + '0', objcis);
}

void ihex_code_output() 
{
  int i;
  int v = 0;
  unsigned int *p = object+1;
  if (objcis == NULL) return;

#if 0
  if (relorigin && xaddr && output == O_TEXT)  /* Fill uninitialised storage */
    {
      for (i=0; i< xaddr; i++)  fprintf(objcis, "00");
    }
#endif

  if (object[0] == 0) return;
  
  if (output != O_TEXT) 
    {
      int f = 0;
      for (i=1; i<=object[0]; i++) if (object[i]) f = 1;
      if (f) gaserror_i("non-zero code in bss. line no %i", src_line_no);
    }

  while(v < object[0])
    {
      int sum = 0;
      int amount = (object[0]-v > 64) ? 64: object[0]-v;

      fprintf(objcis, ":%02X%04X00", amount, baddr[output]+v);
      sum = sum + amount + (v+baddr[output]) + ((v+baddr[output]) >> 8);
      
      for (i=0; i<amount; i++) 
	{
	  objhex(*p);
	  sum = *p + sum;
	  p++;
	  v += 1;
	}
      fprintf(objcis, "%02X\n", (256 - sum) & 255);
    }
}


void binary_code_output() 
{
  int i;
  if (objcis == NULL) return;
 

  if (relorigin && xaddr && output == O_TEXT)  /* Fill uninitialised storage */
    {
      for (i=0; i< xaddr; i++)  putc(0, objcis);
    }

  if (object[0] == 0) return;
  
  if (output != O_TEXT) gaserror("code generation in data/bss segment");
  
  
  for (i=1; i<=object[0]; i++) 
    {
      putc(object[i], objcis);
    }
}

/*
 * Code output suitable for Verilog-XL to read with $readmemh command
 */
void rawhex_code_output() 
{
  int i;
  if (objcis == NULL) return;
 

  if (relorigin && xaddr && output == O_TEXT)  /* Fill uninitialised storage */
    {
      for (i=0; i< xaddr; i++)  fprintf(objcis, "0x0\n");
    }

  if (object[0] == 0) return;
  
  if (output != O_TEXT) return;

  /* gaserror("code in bss", 0); */
   
  
  for (i=1; i<=object[0]; i++) 
    {
      fprintf(objcis, "0x%02X%02X\n", object[i], object[i+1]);
    }
}


void code_output() 
{
  if (binaryf)
    {
      if (rawhexf) rawhex_code_output();
      else binary_code_output();
    }
  else ihex_code_output();
}


void unstackcis()
{
  if (oldcis == NULL) running = 0;
  else
    { 
      srccis = oldcis;
      oldcis = NULL;
      src_line_no = oldline;
      eos = 0;
      currentsrc = oldsrc;
      readline();
    }
}


void perform_pass()
{ 
  char sf; running = 1;
  output = O_TEXT;
  eos = 0;
  copyf = 1;
  cards = 0;
  seclen[0] = 0; /* No output text yet */
  seclen[1] = 0; /* No output text yet */
  /*  printf("Source: '%s' pass %i.\n", jobname, pass);
   */
  currentsrc = jobname;

  srccis = fopen(jobname, "r");
  if (srccis == NULL) 
    {
      printf("Can't open src file '%s' \n", jobname);
      perror(jobname);
      exit(1);
    }
  src_line_no = 0;
  relorigin = 1;  /* default to relocatable module */
  bss_relorigin = 1;  /* default to relocatable module */
  baddr[O_TEXT] = 0;          /* default origin */
  baddr[O_BSS] = 0;          /* default origin */
  errors = 0;


  while (running)
    {
      error_type = NULL;
      gaserror_sup.ss = 0; 
      object[0] = 0; 
      readline();
      if (eos) unstackcis();
      if (running == 0) break;
      equate = 0;
      xaddr = 0;
      if (lable[0] == ';') continue;
      sf = (char) 0;
      
      if (strncmp(inst, ".EXTERN", 6)==0) sf = 'e';
      if (strncmp(inst, ".GLOBAL", 6)==0) sf = 'g';
      
      if (lable[0] != (char) 0) 
	{
	  lable_handle(sf);
	  lhsnode = wordnode;
	}
      else lhsnode = NULL;
      
      tab = 0;
      lok(); 
      if ((error_type !=NULL) && (pass==2) && listcis != stdout)
        {  
          writeline(stdout);
          say_gaserror(stdout);
        }
      if (listcis != NULL)
        {
	  writeline(listcis);
	  if (error_type != NULL)  say_gaserror(listcis);
        }
      if (copyf)
	{
	  /*     DO NOT ROUND UP
	  if (xaddr & 1) xaddr++;  
	  if (object[0] & 1) objbyte(0); 
	  */
	  error_type = 0;
	  if (pass==2)  code_output();
	  if (error_type) say_gaserror(stdout);
	  baddr[output] = baddr[output] + object[0] + xaddr;
	  seclen[output] += object[0];
	  if ((output == O_BSS)? bss_relorigin: relorigin) seclen[output] +=  xaddr;
	}
    }
  fclose(srccis);
  if (verbosef) fprintf(stderr, "Pass finished: cards = %i\n", cards);
}

#if 0
int chop(int o, int c)           /* check presence of case in opts */
{ 
  return 0;
}
#endif

void list_externs(int x, char *title) 
{
  int h;
  if (objcis == NULL) return;
  fprintf(objcis, title);
  for (h=0; h<HASHSIZE; h++)
    {
      struct stnode *p = stab_start[h];
      while (p)
	{ 
	  if (p->type == x)
	    { 
	      struct locchain *q = p->schain;
	      if (q)  
		{
		  fprintf(objcis, ">%s ", p->textual);
		  while (q)
		    {
		      fprintf(objcis, "%08X ", q->val);
		      q = q->next;
		    }
		  fprintf(objcis, "\n");
		}
	    }
	  p = p->stnext;
	}
    }
}

void usenix(char *toolname)
{
  printf("Usenix is: %s [ -rom ] srcfile objfile [ listfile [ symboldumpfile]]\n\n", toolname);
}
int main(int argc, char **argv)
{  
  int lenfirst;
  int h;
  /* printf("Slider 68 series assemblers.  cbg. \n");
   */
  stab_start = (struct stnode **) malloc(HASHSIZE * sizeof(struct stnode *));
  for (h=0; h<HASHSIZE; h++) stab_start[h] = NULL; 
  initspecific();

  if (argc < 2 || !strcmp(argv[1], "-h") ||  !strcmp(argv[1], "--help") ||  !strcmp(argv[1], "-help")) 
    {
      usenix(argv[0]);
      exit(0);
    }
  while(1)
    {
      if (!strcmp(argv[1], "-rom"))
	{
	  readonlyopt = 1;
	  argv++; argc--;
	  continue;
	}

      if (!strcmp(argv[1], "-verbose"))
	{
	  verbosef = 1;
	  argv++; argc--;
	  continue;
	}

      if (!strcmp(argv[1], "-binary"))
	{
	  binaryf = 1;
	  argv++; argc--;
	  continue;
	}

      if (!strcmp(argv[1], "-marmite"))
	{
	  binaryf = 0;
	  argv++; argc--;
	  continue;
	}

      if (!strcmp(argv[1], "-rawhex"))
	{
	  rawhexf = 1;
	  binaryf = 1; /* sic */
	  argv++; argc--;
	  continue;
	}

      break;
    }

  jobname = strdup(argv[1]);    
  /**====================== */
  /* FIRST PASS */
  /**====================== */
  pass = 1;
  objcis = NULL;
  if (fopen(argv[1], "r")==NULL)
    {
      printf("as: cant open src file ");
      perror(argv[1]);
      exit(1);
    }
  /* Module file */
  if (argc > 2)
    { 
      objcis = fopen(argv[2], "w");
      if (objcis == NULL)
	{
	  perror(argv[2]);
	  exit(1);
	}
      if (!binaryf) fprintf(objcis, "$$ CBG %s assembler job %s\n", assembler, jobname);
    }
  listcis = NULL;
  
  
  perform_pass();
  lenfirst = baddr[O_TEXT];
  
  /**====================== */
  /* 2nd   PASS */
  /**====================== */
  if (verbosef) printf("Second pass\n");
  if (seclen[0] + seclen[1] > 0)
    {
      pass = 2;
      
      if ((objcis && marmitef))
	{
	  if (seclen[O_BSS]) fprintf(objcis, "-BL %08X\n", seclen[O_BSS]);     
	  fprintf(objcis, "-T%08X\n", seclen[O_TEXT]);
	  
	}
      if (argc > 3)
        { 
	  if (strcmp(argv[3], "-") == 0) listcis = stdout;
	
           else listcis = fopen(argv[3], "w");
	  if (listcis == NULL)
	    {
	      perror(argv[3]);
	      exit(1);
	    }
	  fprintf(listcis, "** CBG %s assembler job %s\n", assembler, jobname);
	}
      perform_pass();
      if (marmitef)
	{
	  list_externs(RELOCATABLE, "\n-R\n");
	  list_externs(EXTERN, "\n-E\n");
	}
      if (lenfirst != baddr[O_TEXT])
        {
	  printf("as: %s unequal passes %x  %x \n", argv[1], lenfirst, baddr[O_TEXT]);
	  errors = errors + 1;
        }
    } /* endif textlen > 0 */

  if (listcis != NULL)
    {
      fprintf(listcis, "\n\nERRORS = %i,   SRC CARDS READ = %i\n\n",
	      errors, cards);
      if (listcis != stdout) fclose(listcis);
    }

  /* dump symbols optionally
   */
  if (argc > 4)
    {
      FILE * sss;
      if (!strcmp(argv[4],"-")) sss = stdout;
      else sss = fopen(argv[4], "w");
      if (verbosef) printf("Dumping symbols\n");
      if (listcis == NULL)
	{
	  perror(argv[4]);
	  exit(1);
	}
      for (h=0; h<HASHSIZE; h++)
	{
	  struct stnode *p = stab_start[h];
	  while (p)
	    {
	      fprintf(sss, "%08X %i %s\n", p->value, p->type, p->textual);
	      p = p->stnext;
	    }
	}
      fprintf(sss, "End of lables\n");
      if (sss != stdout) fclose(sss);
    }
  

  /* Close module */
  if (objcis != NULL)
    {
      if (!binaryf) fprintf(objcis, "-%c \n", (errors>0) ? 'X': '-');
      fclose(objcis);
      if (errors) remove(argv[2]);
    }

  /* if (errors == 0) printf("Successful completion.\n");
   */
  return errors;
}


/*
 * Must add a directive for invoking this...
 */
void gas_get()
{
  if (oldcis)
    {
      printf("Nested textual includes or GETs not supported.\n");
      exit(1);
    }
  oldcis = srccis;
  oldline  = src_line_no;
  oldsrc = currentsrc;
  {
    char *n = (char *) malloc(80);
    int i = 0;
    ignblank();
    while (ech != (char) 0 && ech != ';')
      {
	if (ech != (char) 0x22) n[i++] = ech;
	nextch(1);
      }
    n[i++] = (char) 0;
    srccis = fopen(n, "r");
    if (srccis == NULL)
      { 
	printf("Cant open included file '%s'.\n", n);
	exit(1);
      }
    /*   if (pass==1) printf("Reading: %s\n", n);
     */
    currentsrc = n;
  }
} 

void gas_copy(int f)
{
  copyf = f;
}


IP *iphashtab[256];

typedef struct parseitem 
{
  int dpp, mod, val, val1, offset, sign, shift;
  struct stnode *symb;
} PI;





IP *lookupip(char *name, int insertf)
{
  int h = (name[0] + (name[1]<<4)) & 255;
  IP *r = iphashtab[h];
  while(r)
    {
      if(0) printf(">%s< >%s<\n", r->name, name);
      if (!strcmp(r->name, name)) break;
      r = r-> next;
    }
  if (r && insertf) printf("opcode %s dmto\n", name); 
  if (r) return r;
  if (insertf)
    {
      r = NEW(IP);
      r->name = name;
      r->next = iphashtab[h];
      iphashtab[h] = r;
    }
  return r;
}

/* routine looks up nemonics  */
void lok()
{
  IP *ip;
  char nem [132];
  int i = 0;
  char xch = toupper(inst[tab++]);
  while (xch == ' ') xch = toupper(inst[tab++]);
  if (xch == (char) 0) return;       /* blank line */
  if (xch == ';' || xch == '@')  return;           /* comment */
  while(isalpha(xch) || xch == '.')
    {
      nem[i++] = xch;
      xch = toupper(inst[tab++]);
    }
  nem[i] = (char) 0;
  ip = lookupip(nem, 0);
  
  if (ip == NULL) 
    {
      gaserror_s("bad instruction ", strdup(nem));
      printf("bad bog `%s' %s\n", nem, inst);
    }
  else
    {
      nextch(1);
      (ip->execute)(inst+tab-1, ip);
    }
}

/* specific prototypes */
int parse_immed();
int getpling();

int readshift()
{
  if (!strncmp(inst+tab-1, "lsl", 3))
    {
      int v = 0;
      int d;
      nextch(1);      nextch(1);      nextch(1);
      d = parse_immed();
      v = v | d << 3;
      return v;
    }
  if (!strncmp(inst+tab-1, "lsr", 3))
    {
      int v = 1;
      int d;
      nextch(1);      nextch(1);      nextch(1);
      d = parse_immed();
      v = v | d << 3;
      return v;
    }
  if (!strncmp(inst+tab-1, "asr", 3))
    {
      int v = 2;
      int d;
      nextch(1);      nextch(1);      nextch(1);
      d = parse_immed();
      v = v | d << 3;
      return v;
    }
  
  if (!strncmp(inst+tab-1, "rot", 3))
    {
      int v = 3;
      int d;
      nextch(1);      nextch(1);      nextch(1);
      d = parse_immed();
      v = v | d << 3;
      return v;
    }
  
  else gaserror_s("bad shift %s", strdup(tab+inst-1));
  return 0;
}

void parse(PI *ps)
{
  /*  char *parsestart = inst+tab-1; */

  ps->mod = -1;  /* invalid value */
  ps->offset = 0;
  /* find bit addressable things, including register names and register bits 
   */
  if (isalpha(ech))
    {
      int starttab = tab;
      char token[132];
      int i = 0;
      while (isalnum(ech) || (ech=='_') )       /* symbol look up */
      {
	token[i++] = ech; 
	nextch(0);
      }
      token[i++] = (char) 0;
      if (vlookup(token, 0))
	{
   
	  if (wordnode->type == GPREG)
	    {
	      ps->mod = m_reg;
	      ps->val = wordnode->value;
	      ignblank();
	      return;

	    }

	  else if (wordnode->type == SFR)
	    {
	      ps->val = wordnode->bshort;
	      ps->val1 = wordnode->blong;
	      ps->mod = m_sfr;
	      ignblank();
	      return;
	    }
	  /* otherwise, will lookup and find again in expression eval */  
	}
      if (0) printf("Not found %s\n", token);
      tab = starttab-1;
      nextch(1);
      
    }

  /* find pre and post forms */
  if (ech == '[')
    {
      PI p1, p2;
      nextch(1);
      parse (&p1);
      if (p1.mod != m_reg) 
	{
	  /* printf("BRE %i %c\n", p1.mod, ech); */
	  gaserror("base register expected");
	}
      ps->val = p1.val;
      
      if (ech == ']')  /* early close means m_pre with zero index */
	{
	  nextch(1);
	  ps->mod = m_pre;
	  ps->offset = 0;
	}
      else ps->mod = m_pre;

      if (ech == ',') 
	{
	  chcomma();
	  /*
	  if (ech == '-')
	    {
	      ps->sign = 1;
	      nextch(1);
	    }
	  else ps->sign = 0;
	  */

	  parse (&p2);
	  if (p2.mod == m_immed || p2.mod == m_abs)
	    {
	      ps->val1 = -1;
	      ps->offset = p2.val;
	    }
	  else if (p2.mod == m_reg)
	    {
	      ps->val1 = p2.val;
	    }
	  else gaserror("bad index");
	  if (ech == ',')
	    {
	      nextch(1);
	      ps->shift = readshift();
	    }
	  else ps->shift = 0;
	  
	  if (ech == ']')
	    {
	      nextch(1);
	    }
	  else if (ps->mod == m_pre) gaserror("missing close bracket");
	}
      else
	{
	  ps->offset = 0;
	  ps->val1 = -1;
	}
      return;
    }

  if (ech == '#') /* find immediate addressing */
    {
      STNODE *poi;
      ps->mod = m_immed;
      ps->val = eval(&poi);
      return;
    }


  /* otherwise eval it */
  {
    STNODE *poi;
    ps ->mod = m_abs;
    ps ->val = eval(&poi);
    return;
  }
}


/*
 * Bit array of when we decided to go large
 */
int big_notes[65536/32];

int note_big(int mode)
{
  int a = baddr[O_TEXT];
  int bit = a & 31;
  int word = a >> 5;

  if (mode == 1) big_notes[word] |= (1 << bit);
  
  return (big_notes[word]) & ( 1<<bit);
}


int parsereg()
{
  PI p;
  parse(&p);
  if (p.mod != m_reg) gaserror("Register expected");
  return p.val;
}


int parse_immed()
{
  PI p;
  parse(&p);
  if (p.mod != m_immed) gaserror("Immediate value expected");
  return p.val;
}



void pu17idx(int storef, int ireg, int rd, int bytef, int idx)
{
  if (ireg >= 3 && ireg != 7) gaserror_i("only regs 0-2 and 7 can be used for index. Reg=R%i", ireg);
  if (idx & 1 || idx > 128 || idx < -126) gaserror_i("index must be even and between -128 and 126, not %i", idx);
  idx = (idx >> 1) & 0x7F;
  objword(0x8000 |  (bytef<<13)| (storef << 12) | ((ireg & 3)<<10) | (rd<<7) | (idx));
}

void pu17imed(int op, int rd, int val)
{
  if (val > 255 || val < 0) gaserror_i("immediate value out of range 0 to 255", val);
  val = val & 0xFF;
  objword((rd <<7) | (op << 3) | (((val >> 3) & 0x1F) << 10) | (val & 7)); 
}


void i_lodstr(char *v, IP *self)
{
  int op = self->i;
  int rd = parsereg();
  int storef = (op >> 5) & 1;
  int bytef = (op >> 6) & 1;

  PI p;

  chcomma();
  parse(&p);

  if (p.mod == m_immed)
    {
      if (storef) gaserror("store to immediate constant");
      if (p.val & 0xFF00 || error_type || note_big(0))
	{
	  note_big(1);
	  objword(0xF400 + (rd << 7));
	  objword(p.val);
	}
      else pu17imed(12, rd, p.val); 
    }
  else if (p.mod == m_abs)
    {
      op |= 0xD400 | (rd << 7);
      objword(op);
      objword(p.val);
    }
  else if (p.mod == m_pre)
    {
      pu17idx((storef) ?1:0, p.val, rd, bytef, p.offset);
    }
  else 
    {
      printf("%i\n", p.mod);
      gaserror("bad movement addressing mode");
    }
}

void i_addsub_notused(char *v, IP *self)
{
  int op = self->i;
  int rd = parsereg();
  PI p;

  chcomma();
  parse(&p);

  if (p.mod == m_immed)
    {
      pu17imed(op, rd, p.val); 
    }

  else if (p.mod == m_reg)  /* alu reg, reg */
    {
      objword(0xD000 | (op<<3) | (rd << 7) | p.val);
    }
  else if (p.mod == m_pre)
    {
      /* pu17idx old ((op==1) ?2:3, p.val, rd, 0, p.val1); */
    }
  else 
    {
      printf("bad add sub %i\n", p.mod);
      gaserror("bad add/sub addressing mode");
    }
}

/*
 * Predicate for ASL,ASR,LSL function codes.
 */
int isashift(int c)
{
  if (c == 9 || c == 10 || c == 11) return 1;
  return 0;
}

void i_alu(char *v, IP *self)
{
  int op = self->i;
  int rd;
  PI p;
  int bytef = (op >= 128) ? 1: 0;
  op &= 127;


  rd = parsereg();
  chcomma();
  parse(&p);

  if (p.mod == m_reg)  /* alu reg, reg */
    {
      if (isashift(op)) gaserror("shift must be immediate #1");
      if (bytef) gaserror("byte ops not allowed in reg,reg mode"); 
      objword(0xD000 | (op<<3) | (rd << 7) | p.val);
    }
  else if (p.mod == m_immed)
    {
      if (isashift(op) && p.val != 1) gaserror("pu17.0 shift must be immediate #1");
      pu17imed(op, rd, p.val);
    }

  else gaserror("alu: bad addressing mode");
}



void i_null(char *v, IP *self)
{
  /* */
}

/* 
 * Indirect branch 
 */
void i_bx(char *v, IP *self)
{
  int op = self->i;
  int rd = parsereg();
  objword(op | (rd << 7));
}

/* 
 * Implied
 */
void i_imp(char *v, IP *self)
{
  int op = self->i;
   objword(op);
}


void pu_jmpc(int o, int dest)
{
  objword(0xD800 + (o << 2));
  objword(dest);
}

/* Jumps - abs */
void i_jmpc(char *v, IP *self)
{
  STNODE *poi;
  int dest = eval(&poi);
  int o = self->i;
  pu_jmpc(o, dest);
}


void i_align(char *v, IP *self)
{
  STNODE *poi;
  int amount = eval(&poi); 
  int mask = amount - 1;
  int i = 0;
  while ((i+baddr[output])  & mask) { i++; objbyte(0); }
}



/* Jumps - assembler auto select rel or abs */
void i_bcc(char *v, IP *self)
{
  STNODE *poi;
  int dest = eval(&poi);
  int o = self->i;
  int delta = (dest - (baddr[output] + 0)) >> 1;

 
  if (delta > 127 || delta < -128 || error_type || note_big(0)) 
    {
      note_big(1);
      pu_jmpc(o, dest);
    } 
  else
    {
      if (o == 15) delta -= 1; /* bsr delta is one less */
      objword(0xC000 | (o << 8) | (delta & 0xFF));
    }
}


void i_tbsel(char *v, IP *self)
{
  output = self->i;
}


void i_multiple(char *v, IP *self)
{
  int op = self->i;
  int o = 0;

  if (ech == '{')
    {
      nextch(1);
      while (1)
	{
	  int x = parsereg();
	  if (ech == '-')
	    {
	      int y;
	      nextch(1);
	      y = parsereg();
	      while (1)
		{
		  o |= (1 << x);
		  if (x == y) break;
		  if (x < y) x++; else x--;
		}
	    }
	  else o |= (1 << x);
	  if (ech =='}') break;
	  chcomma();
	}
    }
  else gaserror("register list needed in { }");

  objword(op | (o << 2));
}

void i_defs(char *v, IP *self)
{
  int a = atoi(inst+tab-1);
  while (a > 0)
    {
      objbyte(0);
      a -= 1;
    }
}



void i_defw(char *v, IP *self)
{
  while(1)
    {
      STNODE *poi;
      int va = eval(&poi);
      objword(va);
      if (ech == 0 || ech == ';' || ech == '@') return;
      chcomma();
    }
}

void i_defb(char *v, IP *self)
{
  while(1)
    {
      if (ech == (char) 0x22)
	{
	  nextch(0);
	  while (ech)
	    {
	      if (ech == '\\') 
		{
		  nextch(0);
		}
	      objbyte(ech);
	      nextch(0);

	      if (ech == (char) 0x22) break;
	    }
	  nextch(1);
	}
      else
	{
	  STNODE *poi;
	  int va = eval(&poi);
	  objbyte(va);
	}
      if (ech == 0 || ech == ';' || ech == '@') return;
      chcomma();
    }
}

void install_ii(char *name, void (*ex)(), int i)
{
  IP *r = lookupip(name, 1);
  r->name = name;
  r->execute = ex;
  r->i = i;
}


void install_r(char *name, int val, int type)
{
  vlookup(name, 1);
  wordnode -> type = type;
  wordnode -> value = val;
}
   
/* define a special function register - self indicates bit addressable */
void i_regdecl(char *v, IP *self)
{
  PI pi;
  parse(&pi);

  if (pi.mod != m_reg) gaserror("R0-R15 needed");
  if (lhsnode)
    {
      lhsnode ->value = pi.val;
      lhsnode ->type = GPREG;
      lhsnode->equatef = 1;
    }
  else gaserror("missing reg in lab field");


}

/*
 * org directive 
 */
void i_org(char *v, IP *self)
{
  STNODE *ps0;
  int vv = eval(&ps0);

  org(vv);
}

void i_extglob(char *v, IP *self)
{
  ;
}



/*
 * equate directive: set a label to an expression.
 */
void i_equ(char *v, IP *self)
{
  STNODE *ps0;
  int vv = eval(&ps0);

   if (lhsnode)
    {
      if (pass == 1 && lhsnode ->equatef) lhsnode->type = DMTO;
      else
	{
	  lhsnode ->value = vv;
	  lhsnode ->equatef = 1;
	  lhsnode ->type = ABSOLUTE;
	  if (0)printf("Equate set %s %x\n", lhsnode->textual, vv); 
	}
    }
  else gaserror("missing equate label");
}

void initspecific()
{
  int i;
  for (i=0;i<256;i++) iphashtab[i] = NULL;
  install_ii("EQU", i_equ, 0);
  install_ii(".EQU", i_equ, 0);

  install_ii(".ORG", i_org, 0);

  install_ii("LDM", i_multiple, 0xDC00);
  install_ii("STM", i_multiple, 0xDC02);


  install_ii("BEQ", i_bcc, 0);
  install_ii("BNE", i_bcc, 1);
  install_ii("BLT", i_bcc, 2);
  install_ii("BGE", i_bcc, 3);
  install_ii("BGT", i_bcc, 4);
  install_ii("BLE", i_bcc, 5);
  install_ii("BCS", i_bcc, 6);
  install_ii("BCC", i_bcc, 7);
  install_ii("BVS", i_bcc, 8);
  install_ii("BVC", i_bcc, 9);
  install_ii("BRA", i_bcc, 10);
  install_ii("BHI", i_bcc, 11);
  install_ii("BLS", i_bcc, 12);
  install_ii("BMI", i_bcc, 13);
  install_ii("BPL", i_bcc, 14);
  install_ii("BSR", i_bcc, 15);

  install_ii("JEQ", i_jmpc, 0);
  install_ii("JNE", i_jmpc, 1);
  install_ii("JLT", i_jmpc, 2);
  install_ii("JGE", i_jmpc, 3);
  install_ii("JGT", i_jmpc, 4);
  install_ii("JLE", i_jmpc, 5);
  install_ii("JCS", i_jmpc, 6);
  install_ii("JCC", i_jmpc, 7);
  install_ii("JVS", i_jmpc, 8);
  install_ii("JVC", i_jmpc, 9);
  install_ii("JRA", i_jmpc, 10);  /* Unconditional */
  install_ii("JMP", i_jmpc, 10); /* Unconditional */
  install_ii("JHI", i_jmpc, 11);
  install_ii("JLS", i_jmpc, 11);
  install_ii("JMI", i_jmpc, 13);
  install_ii("JPL", i_jmpc, 14);
  install_ii("JSR", i_jmpc, 15);


  install_ii("LOD", i_lodstr, 0x0000);
  install_ii("LODB", i_lodstr, 0x0040);
  install_ii("STR", i_lodstr, 0x0020);
  install_ii("STRB", i_lodstr, 0x0060);

  install_ii("MOV", i_alu, 12);       /* select argbus */
  install_ii("ADD", i_alu, 1);
  install_ii("SUB", i_alu, 2);
  install_ii("ADDC", i_alu, 3);
  install_ii("SUBC", i_alu, 4);
  install_ii("CMP", i_alu, 5);
  install_ii("OR", i_alu, 6);
  install_ii("AND", i_alu, 7);
  install_ii("XOR", i_alu, 8);
  install_ii("ASL", i_alu, 9); /* must have 2nd arg #1 */
  install_ii("LSL", i_alu, 9); /* must have 2nd arg #1 */
  install_ii("ASR", i_alu, 10); /* must have 2nd arg #1 */
  install_ii("LSR", i_alu, 11); /* assembly must have 2nd arg #1, pu ignores the value for now */
  install_ii("TST", i_alu, 13);

 
  install_ii("ADDB", i_alu, 1+128);
  install_ii("SUBB", i_alu, 2+128);
  install_ii("ADDCB", i_alu, 3+128);
  install_ii("SUBCB", i_alu, 4+128);
  install_ii("CMPB", i_alu, 5+128);
  install_ii("ORB", i_alu, 6+128);
  install_ii("ANDB", i_alu, 7+128);
  install_ii("XORB", i_alu, 8+128);
  install_ii("ASLB", i_alu, 9+128);
  install_ii("LSLB", i_alu, 9+128);
  install_ii("ASRB", i_alu, 10+128);
  install_ii("LSRB", i_alu, 11+128);



  /*  install_ii("NOP", i_imp, 0xD000);   move r0 to r0 - alters the flags*/
  install_ii("RET", i_imp, 0xF300); /* branch indirect R6 */
  install_ii("BX", i_bx, 0xF000);
  install_ii("BXL", i_bx, 0xF001);
  

  install_ii(".EXTERN", i_extglob, 0);

  install_ii(".WORD", i_defw, 0);
  install_ii(".DEFW", i_defw, 0);
  install_ii(".BYTE", i_defb, 0);
  install_ii(".DATA", i_defb, 0);
  install_ii(".STRING", i_defb, 0);
  install_ii(".DEFB", i_defb, 0);
  install_ii(".SPACE", i_defs, 0);
  install_ii(".DEFS", i_defs, 0);
  install_ii(".ASCII", i_defb, 0);

  install_ii(".GLOBAL", i_null, 0);
  install_ii(".END", i_null, 0);
  install_ii(".STABS", i_null, 0);
  install_ii(".STABN", i_null, 0);
  install_ii(".STABD", i_null, 0);
  install_ii(".ALIGN", i_align, 0);
  install_ii(".COMM", i_null, 0);
  install_ii(".REQ", i_regdecl, 0);
  install_ii(".TEXT", i_tbsel, O_TEXT);
  install_ii(".BSS", i_tbsel, O_BSS);

  install_r("r0", 0, GPREG);
  install_r("r1", 1, GPREG);
  install_r("r2", 2, GPREG);
  install_r("r3", 3, GPREG);
  install_r("r4", 4, GPREG);
  install_r("r5", 5, GPREG);
  install_r("r6", 6, GPREG);
  install_r("r7", 7, GPREG);

  install_r("R0", 0, GPREG);
  install_r("R1", 1, GPREG);
  install_r("R2", 2, GPREG);
  install_r("R3", 3, GPREG);
  install_r("R4", 4, GPREG);
  install_r("R5", 5, GPREG);
  install_r("R6", 6, GPREG);
  install_r("R7", 7, GPREG);


}


/* end of cbg aspu17.c */

 
