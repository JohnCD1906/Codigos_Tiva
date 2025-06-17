/*
MICROPROCESADORES Y MICROCONTROLADORES
PROYECTO FINAL "PROTECCION CONTRA SOBREVOLTAJE CON TIVA TM4C1294NCPDT"
BY: CESAREO DOLORES JOHN BRIAN
 */

//*******************LIBRERIAS*****************
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "driverlib/interrupt.h"

//***************DIRECCIONES DE MEMORIA**********************************RELOJ
#define SYSCTL_RCGCGPIO_R   (*((volatile uint32_t *)0x400FE608))
#define SYSCTL_RCGCI2C_R    (*((volatile uint32_t *)0x400FE620))
#define SYSCTL_PRGPIO_R     (*((volatile uint32_t *)0x400FEA08))
//***********************************************************************GPIO B
#define GPIO_PORTB_DATA_R   (*((volatile uint32_t *)0x400593FC))
#define GPIO_PORTB_DIR_R    (*((volatile uint32_t *)0x40059400))
#define GPIO_PORTB_AFSEL_R  (*((volatile uint32_t *)0x40059420))
#define GPIO_PORTB_ODR_R    (*((volatile uint32_t *)0x4005950C))
#define GPIO_PORTB_DEN_R    (*((volatile uint32_t *)0x4005951C))
#define GPIO_PORTB_PCTL_R   (*((volatile uint32_t *)0x4005952C))
//*********************************************************************UART
#define GPIO_PORTC_AFSEL_R      (*((volatile uint32_t *)0x4005A420))
#define GPIO_PORTC_DEN_R        (*((volatile uint32_t *)0x4005A51C))
#define GPIO_PORTC_AMSEL_R      (*((volatile uint32_t *)0x4005A528))
#define GPIO_PORTC_PCTL_R       (*((volatile uint32_t *)0x4005A52C))
#define UART7_DR_R              (*((volatile uint32_t *)0x40013000))
#define UART7_FR_R              (*((volatile uint32_t *)0x40013018))
#define UART_FR_TXFF            0x00000020  // FIFO TX UART llena
#define UART_FR_RXFE            0x00000010  // FIFO RX UART vacia
#define UART7_IBRD_R            (*((volatile uint32_t *)0x40013024)) //Divisor de entero de Baud Rate UART (p.1184)
#define UART7_FBRD_R            (*((volatile uint32_t *)0x40013028)) //Divisor de fracción de Baud Rate UART (p.1185)
#define UART7_LCRH_R            (*((volatile uint32_t *)0x4001302C)) // Control de linea UART (p.1186)
#define UART_LCRH_WLEN_8        0x00000060  // palabra de 8 bits
#define UART_LCRH_FEN           0x00000010  // Habilitación de la FIFO de la UART
#define UART7_CTL_R             (*((volatile uint32_t *)0x40013030)) //Control UART (p.1188)
#define UART7_CC_R              (*((volatile uint32_t *)0x40013FC8)) // Configuración de control (p.1213)
#define UART_CC_CS_M            0x0000000F  // UART fuente del reloj de Baud Rate
#define UART_CC_CS_SYSCLK       0x00000000  // Sistema de reloj (basado en fuente de reloj y factor de división)                    // source and divisor factor)
#define UART_CC_CS_PIOSC        0x00000005  // PIOSC
#define SYSCTL_ALTCLKCFG_R      (*((volatile uint32_t *)0x400FE138))//Configuración de reloj alterno (p.280)
#define SYSCTL_ALTCLKCFG_ALTCLK_M                                             \
                                0x0000000F  // Fuente alternativa de reloj
#define SYSCTL_ALTCLKCFG_ALTCLK_PIOSC                                         \
                                0x00000000  // PIOSC
