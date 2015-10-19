
void init_hw();
int putc_uart0(const char *s);

 
int c_entry() {

  init_hw();  
  putc_uart0("Test Hello world!\n");

  return 0;
}

void exit(int x) {
  while(1);  // loop forever.
}


// -------------------------------------------------------------------


volatile unsigned char * const UART0DR = (unsigned char *)0xe0004500;

int putc_uart0(const char *s) {
 
  while(*s != '\0') { /* Loop until end of string */

    *UART0DR = (unsigned char)(*s); /* Transmit char */

    //*((char*)0xe0004500) = *s;

    //char __volatile__ *addr2 = (char*)0xe0004500;
    //*addr2=*s;

    s++; /* Next char */
  }
  return 0; 
}

// -------------------------------------------  uboot setup


//-------------------------------------
// From u-boot arch/powerpc/include/asm/processor.h
/* Macros for setting and retrieving special purpose registers */
//typedef signed char s8;
typedef unsigned char u8;

//typedef signed short s16;
//typedef unsigned short u16;

//typedef signed int s32;
typedef unsigned int u32;

//typedef signed long long s64;
typedef unsigned long long u64;
#include "processor.h"
#include "mmu.h"



void write_tlb(u32 mas0, u32 mas1, u32 mas2, u32 mas3, u32 mas7)
{
       	mtspr(MAS0, mas0);
	mtspr(MAS1, mas1);
	mtspr(MAS2, mas2);
	mtspr(MAS3, mas3);
	mtspr(MAS7, mas7);
	asm volatile("isync;tlbwe;msync;isync");
}


void set_tlb(u8 tlb, 
	     u32 epn, 
	     u64 rpn,
	     u8 perms, 
	     u8 wimge,
	     u8 ts, 
	     u8 esel, 
	     u8 tsize, 
	     u8 iprot)
{
  if ((mfspr(SPRN_MMUCFG) & MMUCFG_MAVN) == MMUCFG_MAVN_V1 &&  (tsize & 1)) {
    // this mmu-version does not allow odd tsize values
    return;
  }
  u32 mas0 = FSL_BOOKE_MAS0(tlb, esel, 0);
  u32 mas1 = FSL_BOOKE_MAS1(1, iprot, 0, ts, tsize);
  u32 mas2 = FSL_BOOKE_MAS2(epn, wimge);
  u32 mas3 = FSL_BOOKE_MAS3(rpn, 0, perms);
  u32 mas7 = FSL_BOOKE_MAS7(rpn);

  write_tlb(mas0, mas1, mas2, mas3, mas7);
}


#include "qemu-ppce500.h"
void init_hw() {
  // map ccsrbar
  // at start we ececute from esel = 0, so chose something else..
  set_tlb(1, 
	  CONFIG_SYS_CCSRBAR,      /* v_addr  0xe0000000 see  qemu-ppce500.h */
	  CONFIG_SYS_CCSRBAR,      /* p_addr. nb. same as the v_addr here. */
	  MAS3_SW|MAS3_SR,         /* perm  type=TLB_MAP_IO */
	  MAS2_I|MAS2_G,           /* wimge type=TLB_MAP_IO */
	  0,                       /* ts i.e AS=0 */ 
	  1,                       /* esel (a.k.a tlb_index*/ 
	  BOOKE_PAGESZ_1M,         /* tsize  ie 2^10kB ie 1MB */
	  1);

}
