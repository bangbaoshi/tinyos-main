/*
 * Copyright (c) 2016 Eric B. Decker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 * - Neither the name of the Stanford University nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL STANFORD
 * UNIVERSITY OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @author Eric B. Decker <cire831@gmail.com>
 *
 * Vector table for msp432 cortex-m4f processor.
 * Startup code and interrupt/trap handlers for the msp432 processors.
 * initial h/w initilization.  In particular clocks and first stage
 * of timer h/w.  See below for h/w initilization.
 */

#include <stdint.h>
#include <msp432.h>

/*
 * msp432.h find the right chip header (msp432p401r.h) which also pulls in
 * the correct cmsis header (core_cm4.h).
 *
 * If __MSP432_DVRLIB_ROM__ is defined driverlib calls will be made to
 * the ROM copy on board the msp432 chip.
 */

extern uint32_t __data_load__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop__;

/* pull in the type definitions.  won't allocate any space */
SCB_Type                    *__scb;
SCnSCB_Type                 *__scnscb;
SysTick_Type                *__systick;
NVIC_Type                   *__nvic;
ITM_Type                    *__itm;
DWT_Type                    *__dwt;
TPI_Type                    *__tpi;
CoreDebug_Type              *__cd;
MPU_Type                    *__mpu;
FPU_Type                    *__fpu;

RSTCTL_Type                 *__rstctl;
SYSCTL_Type                 *__sysctl;
SYSCTL_Boot_Type            *__sysboot;
CS_Type                     *__cs;
PSS_Type                    *__pss;
PCM_Type                    *__pcm;
FLCTL_Type                  *__flctl;
DMA_Channel_Type            *__dmachn;
DMA_Control_Type            *__dmactl;
PMAP_COMMON_Type            *__pmap;
PMAP_REGISTER_Type          *__p1map;
CRC32_Type                  *__crc32;
AES256_Type                 *__aes256;
WDT_A_Type                  *__wdt;
Timer32_Type                *__t32;
Timer_A_Type                *__ta0;
RTC_C_Type                  *__rtc;
REF_A_Type                  *__ref;
ADC14_Type                  *__adc14;
EUSCI_A_Type                *_uca0;
EUSCI_B_Type                *_ucb0;
FL_BOOTOVER_MAILBOX_Type    *__bomb;
TLV_Type                    *__tlv;


int  main();                    /* main() symbol defined in RealMainP */
void __Reset();                 /* start up entry point */
void __system_init();

void HardFault_Handler() __attribute__((interrupt));
void HardFault_Handler() {
  __bkpt();
  /* If set, make sure to clear:
   *
   * CFSR.MMARVALID, BFARVALID
   */
  while (1) {
    __nop();
  }
}

#ifdef notdef

typedef struct {
  uint32_t rtc_val;
  uint32_t usec_val;
} rtc_usec_t;

#define RTC_SIZE 1024

rtc_usec_t rtc_vals[RTC_SIZE];

uint32_t rtc_idx;
uint32_t ty_usecs;

void T32_INT2_Handler() __attribute__((interrupt));
void T32_INT2_Handler() {
  uint32_t v1, v2;

  TIMER32_2->INTCLR = 0;        /* clear the interrupt */
  ty_usecs++;

  /*
   * also grab the current PS value from the RTC for comparison
   *
   * read twice since Tmilli is async to the main clock
   */
  v1 = RTC_C->PS;
  v2 = RTC_C->PS;
  if (v1 != v2)
    v1 = RTC_C->PS;
  if (v1 != v2) {
    __bkpt();
  }
  rtc_vals[rtc_idx].rtc_val = v1;
  rtc_vals[rtc_idx++].usec_val = ty_usecs;
  if (rtc_idx >= RTC_SIZE)
    rtc_idx = 0;
  BITBAND_PERI(P1->OUT, 0) ^= 1;
}

#endif


void __default_handler()  __attribute__((interrupt));
void __default_handler() {
  __bkpt(1);
}