#define SYSCTL_RCGCUART_R       (*((volatile uint32_t *)0x400FE618)) // Control de reloj UART (p.388)
#define SYSCTL_PRGPIO_R0        0x00000004  // Puerto GPIO C listo
#define SYSCTL_PRUART_R         (*((volatile uint32_t *)0x400FEA18))//Estado del UART (p.505)
#define SYSCTL_PRUART_R0       0x00000080  // UART Module 7 del UART listo
//*********************************************************************GPIO E
#define GPIO_PORTE_AHB_DIR_R (*((volatile uint32_t *)0x4005C400))
#define GPIO_PORTE_AHB_AFSEL_R (*((volatile uint32_t *)0x4005C420))
#define GPIO_PORTE_AHB_DEN_R (*((volatile uint32_t *)0x4005C51C))
#define GPIO_PORTE_AHB_AMSEL_R (*((volatile uint32_t *)0x4005C528))
//*********************************************************************ADC
#define SYSCTL_RCGCADC_R (*((volatile uint32_t *)0x400FE638)) // Registro para habilitar el reloj al ADC(p. 396)
#define SYSCTL_PRADC_R (*((volatile uint32_t *)0x400FEA38)) // Registro para verificar si el ADC esta listo (p.515)
#define ADC0_PC_R (*((volatile uint32_t *)0x40038FC4)) // Registro para configurar tasa de muestreo (p.1159)
#define ADC0_SSPRI_R (*((volatile uint32_t *)0x40038020)) // Regidtrp para configurar la prioridad del secuenciador (p.1099)
#define ADC0_ACTSS_R (*((volatile uint32_t *)0x40038000)) // Registro para controlar la activación del secuenciador (p. 1076)
#define ADC0_EMUX_R (*((volatile uint32_t *)0x40038014)) // Registro para seleccionar el evento (trigger) que inicia el muestreo en cada secuenciador (p.1091)
#define ADC0_SSEMUX3_R (*((volatile uint32_t *)0x400380B8)) // Registro que selecciona entre las entradas AIN[19:16] o AIN[15:0] (p.1046)
#define ADC0_SSMUX3_R (*((volatile uint32_t *)0x400380A0)) // Registro para configurar la entrada analógica para el Secuenciador 3 (p.1041)
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
#define ADC0_SAC_R        (*((volatile uint32_t *)0x40038030)) // Sample Averaging Control
#define ADC0_CTL_R        (*((volatile uint32_t *)0x40038038)) // ADC Control
#define ADC0_SSOP3_R      (*((volatile uint32_t *)0x40038070)) // Sample Sequencer 3 Operation
#define ADC0_SSTSH3_R     (*((volatile uint32_t *)0x400380BC)) // Sample Sequencer 3 Sample and Hold Time
#define NVIC_EN0_R        (*((volatile uint32_t *)0xE000E100)) // Interrupt Enable Register 0
//*********************************************************************GPIO N
#define GPIO_PORTN_DATA_R (*((volatile unsigned long *)0x4006400C)) // Registro de Datos Puerto N
#define GPIO_PORTN_DIR_R (*((volatile unsigned long *)0x40064400)) // Registro de Dirección Puerto N
#define GPIO_PORTN_DEN_R (*((volatile unsigned long *)0x4006451C)) // Registro de Habilitación Puerto N
//*********************************************************************
#define I2C0_MSA_R          (*((volatile uint32_t *)0x40020000))
#define I2C0_MCS_R          (*((volatile uint32_t *)0x40020004))
#define I2C0_MDR_R          (*((volatile uint32_t *)0x40020008))
#define I2C0_MTPR_R         (*((volatile uint32_t *)0x4002000C))
#define I2C0_MCR_R          (*((volatile uint32_t *)0x40020020))

//***************************MACROS************************************
#define I2C_MCS_RUN       0x00000001  // Iniciar la transmisión
#define I2C_MCS_START     0x00000002  // Condición de START
#define I2C_MCS_STOP      0x00000004  // Condición de STOP
#define I2C_MCS_ACK       0x00000008  // Enviar ACK
#define I2C_MCS_HS        0x00000010  // Alta velocidad (opcional)
#define I2C_MCS_BUSY      0x00000001  // Lectura: I2C está ocupado
#define I2C_MCS_ERROR     0x00000002  // Error detectado
#define I2C_MCS_ARBLST    0x00000010  // Pérdida de arbitraje

