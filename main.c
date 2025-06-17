#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#define GPIO_PORTE_AHB_DIR_R     (*((volatile uint32_t *)0x4005C400))
#define GPIO_PORTE_AHB_AFSEL_R   (*((volatile uint32_t *)0x4005C420))
#define GPIO_PORTE_AHB_DEN_R     (*((volatile uint32_t *)0x4005C51C))
#define GPIO_PORTE_AHB_AMSEL_R   (*((volatile uint32_t *)0x4005C528))
#define GPIO_PORTK_DIR_R         (*((volatile uint32_t *)0x40061400))
#define GPIO_PORTK_DEN_R         (*((volatile uint32_t *)0x4006151C))
#define GPIO_PORTK_DATA_R        (*((volatile uint32_t *)0x400613FC))

#define SYSCTL_RCGCGPIO_R        (*((volatile uint32_t *)0x400FE608))
#define SYSCTL_PRGPIO_R          (*((volatile uint32_t *)0x400FEA08))
#define SYSCTL_RCGCADC_R         (*((volatile uint32_t *)0x400FE638))
#define SYSCTL_PRADC_R           (*((volatile uint32_t *)0x400FEA38))

#define ADC0_PC_R                (*((volatile uint32_t *)0x40038FC4))
#define ADC0_SSPRI_R             (*((volatile uint32_t *)0x40038020))
#define ADC0_ACTSS_R             (*((volatile uint32_t *)0x40038000))
#define ADC0_EMUX_R              (*((volatile uint32_t *)0x40038014))
#define ADC0_SSEMUX3_R           (*((volatile uint32_t *)0x400380B8))
#define ADC0_SSMUX3_R            (*((volatile uint32_t *)0x400380A0))
#define ADC0_SSCTL3_R            (*((volatile uint32_t *)0x400380A4))
#define ADC0_IM_R                (*((volatile uint32_t *)0x40038008))
#define ADC0_ISC_R               (*((volatile uint32_t *)0x4003800C))
#define ADC0_PSSI_R              (*((volatile uint32_t *)0x40038028))
#define ADC0_RIS_R               (*((volatile uint32_t *)0x40038004))
#define ADC0_SSFIFO3_R           (*((volatile uint32_t *)0x400380A8))

#define SYSCTL_PLLFREQ0_R        (*((volatile uint32_t *)0x400FE160))
#define SYSCTL_PLLSTAT_R         (*((volatile uint32_t *)0x400FE168))
#define SYSCTL_PLLFREQ0_PLLPWR   0x00800000

#define NVIC_EN0_R               (*((volatile uint32_t *)0xE000E100))
#define NVIC_PRI4_R              (*((volatile uint32_t *)0xE000E410))

#define GPIO_PORTN_DATA_R (*((volatile unsigned long *)0x4006400C)) // Registro de Datos Puerto N
#define GPIO_PORTN_DIR_R (*((volatile unsigned long *)0x40064400)) // Registro de Dirección Puerto N
#define GPIO_PORTN_DEN_R (*((volatile unsigned long *)0x4006451C)) // Registro de Habilitación Puerto N

#define LEDS_ON 0x03
#define LEDS_OFF 0x00

uint32_t result;

void main(void) {
    SYSCTL_RCGCGPIO_R |= 0x1210; // Habilita reloj para Puertos E y K
    while ((SYSCTL_PRGPIO_R & 0x210) == 0);

    GPIO_PORTE_AHB_DIR_R &= ~0x10; // PE4 como entrada
    GPIO_PORTE_AHB_AFSEL_R |= 0x10; // Función alterna PE4
    GPIO_PORTE_AHB_DEN_R &= ~0x10; // Desactiva digital en PE4
    GPIO_PORTE_AHB_AMSEL_R |= 0x10; // Activa modo analógico PE4

    GPIO_PORTK_DIR_R = 0xFF;
    GPIO_PORTK_DEN_R = 0xFF;
    GPIO_PORTK_DATA_R = 0x00;

    SYSCTL_RCGCADC_R |= 0x01; // Habilita reloj ADC0
    while ((SYSCTL_PRADC_R & 0x01) == 0);

    ADC0_PC_R = 0x01; // 125 ksps
    ADC0_SSPRI_R = 0x0123; // Prioridad alta para SS3
    ADC0_ACTSS_R &= ~0x08; // Desactiva SS3 para configurar
    ADC0_EMUX_R &= ~0xF000; // Trigger por software
    ADC0_SSEMUX3_R = 0x00; // Entrada AIN9
    ADC0_SSMUX3_R = 9; // AIN9 = PE4
    ADC0_SSCTL3_R = 0x0006; // Fin de secuencia e interrupción
    ADC0_IM_R |= 0x08; // Habilita interrupción para SS3
    ADC0_ISC_R = 0x08; // Limpia bandera pendiente
    ADC0_ACTSS_R |= 0x08; // Habilita SS3

    // Prioridad de interrupción: IRQ 17 (ADC0 SS3)
    NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFF1FFF) | 0x00000020; // Prioridad 2
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFF00FFFF) | (0x2 << 21); // Prioridad 2
    NVIC_EN0_R |= 1 << 17; // Habilita interrupción IRQ17 (ADC0 SS3)

    SYSCTL_PLLFREQ0_R |= SYSCTL_PLLFREQ0_PLLPWR;
    while ((SYSCTL_PLLSTAT_R & 0x01) == 0);
    SYSCTL_PLLFREQ0_R &= ~SYSCTL_PLLFREQ0_PLLPWR;

    ADC0_PSSI_R = 0x0008; // Disparo por software SS3
    for (;;) {

        //ADC0_PSSI_R = 0x0008; // Disparo por software SS3
        //__asm("WFI"); // Espera a que la interrupción ocurra

    }
}

void ADC0SS3_Handler(void) {
    ADC0_PSSI_R = 0x0008; // Disparo por software SS3
    //encender_led();
    result = (ADC0_SSFIFO3_R & 0xFFF); // Lee el resultado de la conversión
    GPIO_PORTK_DATA_R = (result >> 4); // Muestra el resultado en Puerto K
    ADC0_ISC_R = 0x0008; // Limpia la bandera de interrupción del SS3
   // apagar_led();
}