/*
 * Unless overridded, most handlers get aliased to __default_handler.
 */

void Nmi_Handler()        __attribute__((weak));
//void HardFault_Handler()  __attribute__((weak));
void MpuFault_Handler()   __attribute__((weak));
void BusFault_Handler()   __attribute__((weak));
void UsageFault_Handler() __attribute__((weak));
void SVCall_Handler()     __attribute__((weak));
void Debug_Handler()      __attribute__((weak));
void PendSV_Handler()     __attribute__((weak));
void SysTick_Handler()    __attribute__((weak));

void Nmi_Handler()        { __bkpt(1); while(1) { __nop(); } }
//void HardFault_Handler()  { __bkpt(1); while(1) { __nop(); } }
void MpuFault_Handler()   { __bkpt(1); while(1) { __nop(); } }
void BusFault_Handler()   { __bkpt(1); /* while(1) { __nop(); }*/ }
void UsageFault_Handler() { __bkpt(1); while(1) { __nop(); } }
void SVCall_Handler()     { __bkpt(1); while(1) { __nop(); } }
void Debug_Handler()      { __bkpt(1); while(1) { __nop(); } }
void PendSV_Handler()     { __bkpt(1); while(1) { __nop(); } }
void SysTick_Handler()    { __bkpt(1); while(1) { __nop(); } }

void PSS_Handler()        __attribute__((weak, alias("__default_handler")));
void CS_Handler()         __attribute__((weak, alias("__default_handler")));
void PCM_Handler()        __attribute__((weak, alias("__default_handler")));
void WDT_Handler()        __attribute__((weak, alias("__default_handler")));
void FPU_Handler()        __attribute__((weak, alias("__default_handler")));
void FLCTL_Handler()      __attribute__((weak, alias("__default_handler")));
void COMP0_Handler()      __attribute__((weak, alias("__default_handler")));
void COMP1_Handler()      __attribute__((weak, alias("__default_handler")));
void TA0_0_Handler()      __attribute__((weak, alias("__default_handler")));
void TA0_N_Handler()      __attribute__((weak, alias("__default_handler")));
void TA1_0_Handler()      __attribute__((weak, alias("__default_handler")));
void TA1_N_Handler()      __attribute__((weak, alias("__default_handler")));
void TA2_0_Handler()      __attribute__((weak, alias("__default_handler")));
void TA2_N_Handler()      __attribute__((weak, alias("__default_handler")));
void TA3_0_Handler()      __attribute__((weak, alias("__default_handler")));
void TA3_N_Handler()      __attribute__((weak, alias("__default_handler")));
void EUSCIA0_Handler()    __attribute__((weak, alias("__default_handler")));
void EUSCIA1_Handler()    __attribute__((weak, alias("__default_handler")));
void EUSCIA2_Handler()    __attribute__((weak, alias("__default_handler")));
void EUSCIA3_Handler()    __attribute__((weak, alias("__default_handler")));
void EUSCIB0_Handler()    __attribute__((weak, alias("__default_handler")));
void EUSCIB1_Handler()    __attribute__((weak, alias("__default_handler")));
void EUSCIB2_Handler()    __attribute__((weak, alias("__default_handler")));
void EUSCIB3_Handler()    __attribute__((weak, alias("__default_handler")));
void ADC14_Handler()      __attribute__((weak, alias("__default_handler")));
void T32_INT1_Handler()   __attribute__((weak, alias("__default_handler")));
void T32_INT2_Handler()   __attribute__((weak, alias("__default_handler")));
void T32_INTC_Handler()   __attribute__((weak, alias("__default_handler")));
void AES_Handler()        __attribute__((weak, alias("__default_handler")));
void RTC_Handler()        __attribute__((weak, alias("__default_handler")));
void DMA_ERR_Handler()    __attribute__((weak, alias("__default_handler")));
void DMA_INT3_Handler()   __attribute__((weak, alias("__default_handler")));
void DMA_INT2_Handler()   __attribute__((weak, alias("__default_handler")));
void DMA_INT1_Handler()   __attribute__((weak, alias("__default_handler")));
void DMA_INT0_Handler()   __attribute__((weak, alias("__default_handler")));
void PORT1_Handler()      __attribute__((weak, alias("__default_handler")));
void PORT2_Handler()      __attribute__((weak, alias("__default_handler")));
void PORT3_Handler()      __attribute__((weak, alias("__default_handler")));
void PORT4_Handler()      __attribute__((weak, alias("__default_handler")));
void PORT5_Handler()      __attribute__((weak, alias("__default_handler")));
void PORT6_Handler()      __attribute__((weak, alias("__default_handler")));