#define N_MUESTRAS 100
#define GAIN 58.0f
#define OFFSET 1.55f
#define UMBRAL_BAJO 30.0f
#define UMBRAL_ALTO 80.0f
#define PCF8574T_ADDR   0x27  // Dirección por defecto del módulo I2C
#define LCD_BACKLIGHT   0x08  // Luz de fondo (bit P3)
#define LCD_ENABLE      0x04  // Pulso de Enable (bit P2)
#define LCD_RW          0x00  // RW en 0 para escritura
#define LCD_RS_CMD      0x00  // RS en 0 para comando
#define LCD_RS_DATA     0x01  // RS en 1 para datos
//**Direcciones del DS1307
int AdreDS1307 = 0x68;  // Dirección I2C correcta del DS1307 (7 bits)
int AdreSec= 0x00;
int AdreMin=0x01;


float vrms=0;
float data = 0;
int u=0;
uint8_t error;     // Bandera de error de comunicación
uint32_t i;        // Variable para retardos
uint16_t lectura=0;
uint8_t segundos, minutos, horas, dia, fecha, mes, anio;
uint8_t sec, min, hour, day, fech, month, year;
char alarma[40];

//***********CONFIGURACION DEL ADC_SEC3****************************+
void ADC0_Ini(void) {
    SYSCTL_RCGCADC_R |= 0x01;
    while ((SYSCTL_PRADC_R & 0x01) == 0);
    SYSCTL_RCGCGPIO_R |= 0x0010;
    while ((SYSCTL_PRGPIO_R & 0x0010) == 0);

    GPIO_PORTE_AHB_DIR_R &= ~0x20; // PE5 entrada
    GPIO_PORTE_AHB_AFSEL_R |= 0x20;
    GPIO_PORTE_AHB_DEN_R &= ~0x20;
    GPIO_PORTE_AHB_AMSEL_R |= 0x20;

    ADC0_PC_R = 0x01;
    ADC0_SSPRI_R = 0x0123;
    ADC0_ACTSS_R &= ~0x0008;
    ADC0_EMUX_R = 0x0000;
    ADC0_SAC_R = 0x0;
    ADC0_CTL_R = 0x0;
    ADC0_SSOP3_R = 0x0000;
    ADC0_SSTSH3_R = 0x000;

    ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 8; // AIN8 (PE5)
    ADC0_SSCTL3_R = 0x0006;
    ADC0_IM_R |= 0x08;
    ADC0_ACTSS_R |= 0x0008;
    NVIC_EN0_R |= (1 << 17);

    SYSCTL_PLLFREQ0_R |= SYSCTL_PLLFREQ0_PLLPWR;
    while ((SYSCTL_PLLSTAT_R & 0x01) == 0);
    SYSCTL_PLLFREQ0_R &= ~SYSCTL_PLLFREQ0_PLLPWR;

    ADC0_ISC_R = 0x0008;
    ADC0_PSSI_R |= 0x0008;
}

//***********CONFIGURACION PUERTO N*******************************************
void GPIO_Ini(void) {
    SYSCTL_RCGCGPIO_R |= 0x1000;  // Puerto N
    while ((SYSCTL_PRGPIO_R & 0x1000) == 0);

    GPIO_PORTN_DIR_R |= 0x01;     // PN0 salida
    GPIO_PORTN_DEN_R |= 0x01;
}

//================== FUNCION PARA INICIALIZAR I2C0 Y PUERTO B PARA SCL/SDA ===================
void I2C_Init(void){
    SYSCTL_RCGCI2C_R |= 0x01;     // Activa el reloj del I2C0
    SYSCTL_RCGCGPIO_R |= 0x02;    // Activa el reloj para el puerto B
    while((SYSCTL_PRGPIO_R & 0x02) == 0); // Espera a que el GPIOB esté listo

    GPIO_PORTB_AFSEL_R |= 0x0C;       // Función alterna para PB2 (SCL) y PB3 (SDA)
    GPIO_PORTB_ODR_R |= 0x08;         // Open-drain en PB3 (SDA)
    GPIO_PORTB_DIR_R |= 0x0C;         // Configura PB2 y PB3 como salidas
    GPIO_PORTB_DEN_R |= 0x0C;         // Habilita función digital
    GPIO_PORTB_PCTL_R |= 0x00002200;  // Función alterna I2C para PB2 y PB3

    I2C0_MCR_R = 0x10;        // Habilita el I2C0 como maestro
    I2C0_MTPR_R = 7;          // Velocidad de 100 kbps (16 MHz / (20 * (TPR + 1)))
}

