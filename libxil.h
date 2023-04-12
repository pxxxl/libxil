#pragma once

#include "xgpio.h"
#include "xparameters.h"
#include "xil_io.h"
#include "stdlib.h"
#include "xintc_l.h"
#include "xtmrctr_l.h"
#include "mb_interface.h"

#define XPAR_AXI_INTC_0_BASEADDR 0x41200000

#define TIMER0_FREQ 50
#define TIMER1_FREQ 100
#define SYSTEM_CLOCK_FREQ 100000000
#define TIMER0_RESET_VALUE (SYSTEM_CLOCK_FREQ / TIMER0_FREQ)
#define TIMER1_RESET_VALUE (SYSTEM_CLOCK_FREQ / TIMER1_FREQ)

u8* seg_map() {
    u8* ascii = (u8*)malloc(256);
    int i;
    for (i = 0; i < 256; i++) {
        ascii[i] = 0xff;
    }
    ascii['0'] = 0xc0;
    ascii['1'] = 0xf9;
    ascii['2'] = 0xa4;
    ascii['3'] = 0xb0;
    ascii['4'] = 0x99;
    ascii['5'] = 0x92;
    ascii['6'] = 0x82;
    ascii['7'] = 0xf8;
    ascii['8'] = 0x80;
    ascii['9'] = 0x90;
    ascii['a'] = 0x88;
    ascii['b'] = 0x83;
    ascii['c'] = 0xc6;
    ascii['d'] = 0xa1;
    ascii['e'] = 0x86;
    ascii['f'] = 0x8e;
    return ascii;
}

void init_GPIO(){
    // GPIO Tri

    // Switches 15-0
    // left side high
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI_OFFSET, 0xffff);

    // LEDs 15-0
    // left side high
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI2_OFFSET, 0x0000);

    // Anodes 7-0
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_TRI_OFFSET, 0x00);

    // Cathodes 7-0
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_TRI2_OFFSET, 0x00);

    // BTN D, U, L, R, C
    // D: 
    Xil_Out8(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_TRI_OFFSET, 0xff);

    // LD17(B\G\R), LD16(B\G\R),
    Xil_Out8(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_TRI2_OFFSET, 0x00);


    // Enable GPIO interrupt
    // Enable switch interrupt
    Xil_Out8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_IER_OFFSET, 0x01);    
    // Enable BTN interrupt
    Xil_Out8(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_IER_OFFSET, 0xff);
}

void init_INTC(){
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, 0xffffffff);

    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x01);           // Enable GPIO0 interrupt
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x02);           // Enable GPIO2 interrupt
    //Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x04);           // Enable AXI_Timer interrupt
    //Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x08);           // Enable SPI0 interrupt
    //Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x10);           // Enable SPI1 interrupt
    //Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x20);           // Enable UART0 interrupt
    //Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x40);           // Enable UART1 interrupt
    
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0x03);

    microblaze_enable_interrupts();
}

void init_Timer(){
    Xil_Out32(
        XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET,
        Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET) & (~XTC_CSR_ENABLE_TMR_MASK)
    );

    Xil_Out32(
        XPAR_TMRCTR_0_BASEADDR + XTC_TLR_OFFSET,
        TIMER0_RESET_VALUE
    ); 

    Xil_Out32(
        XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET,
        Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET) | XTC_CSR_LOAD_MASK
    );

    Xil_Out32(
        XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET,
        Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET) | (
            XTC_CSR_ENABLE_TMR_MASK | 
            XTC_CSR_DOWN_COUNT_MASK | (
                XTC_CSR_AUTO_RELOAD_MASK & 
                ~XTC_CSR_LOAD_MASK
            )
        )
    );

    Xil_Out32(
        XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET,
        Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET) | 
        XTC_CSR_INT_OCCURED_MASK | 
        XTC_CSR_ENABLE_INT_MASK
    ); 
}