void (* const __vectors[])(void) __attribute__ ((section (".vectors"))) = {
//    handler                              IRQn      exceptionN     priority
  (void (*)(void))(&__StackTop__),      // -16          0
  __Reset,                              // -15          1           -3

  Nmi_Handler,                          // -14          2           -2
  HardFault_Handler,                    // -13          3           -1
  MpuFault_Handler,                     // -12          4
  BusFault_Handler,                     // -11          5
  UsageFault_Handler,                   // -10          6
  0,                                    // -9           7
  0,                                    // -8           8
  0,                                    // -7           9
  0,                                    // -6           10
  SVCall_Handler,                       // -5           11
  Debug_Handler,                        // -4           12
  0,                                    // -3           13
  PendSV_Handler,                       // -2           14
  SysTick_Handler,                      // -1           15
  PSS_Handler,                          //  0           16
  CS_Handler,                           //  1           17
  PCM_Handler,                          //  2           18
  WDT_Handler,                          //  3           19
  FPU_Handler,                          //  4           20
  FLCTL_Handler,                        //  5           21
  COMP0_Handler,                        //  6           22
  COMP1_Handler,                        //  7           23
  TA0_0_Handler,                        //  8           24
  TA0_N_Handler,                        //  9           25
  TA1_0_Handler,                        // 10           26
  TA1_N_Handler,                        // 11           27
  TA2_0_Handler,                        // 12           28
  TA2_N_Handler,                        // 13           29
  TA3_0_Handler,                        // 14           30
  TA3_N_Handler,                        // 15           31
  EUSCIA0_Handler,                      // 16           32
  EUSCIA1_Handler,                      // 17           33
  EUSCIA2_Handler,                      // 18           34
  EUSCIA3_Handler,                      // 19           35
  EUSCIB0_Handler,                      // 20           36
  EUSCIB1_Handler,                      // 21           37
  EUSCIB2_Handler,                      // 22           38
  EUSCIB3_Handler,                      // 23           39
  ADC14_Handler,                        // 24           40
  T32_INT1_Handler,                     // 25           41
  T32_INT2_Handler,                     // 26           42
  T32_INTC_Handler,                     // 27           43
  AES_Handler,                          // 28           44
  RTC_Handler,                          // 29           45
  DMA_ERR_Handler,                      // 30           46
  DMA_INT3_Handler,                     // 31           47
  DMA_INT2_Handler,                     // 32           48
  DMA_INT1_Handler,                     // 33           49
  DMA_INT0_Handler,                     // 34           50
  PORT1_Handler,                        // 35           51
  PORT2_Handler,                        // 36           52
  PORT3_Handler,                        // 37           53
  PORT4_Handler,                        // 38           54
  PORT5_Handler,                        // 39           55
  PORT6_Handler,                        // 40           56
  __default_handler,                    // 41           57
  __default_handler,                    // 42           58
  __default_handler,                    // 43           59
  __default_handler,                    // 44           60
  __default_handler,                    // 45           61
  __default_handler,                    // 46           62
  __default_handler,                    // 47           63
  __default_handler,                    // 48           64
  __default_handler,                    // 49           65
  __default_handler,                    // 50           66
  __default_handler,                    // 51           67
  __default_handler,                    // 52           68
  __default_handler,                    // 53           69
  __default_handler,                    // 54           70
  __default_handler,                    // 55           71
  __default_handler,                    // 56           72
  __default_handler,                    // 57           73
  __default_handler,                    // 58           74
  __default_handler,                    // 59           75
  __default_handler,                    // 60           76
  __default_handler,                    // 61           77
  __default_handler,                    // 62           78
  __default_handler                     // 63           79
};


