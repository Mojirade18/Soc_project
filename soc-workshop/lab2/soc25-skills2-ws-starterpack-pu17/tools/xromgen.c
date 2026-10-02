/*
 * 
 * CBG ROM generator from Intel hex.  Generates a Xilinx rom and also a Verilog model.
 *
 * (C) 1999 DJG - University of Cambridge, Computer Laboratory.
 *
 * Input formats   Intel Hex
 * Output formats  XNF, Verilog, EDIF to be added.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define uchar unsigned char


const int MAXSIZE = 1234567;

FILE *fd = 0;

// Read len bytes of hex data from open fd.
int iread(int len)
{
  int r = 0;
  int i;
  for (i=0; i < len; i++)
  {
    unsigned char c = getc(fd); // Upper case.
    c = (c <= '9') ? c-'0': c-('0'+7);
    r = (r<<4) + c;
  }
  return r;
}


static char *pp, *base, *limit;

void poke(uchar c)
{
  *pp ++ = c;
}


void verify(uchar c)
{
  uchar d = *pp;
  if (d != c)
    {
      printf("Verify error at 0x%x   %02x  %02x\n", pp-base, c, d);
    }
  pp ++;
  if (pp > limit) 
    {
      printf("Second file is longer");
      exit(1);
    }
}

int bound_log2(unsigned int d)
{
  int r = 0;
  while (d)
    {
      r++;
      d /= 2;
    }
  return r;
}


void verilog_rom_gen(char *fn, int *data_array, char *rom_name, int length, int width)
{
  int i;
  FILE *fd = fopen(fn, "w");
  if (!fd)
    {
      perror("");
      exit(1);
    }
  fprintf(fd, "// \n");
  fprintf(fd, "// XROMGEN OUTPUT %s\n", fn);
  fprintf(fd, "// \n");
  fprintf(fd, "module %s(data, addr);   \n", rom_name);
  fprintf(fd, " \n");
  fprintf(fd, "  output [%i:0] data; \n", width-1);
  fprintf(fd, "  reg [%i:0] data; \n", width-1);
  fprintf(fd, "  input [%i:0] addr; \n", bound_log2(length)-1);
  fprintf(fd, " \n");
  fprintf(fd, "  always @(addr) case (addr)\n");

  for (i=0;i<length;i++)
    {
      fprintf(fd, "   %i:  data = %i'h%0X; \n", i, width, data_array[i]); 
    }
  fprintf(fd, "  default : data = 0; // change to X\n");
  fprintf(fd, "  endcase \n");
  fprintf(fd, "endmodule \n");
  fprintf(fd, "// \n");
  fclose(fd);

}


void dump(char *buffer)
{
  int b = 0;
  int addr = 0xF800;
  while (addr <= 0xFFFF)
    {
      printf("%c\n", 13);
      {
	uchar sum;
	int i;
	int objbytes = 16;
	printf(":%02X%04X00", objbytes, addr);
	sum = objbytes + (addr & 255) + ((addr >> 8) & 255);
	for (i=0; i<objbytes; i++) 
	  {
	    uchar d = buffer[b++];
	    addr ++;
	    printf("%02X", d);
	    sum = d + sum;
	  }
	printf("%02X%c\n", (256 - sum) & 255, 13);
      }
    }
}

// Take data from the buffer and put it in the array.
int repack(int *array, unsigned char *buffer, int length, int width)
{
  int d = 0;
  unsigned char *p = buffer;
  int little_endian_f = 1;
  while (p < buffer+length)
    {
      int s = 0;
      int w = width/8;
      if (little_endian_f)
	{
	  for (int zz=0;zz<w; zz++)
	    {
	      s |= (*p++) << (8 * zz); // Little-endian pack
	    }
	}
      else
	{
	  while (w-- > 0)
	    {
	      s = (s << 8) + (*p++); // Big-endian pack
	    }
	}
      
      array[d++] = s;
    }
  return d;
}

int hexrun(char *fn, void (*f)(uchar));

int main(int argc, char ** argv)
{
  char *progname = argv[0];
  char *vfn = NULL;
  char *xfn = NULL;
  char *rom_name = "ROMNAME";
  while (argc > 3)
    {
      if (!strcmp(argv[1], "-x"))
	{
	  xfn = argv[2];
	  argc -=2; argv += 2;
	  continue;
	}
      if (!strcmp(argv[1], "-n"))
	{
	  rom_name = argv[2];
	  argc -=2; argv += 2;
	  continue;
	}
      if (!strcmp(argv[1], "-v"))
	{
	  vfn = argv[2];
	  argc -=2; argv += 2;
	  continue;
	}
      break;
    }
  if (argc < 3)
    {
      printf("usenix:\n %s [ -v verilogfn ] [ -n romname  ] [ -x xnffname ] width fn \n", progname);
      exit(1);
    }
  int width = atoi(argv[1]);
  
  char *buffer = (char *) malloc(MAXSIZE);
  pp = buffer;
  base = buffer;
  limit = buffer + MAXSIZE;
  hexrun(argv[2], poke); // Read the ihex into the buffer.
  int length = pp-buffer;
  pp = buffer;

  int *data_array = (int *) malloc(length * sizeof(int));
  length = repack(data_array, buffer, length, width);

  if (vfn) verilog_rom_gen(vfn, data_array, rom_name, length, width);

  printf("Xrom gen finished, Width %i bits, length %i locations.\n", width, length);
  return 0;
}


int bigsum(unsigned char *l, int b)
{
  int r = 0;
  while (b-- > 0)
    {
      r += *l++;
    }
  return r;
}

int bigsum_fillff(unsigned char *l, int b, int total)
{
  
  int r = 0;
  total -= b;
  while (b-- > 0)
    {
      r += *l++;
    }

  while (total-- > 0) r += 255;

  return r;
}


int hexrun(char *fn, void (*f)(uchar))
{
  int line = 1;
  int addr = -1;
  int bytes = 0;  
  fd = fopen(fn, "r");
  if (fd == 0) 
    {
      printf("%s ", fn);
      perror("cant open input file");
      exit(1);
    }
  while (feof(fd)==0)
  {
    uchar c = getc(fd);
    int llen, laddr, ltype;
    uchar lsum;
    int wq;

    if (c == '\n') line++;
    if (c != ':') continue;
    llen = iread(2);
    laddr = iread(4);
    ltype = iread(2);
/*
    printf("Type %x  addr %x len %x \n", ltype, laddr, llen);
*/
    if (ltype != 0) 
    { 
      printf("Type %i encountered in line %i\n", ltype, line);
      continue;
    }
    if (addr == -1) addr = laddr;
    else if (laddr != addr)
    {
      printf("Disjoint addresses %x and %x in line %i\n",
                   laddr, addr, line);
      addr = laddr;
    }
    lsum = llen + laddr + ltype + (laddr >> 8);
    for (wq=0; wq<llen; wq++)
    {
      uchar data = iread(2);
      lsum = lsum + data;
      f(data);
      bytes += 1;
    }
    lsum = lsum + iread(2);

    if (lsum != 0) 
          fprintf(stderr, "Checksum error in line %i, residue %i\n", line, lsum);    

    addr = addr + llen;
  }
  if (0)
    {
      printf("Length = %i bytes.  Total checksum %08X\n", bytes, bigsum(base, bytes));
      
      printf("Length = %i bytes.  Total checksum for 27C256 %08X\n", bytes, bigsum_fillff(base, bytes, 256 * 1024 / 8));
  }
  return 0;
}


/* eof */

