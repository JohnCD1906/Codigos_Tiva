#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/interrupt.h"

// ================= Variables Globales =================
float temperature = 0;
float temperatureC = 0;
volatile uint8_t counter = 0;
uint8_t contadorTimer = 0;

uint8_t displayDigits[10] ={0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xE7};

// ================= Inicialización GPIO (Display y Botón) =================
void GPIO_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x0300; // Puerto J y K
    while ((SYSCTL_PRGPIO_R & 0x0300) == 0);

    // Display (PK[7:0])
    GPIO_PORTK_DIR_R = 0xFF;
    GPIO_PORTK_DEN_R = 0xFF;
    GPIO_PORTK_DATA_R = 0x00;

    // Botón (PJ0)
    GPIO_PORTJ_AHB_DIR_R &= ~0x01;
    GPIO_PORTJ_AHB_DEN_R |= 0x01;
    GPIO_PORTJ_AHB_PUR_R |= 0x01;
    GPIO_PORTJ_AHB_IS_R &= ~0x01;
    GPIO_PORTJ_AHB_IBE_R &= ~0x01;
    GPIO_PORTJ_AHB_IEV_R &= ~0x01;
    GPIO_PORTJ_AHB_ICR_R = 0x01;
    GPIO_PORTJ_AHB_IM_R |= 0x01;

    NVIC_EN1_R |= (1 << (51 - 32)); // IRQ 51: GPIOJ
}

// ================= Inicialización ADC0 =================
void ADC0_Init(void) {
    SYSCTL_RCGCADC_R |= 0x01;
    while ((SYSCTL_PRADC_R & 0x01) == 0);

    SYSCTL_RCGCGPIO_R |= 0x10; // Puerto E
    while ((SYSCTL_PRGPIO_R & 0x10) == 0);

    GPIO_PORTE_AHB_DIR_R &= ~0x20; // PE5 como entrada
    GPIO_PORTE_AHB_AFSEL_R |= 0x20;
    GPIO_PORTE_AHB_DEN_R &= ~0x20;
    GPIO_PORTE_AHB_AMSEL_R |= 0x20;

    ADC0_PC_R = 0x01;
    ADC0_SSPRI_R = 0x0123;
    ADC0_ACTSS_R &= ~0x08;
    ADC0_EMUX_R = 0x0000;
    ADC0_SSMUX3_R = 0;
    ADC0_SSCTL3_R = 0x06;
    ADC0_IM_R |= 0x08;
    ADC0_ACTSS_R |= 0x08;

    NVIC_EN0_R |= (1 << 17); // IRQ 17: ADC0SS3
    ADC0_ISC_R = 0x08;
    ADC0_PSSI_R |= 0x08;
}

// ================= Inicialización Timer3 =================
void Timer3_Init(void) {
    SYSCTL_RCGCTIMER_R |= 0x08;
    SYSCTL_RCGCGPIO_R |= 0x0080;

    asm("  nop");
    asm("  nop");
    asm("  nop");

    GPIO_PORTH_AHB_DIR_R = 0x01;
    GPIO_PORTH_AHB_DEN_R = 0x01;
    GPIO_PORTH_AHB_DATA_R = 0x00;

    TIMER3_CTL_R = 0x00;
    TIMER3_CFG_R = 0x04;
    TIMER3_TAMR_R = 0x02;
    TIMER3_TAILR_R = 0x1E8480; // (2s) 2,000,000 en decimal
    TIMER3_TAPR_R = 0xFF;      // 16 MHz / (255+1) = 62500 Hz
    TIMER3_ICR_R = 0x01;
    TIMER3_IMR_R |= 0x01;
    NVIC_EN1_R |= (1 << (35 - 32)); // IRQ 35: Timer3A
    TIMER3_CTL_R |= 0x01;
}

// ================= ISR: Botón PJ0 =================
void GPIOJ_Handler(void) {
    int rebote;
    GPIO_PORTJ_AHB_ICR_R = 0x01;
    for (rebote = 0; rebote < 200000; rebote++);
    GPIO_PORTK_DATA_R = displayDigits[counter];
    counter = (counter + 1) % 10;
}

// ================= ISR: Timer3 =================
void Timer3AIntHandler(void) {
    TIMER3_ICR_R = 0x01;
    GPIO_PORTH_AHB_DATA_R ^= 0x01;
    contadorTimer++;
}

// ================= ISR: ADC0 =================
void ADC0SS3_Handler(void) {
    ADC0_PSSI_R |= 0x08;
    temperature = (ADC0_SSFIFO3_R * 3300) / 4096;
    temperatureC = temperature / 10;
    ADC0_ISC_R = 0x08;
}

// ================= main =================
int main(void) {
    GPIO_Init();
    ADC0_Init();
    Timer3_Init();

    IntMasterEnable();

    while (1) {
        __asm("  WFI");
    }
}