/*
 * Exception/Interrupt system initilization
 *
 * o enable all faults to go to their respective handlers
 *   mpu still hardfaults.
 *   others are caught with __bkpt()
 *
 * Potential issue with PendSV.
 * http://embeddedgurus.com/state-space/2011/09/whats-the-state-of-your-cortex/
 */

#define DIV0_TRAP       SCB_CCR_DIV_0_TRP_Msk
#define UNALIGN_TRAP    SCB_CCR_UNALIGN_TRP_Msk
#define USGFAULT_ENA    SCB_SHCSR_USGFAULTENA_Msk
#define BUSFAULT_ENA    SCB_SHCSR_BUSFAULTENA_Msk
#define MPUFAULT_ENA    SCB_SHCSR_MEMFAULTENA_Msk

void __exception_init() {
  SCB->CCR |= (DIV0_TRAP | UNALIGN_TRAP);
}


void __watchdog_init() {
  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;         // Halt the WDT
}


void __pins_init() {

  /*
   * P1.0 is a red led, turn it on following power on
   * P1.1 is a switch, needs pull up
   * P1.4 is a switch, needs pull up
   * P2.{0,1,2} is a 3 input led.
   */
  P1->OUT = 0x12;               /* P1.1/P1.4 need pull up */
  P1->DIR = 1;
  P1->REN = 0x12;
  P1->IFG = 0;

  P2->OUT = 0;
  P2->DIR = 7;

  /*
   * We bring the clocks out so we can watch them.
   *
   * P4.2 ACLK,   Opx,01 (Output/Port/Val/Sel01)
   * P4.3 MCLK,   Opx,01
   *      RTCCLK, Opx,10
   * P4.4 HSMCLK, Opx,01
   * P7.0 PM_SMCLK, Opx,01
   *
   * 7 6 5 4 3 2 1 0
   * 0 0 0 1 1 1 0 0
   */
  P4->DIR  = 0x1C;
  P4->SEL0 = 0x1C;
  P4->SEL1 = 0x00;
  P7->DIR  = 0x01;
  P7->SEL0 = 0x01;
  P7->SEL1 = 0x00;
}


inline void __fpu_on() {
  SCB->CPACR |=  ((3UL << 10 * 2) | (3UL << 11 * 2));
}

inline void __fpu_off() {
  SCB->CPACR &= ~((3UL << 10 * 2) | (3UL << 11 * 2));
}


/*
 * Debug Init
 *
 * o turn various clocks to periphs when debug halt
 * o enable various fault system handlers to trip.
 * o turn on div0 and unaligned traps
 *
 * SCB->CCR(STKALIGN) is already set (from RESET)
 *
 * Do we want (SCnSCB->ACTLR) disfold, disdefwbuf, dismcycint?
 * disdefwbuf, we set for precise busfaults
 *
 * what about cd->demcr (vc_bits) vector catch
 * see dhcsr for access.
 */

