//El Programa utiliza al DAC para obtener el valor del sensor de temperatura interno
//El disparo del DAC es por software
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#define SYSCTL_RCGCGPIO_R (*((volatile uint32_t *)0x400FE608)) //Registro para habilitar reloj de GPIO
#define SYSCTL_PRGPIO_R (*((volatile uint32_t *)0x400FEA08)) // Registro para verificar si el reloj esta listo (p.499)
#define SYSCTL_RCGCADC_R (*((volatile uint32_t *)0x400FE638)) // Registro para habilitar el reloj al ADC(p. 396)
#define SYSCTL_PRADC_R (*((volatile uint32_t *)0x400FEA38)) // Registro para verificar si el ADC esta listo (p.515)
#define ADC0_PC_R (*((volatile uint32_t *)0x40038FC4)) // Registro para configurar tasa de muestreo (p.1159)
#define ADC0_SSPRI_R (*((volatile uint32_t *)0x40038020)) // Regidtrp para configurar la prioridad del secuenciador (p.1099)
#define ADC0_ACTSS_R (*((volatile uint32_t *)0x40038000)) // Registro para controlar la activación del secuenciador (p. 1076)
#define ADC0_EMUX_R (*((volatile uint32_t *)0x40038014)) // Registro para seleccionar el evento (trigger) que inicia el muestreo en cada secuenciador (p.1091)
#define ADC0_SSEMUX3_R (*((volatile uint32_t *)0x400380B8)) // Registro que selecciona entre las entradas AIN[19:16] o AIN[15:0] (p.1146)
#define ADC0_SSMUX3_R (*((volatile uint32_t *)0x400380A0)) // Registro para configurar la entrada analógica para el Secuenciador 3 (p.1141)
#define ADC0_SSCTL3_R (*((volatile uint32_t *)0x400380A4)) // Registro que configura la muestra ejecutada con el Secuenciador 3 (p.1142)
#define ADC0_IM_R (*((volatile uint32_t *)0x40038008)) // Registro que controla la mascara de interrupciones en secuenciadores (p. 1081)
#define ADC0_ACTSS_R (*((volatile uint32_t *)0x40038000)) // Registro que controla la activación de los secuenciadores (p.1077)
#define ADC0_ISC_R (*((volatile uint32_t *)0x4003800C)) //Registro de estatus y para borrar las condiciones de interrupción del secuenciador (p. 1084)
#define ADC0_PSSI_R (*((volatile uint32_t *)0x40038028)) //Registro que permite al software iniciar el muestreo en los secuenciadores (p. 1102)
#define ADC0_RIS_R (*((volatile uint32_t *)0x40038004)) //Registro muestra el estado de la señal de interrupción de cada secuenciador (p.1079)
#define ADC0_SSFIFO3_R (*((volatile uint32_t *)0x400380A8)) //Registro que contiene los resultados de conversión de las muestras recogidas con el secuenciador (p. 1118)
#define SYSCTL_PLLFREQ0_R (*((volatile uint32_t *)0x400FE160)) //Registro para configurar el PLL
#define SYSCTL_PLLSTAT_R (*((volatile uint32_t *)0x400FE168)) //Registro muestra el estado de encendido del PLL
#define SYSCTL_PLLFREQ0_PLLPWR 0x00800000 // Valor para encender el PLL
void main(void){
    SYSCTL_RCGCGPIO_R = 0x10; // 1) Habilita reloj para Puerto E
    while( (SYSCTL_PRGPIO_R & 0x10) ==0); // Espera a reloj este listo

    // GPIO_PORTE_AHB_DIR_R = 0x00; // 2) PE4 entrada (analógica)
    // GPIO_PORTE_AHB_AFSEL_R |= 0x10; // 3) Habilita Función Alterna de PE4
    // GPIO_PORTE_AHB_DEN_R = 0x00; // 4) Deshabilita Función Digital de PE4
    // GPIO_PORTE_AHB_AMSEL_R |= 0x10; // 5) Habilita Función Analógica de PE4

    SYSCTL_RCGCADC_R = 0x01; // 6) Habilita reloj para ADC0
    while((SYSCTL_PRADC_R & 0x01)==0); // Espera a reloj este listo

    ADC0_PC_R = 0x01; // 7) Configura para 125Ksamp/s ( 1 octavo de la frecuencia de conversión configurada)(p.1159)
    ADC0_SSPRI_R = 0x0123; // 8) SS3 con la más alta prioridad (p.1099)
    ADC0_ACTSS_R = 0x0000; // 9) Deshabilita SS3 antes de cambiar configuración de registros (p. 1076)
    ADC0_EMUX_R = 0x0000; // 10) iniciar muestreo por software (p.1091)
    ADC0_SSEMUX3_R = 0x00; // 11)Entradas AIN(15:0) (p.1146)
    ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 9; // canal AIN9 (PE4) (p.1141)
    ADC0_SSCTL3_R = 0x000E; // 12) Si: sensor de temperatura, Habilitación de INR3, Fin de secuencia; No:muestra diferencial (p.1142)
    ADC0_IM_R = 0x0000; // 13) Deshabilita interrupciones de SS3(p. 1081)
    ADC0_ACTSS_R |= 0x0008; // 14) Habilita SS3 (p.1077)

    SYSCTL_PLLFREQ0_R |= SYSCTL_PLLFREQ0_PLLPWR; // encender PLL
    while((SYSCTL_PLLSTAT_R & 0x01)==0); // espera a que el PLL fije su frecuencia
    SYSCTL_PLLFREQ0_R &= ~SYSCTL_PLLFREQ0_PLLPWR; // apagar PLL

            ADC0_ISC_R = 0x0008; // Se recomienda Limpia la bandera RIS del ADC0
    for(;;){
        uint32_t adc_result;
        float TEMP;
        ADC0_PSSI_R = 0x0008; // Inicia conversión del SS3
        while ((ADC0_RIS_R & 0x08)==0); // Espera a que SS3 termine conversión (polling)
        adc_result = (ADC0_SSFIFO3_R & 0xFFF);// Resultado en FIFO3 se asigna a variable &quot;result&quot;
        ADC0_ISC_R = 0x0008; // Limpia la bandera RIS del ADC0
        TEMP = 147.5 - ((75 * (3.3) * adc_result)/ 4096); //Ecuacion para convertir el voltaje del sensor a °C (p.1068)
            }
}