//================== FUNCIÓN esperar: espera a que el I2C esté listo y verifica errores ===============
int esperar(){
    while(I2C0_MCS_R & 0x01); // Espera a que RUN = 0
    if((I2C0_MCS_R & I2C_MCS_ERROR) == I2C_MCS_ERROR){
        error = 1;
        return 1;
    }
    return 0;
}
//********************PANTALLA LCD*********************************************
void PCF8574T_Write(uint8_t data) {
    I2C0_MSA_R = (PCF8574T_ADDR << 1); // escritura
    I2C0_MDR_R = data;
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
    esperar();
}

void LCD_pulseEnable(uint8_t data) {
    PCF8574T_Write(data | LCD_ENABLE | LCD_BACKLIGHT);
    for (i = 0; i < 1000; i++) {}
    PCF8574T_Write((data & ~LCD_ENABLE) | LCD_BACKLIGHT);
    for (i = 0; i < 1000; i++) {}
}

void LCD_write4bits(uint8_t nibble) {
    PCF8574T_Write(nibble | LCD_BACKLIGHT);
    LCD_pulseEnable(nibble);
}

void LCD_sendCommand(uint8_t cmd) {
    LCD_write4bits((cmd & 0xF0) | LCD_RS_CMD);
    LCD_write4bits(((cmd << 4) & 0xF0) | LCD_RS_CMD);
}

void LCD_sendData(uint8_t data) {
    LCD_write4bits((data & 0xF0) | LCD_RS_DATA);
    LCD_write4bits(((data << 4) & 0xF0) | LCD_RS_DATA);
}

void LCD_print(const char* str) {
    while (*str) {
        LCD_sendData(*str++);
    }
}

void LCD_setCursor(uint8_t fila, uint8_t columna){
    uint8_t direccion;

    if(fila == 0){
        direccion = 0x00 + columna;  // Primera línea
    } else {
        direccion = 0x40 + columna;  // Segunda línea
    }

    LCD_sendCommand(0x80 | direccion); // Comando para mover cursor
    for(i = 0; i < 1000; i++){};       // Pequeño delay
}

void LCD_init(void) {
    for (i = 0; i < 25000; i++) {}
    LCD_write4bits(0x30); for (i = 0; i < 2500; i++) {}
    LCD_write4bits(0x30); for (i = 0; i < 2500; i++) {}
    LCD_write4bits(0x30); for (i = 0; i < 2500; i++) {}
    LCD_write4bits(0x20); for (i = 0; i < 2500; i++) {}

    LCD_sendCommand(0x28); // 4 bits, 2 líneas
    LCD_sendCommand(0x0C); // display ON
    LCD_sendCommand(0x06); // cursor derecha
    LCD_sendCommand(0x01); // clear
    for (i = 0; i < 5000; i++) {}
}

//*******************************************************************
void UART7_Transmit(char c){
    //while((UART7_FR_R & 0x20) != 0); // Espera a que haya espacio
    //UART7_DR_R = c;
    int timeout = 100000;
        while((UART7_FR_R & UART_FR_TXFF) != 0 && timeout--) {}
        if(timeout > 0){
            UART7_DR_R = c;
        }
}

void UART7_SendString(const char *str){
    volatile int i;
    while(*str){
        UART7_Transmit(*str++);
        for(i=0; i<1000; i++);
    }
}