void __debug_init() {
  CoreDebug->DHCSR |= CoreDebug_DHCSR_C_MASKINTS_Msk;
  CoreDebug->DEMCR |= (
    CoreDebug_DEMCR_VC_HARDERR_Msk      |
    CoreDebug_DEMCR_VC_INTERR_Msk       |
    CoreDebug_DEMCR_VC_BUSERR_Msk       |
    CoreDebug_DEMCR_VC_STATERR_Msk      |
    CoreDebug_DEMCR_VC_CHKERR_Msk       |
    CoreDebug_DEMCR_VC_NOCPERR_Msk      |
    CoreDebug_DEMCR_VC_MMERR_Msk        |
    CoreDebug_DEMCR_VC_CORERESET_Msk);

  /* disable default write buffering.  change all busfaults into precise */
  SCnSCB->ACTLR |= SCnSCB_ACTLR_DISDEFWBUF_Msk;

  SYSCTL->PERIHALT_CTL =
    SYSCTL_PERIHALT_CTL_HALT_T16_0      |       /* TA0 TMicro */
    SYSCTL_PERIHALT_CTL_HALT_T16_1      |       /* TA1 TMilli */
    SYSCTL_PERIHALT_CTL_HALT_T16_2      |
    SYSCTL_PERIHALT_CTL_HALT_T16_3      |
    SYSCTL_PERIHALT_CTL_HALT_T32_0      |       /* raw usecs */
    SYSCTL_PERIHALT_CTL_HALT_EUA0       |
    SYSCTL_PERIHALT_CTL_HALT_EUA1       |
    SYSCTL_PERIHALT_CTL_HALT_EUA2       |
    SYSCTL_PERIHALT_CTL_HALT_EUA3       |
    SYSCTL_PERIHALT_CTL_HALT_EUB0       |
    SYSCTL_PERIHALT_CTL_HALT_EUB1       |
    SYSCTL_PERIHALT_CTL_HALT_EUB2       |
    SYSCTL_PERIHALT_CTL_HALT_EUB3       |
    SYSCTL_PERIHALT_CTL_HALT_ADC        |
    SYSCTL_PERIHALT_CTL_HALT_WDT        |
    SYSCTL_PERIHALT_CTL_HALT_DMA
    ;
}

void __ram_init() {
  SYSCTL->SRAM_BANKEN = SYSCTL_SRAM_BANKEN_BNK7_EN;   // Enable all SRAM banks
}


void __flash_init() {
  /*
   * For now turn off buffering, (FIXME) check to see if buffering makes
   * a difference when running at 16MHz
   */
  FLCTL->BANK0_RDCTL &= ~(FLCTL_BANK0_RDCTL_BUFD | FLCTL_BANK0_RDCTL_BUFI);
  FLCTL->BANK1_RDCTL &= ~(FLCTL_BANK1_RDCTL_BUFD | FLCTL_BANK1_RDCTL_BUFI);
}


#define T32_DIV_16 TIMER32_CONTROL_PRESCALE_1
#define T32_ENABLE TIMER32_CONTROL_ENABLE
#define T32_32BITS TIMER32_CONTROL_SIZE
#define T32_PERIODIC TIMER32_CONTROL_MODE

void __t32_init() {
  Timer32_Type *tp = TIMER32_1;

  /* MCLK/16 (1MHz, 1us), enable, freerunning, no int, 32 bits, wrap */
  tp->LOAD = 0xffffffff;
  tp->CONTROL = T32_DIV_16 | T32_ENABLE | T32_32BITS;

  /*
   * Using Ty as a 1 second ticker.
   */
  tp = TIMER32_2;
  tp->LOAD = 1000000;           /* 1 MHz */
  tp->CONTROL = T32_DIV_16 | T32_ENABLE | T32_32BITS | T32_PERIODIC;
}


#define CLK_DCOTUNE 134

/*
 * DCOSEL_3:    center 12MHz (~8 < 12 < 16, but is actually larger)
 * DCORES:      external resistor
 * DCOTUNE:     +134 (0x86), moves us up to 16MHz.
 * ACLK:        LFXTCLK/1       32768
 * BCLK:        LFXTCLK/1       32768
 * SMCLK:       DCO/2           8MHz
 * HSMCLK:      DCO/1           16MHz
 * MCLK:        DCO/1           16MHz
 *
 * LFXTDRIVE:   3 max (default).
 *
 * CLKEN:       SMCLK/HSMCLK/MCLK/ACLK enabled (default)
 *
 * PJ.0/PJ.1    LFXIN/LFXOUT need to be in crystal mode (Sel01)
 *
 * DO NOT MESS with PJ.4 and PJ.5 (JTAG pins, TDO and TDI)
 *
 * Research Fault counts and mechanisms for oscillators.
 * Research stabilization
 * Research CS->DCOERCAL{0,1}
 */

