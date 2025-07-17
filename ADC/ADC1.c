#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "driverlib/interrupt.h"
#include "inc/tm4c1294ncpdt.h"
float data=0;

void ADC0_Ini(void) {

    SYSCTL_RCGCADC_R |= 0x01;
    while ((SYSCTL_PRADC_R & 0x01) == 0); // Espera a que ADC0 esté listo
    SYSCTL_RCGCGPIO_R |= 0x0010;
    while( (SYSCTL_PRGPIO_R & 0x0010) ==0);

    GPIO_PORTE_AHB_DIR_R &= ~0x20; // PE5 como entrada (bit 3 = 0)
    GPIO_PORTE_AHB_AFSEL_R |= 0X20;   // Función alterna PE5
    GPIO_PORTE_AHB_DEN_R &= ~0X20;    // Desactiva función digital
    GPIO_PORTE_AHB_AMSEL_R |= 0X20;   // Habilita función analógica

    ADC0_PC_R = 0x01;
    ADC0_SSPRI_R = 0x0123;                 // Prioridad: SS3 > SS2 > SS1 > SS0 – [p. 1099]
    ADC0_ACTSS_R &= ~0x0008;            // Disable SS3
    ADC0_EMUX_R = 0x0000;
    ADC0_SAC_R = 0x0;
    ADC0_CTL_R = 0x0;
    ADC0_SSOP3_R = 0x0000;                 // Resultados se guardan en FIFO – [p. 1134]
    ADC0_SSTSH3_R = 0x000;

    ADC0_SSMUX3_R = 0;             // Canal AIN0
    ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 8; // canal AIN8 (PE5)
    ADC0_SSCTL3_R = 0x0006;          // IE0 y END0
    ADC0_IM_R |= 0X08;                // Interrupt enable SS3
    ADC0_ACTSS_R |= 0X0008;             // Enable SS3
    NVIC_EN0_R |= (1 << 17);        // IRQ 14: ADC0SS3

    SYSCTL_PLLFREQ0_R |= SYSCTL_PLLFREQ0_PLLPWR;  // Enciende PLL – [p. 292]
    while((SYSCTL_PLLSTAT_R & 0x01)==0);          // Espera a que PLL alcance estabilidad – [p. 292]
    SYSCTL_PLLFREQ0_R &= ~SYSCTL_PLLFREQ0_PLLPWR; // Apaga PLL (lock se mantiene) – [p. 292]

    ADC0_ISC_R = 0X0008;
    ADC0_PSSI_R |= 0X0008;
}

// ISR: ADC0
void ADC0SS3_Handler(void) {

    ADC0_PSSI_R |= 0X0008;
    data = (ADC0_SSFIFO3_R & 0xFFF);
    ADC0_ISC_R = 0X0008;

}

int main(void) {
    ADC0_Ini();

    IntMasterEnable();


    while (1) {
        __asm("  WFI"); // Espera por interrupción
    }
}
