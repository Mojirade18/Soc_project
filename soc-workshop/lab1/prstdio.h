/*
 * prstdio.h - OR1K version
 *
 * Software Excellence from Mixerton Technology.
 *
 * (C) 1995 DJ Greaves.
 *
 */


typedef int size_t;
#define STDIOLEN 256
#define NULL ((void *) 0)
#define uchar unsigned char
#define SYSFDS 32
#define craft_putchar(X) putchar(X)
#define craft_getchar(X) getchar(X)
#define _sa_wrch(X) craft_wrch(X)
#define _sa_rdch(X) craft_rdch(X)
#define O_RDONLY 1
#define O_WRONLY 2
#define EOF -1
#define leaf


extern void *malloc(int);

extern int printf(const char *, ...);
extern int sprintf(char *, const char *, ...);

extern int g_console_flags;
extern char g_deb;
extern void craft_wrch(char);
extern short int craft_testch();
extern char craft_rdch();
extern void spin_delay();


extern void exit(int);
extern int strlen(const char *);
extern int strcmp(const char *s1, const char *s2);
extern int tolower(int);
extern int toupper(int);


typedef struct stdio
{
  int flags;
  int error;
  int bytes;
  char *poi;
  char *index;
  int sysfd;
  char mode;
  char data[STDIOLEN];
} FILE;

extern int fprintf(FILE*, const char *, ...);

extern FILE *stderr, *stdin, *stdout;

/* eof */