/*
 * CLK_DCOTUNE was determined by running CS_setDCOFrequency(TARGET_FREQ)
 * and seeing what it produced.  We then a) understand and b) make
 * use of the result.  This was from driverlib.  We have observed with a
 * scope clocking at 16MHz.  No idea of the tolerance or variation.
 *
 * DCO tuning is discussed in AppReport SLAA658A, Multi-Frequency Range
 * and Tunable DCO on MSP432P4xx Microcontrollers.
 * (http://www.ti.com/lit/an/slaa658a/slaa658a.pdf).
 *
 * According to https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/411030
 * and page 52 of datasheet (SLAS826E) the DCO with external resistor has a
 * tolerance of worst case +/- 0.6%.  Which gives us a frequency range of
 * 15904000 to 16096000 Hz.
 *
 * We have observed LFXT (crystal) taking ~1.5s to stabilize.  This was
 * timed using TX (Timer32_1) clocking DCOCLK/16 to get 1us ticks.  This
 * assumes the DCOCLK comes right up and is stable.  According to the
 * datasheet (SLAS826E, msp432p401), DCO settling time changing DCORSEL
 * is 10us and t_start is 5 us so we should be good.
 */

#define CLK_DCOTUNE 134

uint32_t lfxt_startup_time;

void __core_clk_init() {
  uint32_t timeout;

  /*
   * only change from internal res to external when dco in dcorsel_1.
   * When first out of POR, DCORSEL will be 1, once we've set DCORES
   * it stays set and we no longer care about changing it (because
   * it always stays 1).
   */
  CS->KEY = CS_KEY_VAL;
  CS->CTL0 = CS_CTL0_DCORSEL_3 | CS_CTL0_DCORES | CLK_DCOTUNE;
  CS->CTL1 = CS_CTL1_SELS__DCOCLK  | CS_CTL1_DIVS__2 | CS_CTL1_DIVHS__1 |
             CS_CTL1_SELA__LFXTCLK | CS_CTL1_DIVA__1 |
             CS_CTL1_SELM__DCOCLK  | CS_CTL1_DIVM__1;

  /*
   * turn on the t32s running off MCLK (16Mhz/16 -> 1MHz) so we can
   * time the turn on of the remainder of the system.
   */
  __t32_init();                   /* rawUsecs */

  /*
   * turn on the 32Ki LFXT system by enabling the LFXIN LFXOUT pins
   * Do not tweak the SELs on PJ.4/PJ.5, they are reset to the proper
   * values for JTAG access.  If you tweak them the debug connection goes
   * south.
   */
  BITBAND_PERI(PJ->SEL0, 0) = 1;
  BITBAND_PERI(PJ->SEL0, 1) = 1;
  BITBAND_PERI(PJ->SEL1, 0) = 0;
  BITBAND_PERI(PJ->SEL1, 1) = 0;

  /* turn on LFXT and wait for the fault to go away */
  timeout = 0;
  BITBAND_PERI(CS->CTL2, CS_CTL2_LFXT_EN_OFS) = 1;
  while (BITBAND_PERI(CS->IFG, CS_IFG_LFXTIFG_OFS)) {
    if (--timeout == 0) {
      CS->IFG;
      CS->STAT;
      __bkpt(0);                /* panic?  what to do, what to do */
    }
    BITBAND_PERI(CS->CLRIFG,CS_CLRIFG_CLR_LFXTIFG_OFS) = 1;
  }
  CS->KEY = 0;
  lfxt_startup_time = -TIMER32_1->VALUE;
}


#define TA_FREERUN      TIMER_A_CTL_MC__CONTINUOUS
#define TA_CLR          TIMER_A_CTL_CLR
#define TA_ACLK1        (TIMER_A_CTL_SSEL__ACLK  | TIMER_A_CTL_ID__1)
#define TA_SMCLK8       (TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_ID__8)