void UART7_Init(void){
    // 1) Habilitar reloj para UART7
    SYSCTL_RCGCUART_R |= 0x00000080;         // bit 7 = UART7
    // Esperar a que UART7 esté listo
    while ((SYSCTL_PRUART_R & SYSCTL_PRUART_R0) == 0) { }

    // 2) Habilitar reloj para GPIOC
    SYSCTL_RCGCGPIO_R |= 0x00000004;         // bit 2 = Puerto C
    // Esperar a que GPIOC esté listo
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) == 0) { }

      UART7_CTL_R &= ~0x00000001; // se deshabilita el UART (p.1188)
      UART7_IBRD_R = 104;  // IBRD = int(16,000,000 / (16 * 115,200)) = int(8.681) (p.1184)
      UART7_FBRD_R = 11; // FBRD = round(0.6806 * 64) = 44 (p. 1185)
    // Palabra de 8 bits (sin bits de paridad, un bit stop, FIFOs) (p. 1186)
      UART7_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
    // UART toma su reloj del la fuente alterna como se define por SYSCTL_ALTCLKCFG_R (p. 1213)
      UART7_CC_R = (UART7_CC_R&~UART_CC_CS_M)+UART_CC_CS_PIOSC;
    // La fuente de reloj alterna es el PIOSC (default)(P. 280)
      SYSCTL_ALTCLKCFG_R = (SYSCTL_ALTCLKCFG_R&~SYSCTL_ALTCLKCFG_ALTCLK_M)+SYSCTL_ALTCLKCFG_ALTCLK_PIOSC; //
      // alta velocidad deshabilitada;divide el reloj por 16 en lugar de 8 (default)(1208)
      UART7_CTL_R &= ~0x00000020;
      UART7_CTL_R |= 0x00000001;  // habilita el UART (p.1208)
     // Se espera a que el reloj se estabilice
      while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R0) == 0){};
      GPIO_PORTC_AFSEL_R |= 0x30; // habilita funcion alterna en PC5-4
      GPIO_PORTC_DEN_R |= 0x30;  // habilita digital I/O en PC5-4
      // configura PC5-4 como UART
      GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00110000;
      GPIO_PORTC_AMSEL_R &= ~0x30; // deshabilita la funcionabilidad analogica de PC
      //No necesario si solo mandamos datos
      //UART7_IM_R |= 0x10;              // Habilita interrupción por Rx (RXIM)
      //NVIC_EN1_R |= (1 << 28);         // Habilita UART7 (IRQ 60) en NVIC
      u=u+1;
}
//*******************************************************************

// Interrupción del ADC
void ADC0SS3_Handler(void) {
    ADC0_PSSI_R |= 0x0008;
    data = (ADC0_SSFIFO3_R & 0xFFF);
    ADC0_ISC_R = 0x0008;
}

// Toma N muestras y calcula Vrms
float calcular_vrms(void) {
    float suma = 0.0f;
    int i=0;
    for (i = 0; i < N_MUESTRAS; i++) {
        ADC0_PSSI_R |= 0x08; // Inicia conversión
        while ((ADC0_RIS_R & 0x08) == 0);
        lectura = ADC0_SSFIFO3_R & 0xFFF;
        ADC0_ISC_R = 0x08;

        float volt = (lectura * 3.1f / 4095.0f - OFFSET) * GAIN;
        suma += volt * volt;
    }

    return sqrtf(suma / N_MUESTRAS);
}

// Convierte entero a texto ASCII
int int_a_ascii(int num, char *buf) {
    int i = 0;
    char temp[10];
    if (num == 0) {
        buf[0] = '0';
        return 1;
    }
    if (num < 0) {
        buf[i++] = '-';
        num = -num;
    }
    int j = 0;
    while (num > 0) {
        temp[j++] = (num % 10) + '0';
        num /= 10;
    }
    for (j = j - 1; j >= 0; j--) {
        buf[i++] = temp[j];
    }
    return i;
}

// Convierte float a cadena con 2 decimales
void float_a_cadena(float valor, char *buffer) {
    int parte_entera = (int)valor;
    int parte_decimal = (int)((valor - parte_entera) * 100);
    if (parte_decimal < 0) parte_decimal = -parte_decimal;

    int i = 0;
    buffer[i++] = 'V';
    buffer[i++] = '=';
    i += int_a_ascii(parte_entera, &buffer[i]);
    buffer[i++] = '.';
    if (parte_decimal < 10) buffer[i++] = '0';
    i += int_a_ascii(parte_decimal, &buffer[i]);
    buffer[i++] = '\n';
    buffer[i] = '\0';
}

