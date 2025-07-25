/**
 * MCP41010
 * Single/Dual Digital Potentiometer with SPI™ Interface
 * main.c
 *
 * Genera una forma de onda de diente de sierra.
 *
 * SSI0 (SPI)
 * PA2: SCLK
 * PA3: FSS
 * PA4: MOSI
 * PA5: MISO (no se usa)
 *
 * MCP41010
 * Vcc: 3.3 V
 * PB0: GND
 * PA0: 3.3 v
 */

#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"

void SSI0_init (void);
void SSI0_sendData (uint16_t dat);
void pot_setVal (uint8_t slider);

int main(void)
{
    SSI0_init();   // Función que habilita el SPI
    uint8_t x = 0x00; // Valor inicial.
    int delay = 200;
    while (1) {
        pot_setVal(x++);

        int i;
        for (i = 0; i < delay; i++);
    }
}

void SSI0_init (void) {
    SYSCTL_RCGCSSI_R = SYSCTL_RCGCSSI_R0; // Activa reloj al SSI0
    while ((SYSCTL_PRSSI_R & SYSCTL_PRSSI_R0) == 0); // Espera a que este listo
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0; // Activa reloj del GPIO A
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) == 0); // Espera a que este listo

    GPIO_PORTA_AHB_AFSEL_R |= 0x3C; // Seleciona la función alterna de PA[2:5].
    GPIO_PORTA_AHB_PCTL_R = (GPIO_PORTA_AHB_PCTL_R & 0XFF0000FF) | 0x00FFFF00; // Configura las terminales de PA a su función de SSI0.
    GPIO_PORTA_AHB_AMSEL_R = 0x00; // Deshabilita la función analogica
    GPIO_PORTA_AHB_DIR_R = (GPIO_PORTA_AHB_DIR_R & ~0x3C) | 0x1C; // Configura al puerto como salida
    GPIO_PORTA_AHB_DEN_R |= 0x3C; // Habilita la función digital del puerto

    SSI0_CR1_R = 0x00; // Selecciona modo maestro/deshabilita SSI0. (p. 1247)
    SSI0_CPSR_R = 0x02; // preescalador (CPSDVSR) del reloj SSI (p. 1252)
    // configura para Freescale SPI; 16bit; 4 Mbps; SPO = 0; SPH = 0 (p. 1245)
    SSI0_CR0_R = (0x0100 | SSI_CR0_FRF_MOTO | SSI_CR0_DSS_16) & ~(SSI_CR0_SPO | SSI_CR0_SPH);

    SSI0_CR1_R |= SSI_CR1_SSE; // Habilita SSI0.

}

void SSI0_sendData (uint16_t dat) {
    // Envia dato de 16-bit
    while ((SSI0_SR_R & SSI_SR_BSY) != 0); // espero si el bus está ocupado
    SSI0_DR_R = dat; // envia dato.
}

void pot_setVal(uint8_t slider) {
    //Combine el valor del control deslizante con el código de comando de escritura.
    // Estructura del mensaje SPI: [comando (8-bits)][deslizador (8-bits)]
    SSI0_sendData(0x1100 | slider);
}