void __ta_init(Timer_A_Type * tap, uint32_t clkdiv) {
  tap->EX0 = TIMER_A_EX0_IDEX__1;
  tap->CTL = TA_FREERUN | TA_CLR | clkdiv;
  tap->R = 0;
}


void __rtc_init() {
  RTC_C_Type *rtc = RTC_C;

  /* write the key, and nuke any interrupt enables */
  rtc->CTL0 = RTC_C_KEY;
  rtc->CTL13 = RTC_C_CTL13_HOLD;
  rtc->PS = 0;
  rtc->TIM0 = 0;
  rtc->TIM1 = 0;
  rtc->DATE = 0;
  rtc->YEAR = 0;
  rtc->CTL0 = 0;                /* close the lock */
}


void __start_timers() {
  /* let the RTC go */
  RTC_C->CTL0 = RTC_C_KEY;
  BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_HOLD_OFS) = 0;
  RTC_C->CTL0 = 0;                /* close the lock */

  /* restart the 32 bit 1MHz tickers */
  TIMER32_1->LOAD = 0xffffffff;
  TIMER32_2->LOAD = 1000000;
}


/**
 * Initialize the system
 *
 * Comment about initial CPU state
 *
 * Desired configuration:
 *
 * LFXTCLK -> ACLK, BCLK
 * HFXTCLK off
 * MCLK (Main Clock) - 16MHz, <- DCOCLK/1
 * HSMCLK (high speed submain) <- DCOCLK/1 16MHz (can be faster than 12 MHz)
 *     only can drive ADC14.
 * SMCLK (low speed submain)   DCOCLK/2, 8MHz (dont exceed 12MHz)
 * SMCLK/8 -> TA0 (1us) -> TMicro
 * ACLK/1 (32KiHz) -> TA1 (1/32768) -> TMilli
 * BCLK/1 (32KiHz) -> RTC
 *
 * Timers:
 *
 * RTCCLK <- BCLK/1 (32Ki)
 * TMicro <-  TA0 <- SMCLK/8 <- DCO/2 (1MHz)
 * TMilli <-  TA1 <- ACLK/1 (32KiHz)
 * rawUsecs<- T32_1 <- MCLK/16 <- DCO/1 32 bit raw usecs
 * rawJiffies<- TA0 <- ACLK/1 (32KiHz) 16 bits wide
 */

void __system_init(void) {
  __exception_init();
  __debug_init();
  __ram_init();
  __flash_init();
  BITBAND_PERI(P1->OUT, 0) = 1;
  __core_clk_init();
  BITBAND_PERI(P1->OUT, 0) = 0;

  __ta_init(TIMER_A0, TA_SMCLK8); /* Tmicro */
  __ta_init(TIMER_A1, TA_ACLK1);  /* Tmilli */
  __rtc_init();
  __start_timers();
}


/*
 * Start-up code
 *
 * Performs the following:
 *   o turns off interrupts (primask)
 *   o copy _data (preinitilized data) into RAM
 *   o zero BSS segment
 *   o move the interrupt vectors if required.
 *   o call __system_init() to bring up required system modules
 *   o call main()
 *   o handle exit from main() (shouldn't happen)
 *
 * leaves interrupts off
 *
 * experiment with configurable/permanent bkpts:
 *      https://answers.launchpad.net/gcc-arm-embedded/+question/248410
 */

void start() __attribute__((alias("__Reset")));
void __Reset() {
  uint32_t *from;
  uint32_t *to;

  __disable_irq();
  __watchdog_init();
  __pins_init();

  from = &__data_load__;
  to   = &__data_start__;;
  while (to < &__data_end__) {
    *to++ = *from++;
  }

  // Fill BSS data with 0
  to = &__bss_start__;
  while (to < &__bss_end__) {
    *to++ = 0;
  }

  __system_init();
  main();
  while (1) {
    __BKPT(0);
  }
}