//** Función para configurar al esclavo RTC DS1307 **
void  CargarFecha(){

    int segundos=0x00, minutos=0x59, horas=0x23, dia=0x07, fecha=0x09, mes=0x06, anio=0x25;
    while(I2C0_MCS_R&0x00000001){}; // wait for I2C ready
    //Para transmitir
    I2C0_MSA_R=(AdreDS1307<<1)&0xFE; //Cargo la dirección del DS1307 e indico "SEND", es decir, el Slave va a recibir
    I2C0_MDR_R=AdreSec&0x0FF; //Envio la Subdirección( dirección del registro interno "segundos") al DS1307
    I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_START); // Condición de START y corro
    for(i=0;i<150;i++){} //Delay
    esperar();


    I2C0_MDR_R=segundos; //Envio el valor de "segundos"
    I2C0_MCS_R=(I2C_MCS_RUN);
    for(i=0;i<150;i++){} //Delay
    esperar();


    I2C0_MDR_R=minutos; //Envio el valor de "minutos"
    I2C0_MCS_R=(I2C_MCS_RUN); //Inicio la transmisión 1
    for(i=0;i<150;i++){} //Delay
    esperar();


    I2C0_MDR_R=horas; //Envio el valor de "horas"
    I2C0_MCS_R=(I2C_MCS_RUN); //Inicio la transmisión 2
    for(i=0;i<150;i++){} //Delay
    esperar();


    I2C0_MDR_R=dia; //Envio el valor de "dia"
    I2C0_MCS_R=(I2C_MCS_RUN); //Inicio la transmisión 4
    for(i=0;i<150;i++){} //Delay
    esperar();


    I2C0_MDR_R=fecha; //Envio el valor de "fecha"
    I2C0_MCS_R=(I2C_MCS_RUN); //Inicio la transmisión 5
    for(i=0;i<150;i++){} //Delay
    esperar();


    I2C0_MDR_R=mes; //Envio el valor de "mes"
    I2C0_MCS_R=(I2C_MCS_RUN); //Inicio la transmisión 6
    for(i=0;i<150;i++){} //Delay
    esperar();


    I2C0_MDR_R= anio; //Envio el valor de "año"
    I2C0_MCS_R=(I2C_MCS_STOP|I2C_MCS_RUN); //Inicio la ultima transmisión y STOP
    for(i=0;i<150;i++){} //Delay
    esperar();

}

void leerFecha(){
        while(I2C0_MCS_R&0x00000001){}; // wait for I2C ready

        //Para actualizar registro para iniciar la lectura
        I2C0_MSA_R=(AdreDS1307<<1)&0xFE; //Cargo la dirección del DS1207 e indico "SEND", es decir, el Slave va a recibir
        I2C0_MDR_R=AdreSec&0x0FF; //Envio la Subdirección( dirección del registro interno "segundos") al DS1307
        I2C0_MCS_R=(I2C_MCS_START|I2C_MCS_RUN);// Condición de START, y corro
        for(i=0;i<150;i++){} //Delay
        esperar();


        //Para recibir información
        I2C0_MSA_R=(AdreDS1307<<1)&0xFE; //La dirección del DS1307 en el Master Slave Adress
        I2C0_MSA_R|=0x01; //Indico "RECIEVE", es decir, el Slave va a transmitir
        I2C0_MCS_R=(I2C_MCS_START|I2C_MCS_RUN|I2C_MCS_ACK); // Condición de RESTART, corro, ack
        for(i=0;i<150;i++){} //Delay
        esperar();
        segundos=(I2C0_MDR_R&0xFF); //El Master lee lo que envia el DS1307

        I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_ACK); // corro, ack
        for(i=0;i<150;i++){} //Delay
        esperar();
        minutos=(I2C0_MDR_R&0xFF); //El Master lee lo que envía el DS1307

         I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_ACK); //corro, ack
         for(i=0;i<150;i++){} //Delay
         esperar();
         horas=(I2C0_MDR_R&0xFF); //El Master lee lo que envía el DS1307

         I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_ACK); // corro, ack
         for(i=0;i<150;i++){} //Delay
         esperar();
         dia=(I2C0_MDR_R&0xFF); //El Master lee lo que envía el DS1307

         I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_ACK); // corro, ack
         for(i=0;i<150;i++){} //Delay
         esperar();
         fecha=(I2C0_MDR_R&0xFF); //El Master lee lo que envía el DS1307

         I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_ACK); // corro, ack
         for(i=0;i<150;i++){} //Delay
         esperar();
         mes=(I2C0_MDR_R&0xFF); //El Master lee lo que envía el DS1307

         I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_ACK); // corro, ack
         for(i=0;i<150;i++){} //Delay
         esperar();
         anio=(I2C0_MDR_R&0xFF); //El Master lee lo que envía el DS1307

         I2C0_MCS_R=(I2C_MCS_STOP|I2C_MCS_RUN); //corro, alto

}

void procesar_hora_fecha() {
    // Convertir los valores desde BCD
    uint8_t dec_segundos =((segundos >> 4) * 10) + (segundos & 0x0F);
    uint8_t dec_minutos  = ((minutos >> 4) * 10) + (minutos & 0x0F);
    uint8_t dec_horas    = ((horas >> 4) * 10) + (horas & 0x0F);
    //uint8_t dec_dia      =((dia >> 4) * 10) + (dia & 0x0F);    // Nombre real del campo
    uint8_t dec_fecha    = ((fecha >> 4) * 10) + (fecha & 0x0F);  // Día numérico
    uint8_t dec_mes      = ((mes >> 4) * 10) + (mes & 0x0F);
    uint8_t dec_anio     = ((anio >> 4) * 10) + (anio & 0x0F);

    // Inicializa el buffer a ceros
    memset(alarma, 0, sizeof(alarma));

    int j = 0;

    // Texto fijo "Alarma: "
    const char *inicio = "Alarma: ";
    while (*inicio) {
        alarma[j++] = *inicio++;
    }

    // Horas
    if (dec_horas < 10) alarma[j++] = '0';
    j += int_a_ascii(dec_horas, &alarma[j]);
    alarma[j++] = ':';

    // Minutos
    if (dec_minutos < 10) alarma[j++] = '0';
    j += int_a_ascii(dec_minutos, &alarma[j]);
    alarma[j++] = ':';

    // Segundos
    if (dec_segundos < 10) alarma[j++] = '0';
    j += int_a_ascii(dec_segundos, &alarma[j]);
    alarma[j++] = ' ';

    // Día
    if (dec_fecha < 10) alarma[j++] = '0';
    j += int_a_ascii(dec_fecha, &alarma[j]);
    alarma[j++] = '/';

    // Mes
    if (dec_mes < 10) alarma[j++] = '0';
    j += int_a_ascii(dec_mes, &alarma[j]);
    alarma[j++] = '/';

    // Año
    alarma[j++] = '2';
    alarma[j++] = '0';
    if (dec_anio < 10) alarma[j++] = '0';
    j += int_a_ascii(dec_anio, &alarma[j]);

    // Salto de línea y fin de cadena
    alarma[j++] = '\n';
    alarma[j] = '\0';  // Asegura que sea cadena válida
    // 🔄 Aquí se enviaría a la SD por SPI:
    // SPI_SD_Write(timestamp_buffer);
}

void verificar_proteccion(void) {
    vrms = calcular_vrms();
    char vrms_buffer[20];
    float_a_cadena(vrms, vrms_buffer);  // Convertimos el float a cadena con formato V=XX.XX

    // Mostrar Vrms en el LCD
    LCD_setCursor(0, 0);
    LCD_print("Voltaje RMS");
    LCD_setCursor(1, 0);
    LCD_print(vrms_buffer);

    // Verificamos umbrales y mandamos resultado al teléfono
    if (vrms > UMBRAL_ALTO) {
        GPIO_PORTN_DATA_R |= 0x01;  // Activa protección
        UART7_SendString("Voltaje alto\n");
        UART7_SendString(alarma);
    }
    else if (vrms < UMBRAL_BAJO) {
        GPIO_PORTN_DATA_R |= 0x01;  // Activa protección
        UART7_SendString("Voltaje bajo\n");
        UART7_SendString(alarma);
    }
    else {
        GPIO_PORTN_DATA_R &= ~0x01;  // Desactiva protección
        UART7_SendString("Voltaje normal\n");
    }
}

//*******************FUNCION PRINCIPAL************************************
int main(void) {
    ADC0_Ini();
    GPIO_Ini();
    I2C_Init();
    LCD_init();
    UART7_Init();
    LCD_print("Medidor Activo  ");

    //Inicializo Slave
    while(I2C0_MCS_R&0x00000001){}; // espera que el I2C esté listo

    // Leer la hora y fecha del DS1307
    CargarFecha();

    IntMasterEnable();
    volatile int d=0;
    while (1) {
        verificar_proteccion();
        for (d = 0; d < 100000; d++); // Delay simple
        leerFecha();
        procesar_hora_fecha();
    }
}


