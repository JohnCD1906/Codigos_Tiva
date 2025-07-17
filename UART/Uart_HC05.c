//PROGRAMA QUE INICILIZA LA INTERRUPCIÓN PARA EL UART7
//LA INTERRUPCIÓN SE DISPARA CUANDO LA FIFO RX RECIBE 1/8 DE SU CAPACIDAD

// Librerias
#include <stdint.h>
#include <stdbool.h>

// Definición de apuntadores para direccionar el puerto N
// se utiliza solo para la función del programa principal CONMUTA_Init
#define GPIO_PORTN_DATA_R       (*((volatile unsigned long *)0x4006400C)) // Registro de Datos Puerto N
#define GPIO_PORTN_DIR_R        (*((volatile unsigned long *)0x40064400)) // Registro de Dirección Puerto N
#define GPIO_PORTN_DEN_R        (*((volatile unsigned long *)0x4006451C)) // Registro de Habilitación Puerto N
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE608)) // Registro de Habilitación de Reloj de Puertos
#define SYSCTL_PRGPIO_R         (*((volatile unsigned long *)0x400FEA08)) // Registro de estatus de Reloj de Puerto

// Definición de loa apuntadores para direccionar el UART/ y GPIO_C
//
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

#define UART7_IFLS_R (*((volatile uint32_t *)0x40013034))   // Selección de interrupión para FIFO (p.1191)
#define UART_IFLS_RX1_8  0x00000000  // RX FIFO >= 1/8 full (p. 1192)
#define UART_IFLS_TX1_8  0x00000000  // TX FIFO <= 1/8 full (p. 1192)
#define UART7_IM_R      (*((volatile uint32_t *)0x40013038))  //Registro de mascara de interrupción UART1 (p.1194)


#define NVIC_EN1_R   (*((volatile uint32_t *)0xE000E104)) // Registro de habilitación de interrupción UART7 (p.154)
#define NVIC_PRI15_R (*((volatile uint32_t *)0xE000E43C)) //Registro de prioridad de interrupción (p. 159)

#define UART7_ICR_R (*((volatile uint32_t *)0x40013044))  // Bandera de interrupción


#define SYSCTL_RCGCGPIO_R       (*((volatile uint32_t *)0x400FE608)) // Control de reloj GPIO  (p.382)
#define SYSCTL_RCGCUART_R       (*((volatile uint32_t *)0x400FE618)) // Control de reloj UART (p.388)
#define SYSCTL_PRGPIO_R0        0x00000004  // Puerto GPIO C listo
#define SYSCTL_PRUART_R         (*((volatile uint32_t *)0x400FEA18))//Estado del UART (p.505)
#define SYSCTL_PRUART_R0        0x00000080  // UART Module 7 del UART listo


// Definición de constantes para operaciones
#define SYSCTL_RCGC2_GPION      0x00001000  // bit de estado del reloj de puerto N

// Definición de variables para el manejo de datos
 volatile int32_t Num;      // Variable para llevar la cuenta de retardo en función CONMUTA_Int
 volatile uint32_t data[6]; // Arreglo donde se guardan los bytes recibidos por UART
 volatile int32_t Interrupcion;   // Variable que cuenta las interrupciones que han ocurrido


 // *****  FUNCION CONMUTA ****
 // Es función del programa principal, mantiene dos leds controlados por el puerto N en conmutación

 void CONMUTA_Init(void){

  Num = 0;

  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPION; //

  while ((SYSCTL_PRGPIO_R & 0X1000) == 0){};  // reloj listo?

  GPIO_PORTN_DIR_R |= 0x0F;    // puerto N de salida
  GPIO_PORTN_DEN_R |= 0x0F;    // habilita el puerto N

  GPIO_PORTN_DATA_R = 0x02;    // enciende 1 led

  while(1){

      if(Num < 4000){
          Num = Num + 1;
      }else{
          GPIO_PORTN_DATA_R ^= 0x03;  // conmuta encendido de leds
          Num=0;
      }
  }
}


// ************* PROGRAMA PRINCIPAL  ***************
//Tiva TM4C1294
// Programa principal que configura una interrupción en el UART
//
// U7Rx (receptor) conectado a PC4
// U7Tx (transmisor) conectado a PC5


//------------Inicialización del UART------------
// Initializa UART7 para 115,200 bauds (reloj de 16 MHz del PIOSC),
// palabra de 8 bit, no hay bits de paridad, un stop bit, los FIFOs habilitados,
// no hay interrupciones

void main(void){
  SYSCTL_RCGCUART_R |=  0x00000080; // activa el reloj para el UART7 (p.388)
  SYSCTL_RCGCGPIO_R |= 0x00000004;  // activa el reloj para el Puerto C (p.382)

  while((SYSCTL_PRUART_R&SYSCTL_PRUART_R0) == 0){}; // Se espera a que el reloj se estabilice (p.505)
  UART7_CTL_R &= ~0x00000001; // se deshabilita el UART (p.1188)
  UART7_IBRD_R = 8;  // IBRD = int(16,000,000 / (16 * 115,200)) = int(8.681) (p.1184)
  UART7_FBRD_R = 44; // FBRD = round(0.6806 * 64) = 44 (p. 1185)
// Palabra de 8 bits (sin bits de paridad, un bit stop, FIFOs) (p. 1186)
  UART7_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
// UART toma su reloj del la fuente alterna como se define por SYSCTL_ALTCLKCFG_R (p. 1213)
  UART7_CC_R = (UART7_CC_R&~UART_CC_CS_M)+UART_CC_CS_PIOSC;
// La fuente de reloj alterna es el PIOSC (default)(P. 280)
  SYSCTL_ALTCLKCFG_R = (SYSCTL_ALTCLKCFG_R&~SYSCTL_ALTCLKCFG_ALTCLK_M)+SYSCTL_ALTCLKCFG_ALTCLK_PIOSC; //

  //Bloque que configura la interrupcion y la desenmascara
    UART7_IFLS_R &= ~0x0000003F;   // Limpia las interrupciones de FIFO de TX y RX (p.1191)
    UART7_IFLS_R += (UART_IFLS_TX1_8|UART_IFLS_RX1_8);    // Habilita las interrupciones de la FIFO de TX y RX, y la interrupción por time-out de RX
    UART7_ICR_R = 0x10;            // Limpia la bandera interrupcion de FIFO RX (En este programa solo manejaremos esta interrupción)
    UART7_IM_R |= 0x00000010;    // Desenmascara la interrupción para UART7
  //Fin de configuración de interrupcion

  // Alta velocidad deshabilitada;divide el reloj por 16 en lugar de 8 (default)(1188)
  UART7_CTL_R &= ~0x00000020;
  UART7_CTL_R |= 0x00000001;  // habilita el UART (p.1188)
 // Se espera a que el reloj se estabilice
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R0) == 0){};
  GPIO_PORTC_AFSEL_R |= 0x30; // habilita funcion alterna en PC5-4
  GPIO_PORTC_DEN_R |= 0x30;  // habilita digital I/O en PC5-4
  // configura PC5-4 como UART
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00110000;
  GPIO_PORTC_AMSEL_R &= ~0x30; // deshabilita la funcionabilidad analogica de PC

// Configuración de prioridad y hablitación de interrupción en NVIC
  NVIC_PRI15_R = (NVIC_PRI15_R&0xFFFFFF00)|0x00000000; // Prioridad 0 (p.152)
  NVIC_EN1_R = 1<<(60-32); // Habilita la interrupción 60 en NVIC (p. 154)

  Interrupcion = 0; // Inicializa variable de conteo de interrupciones

  CONMUTA_Init(); // Llamada a función del programa principal
}



//******* RUTINA DE SERVICIO DE INTERRUPCION **********
// Cuando la FIFO RX del UART7 reciba 1/8 de su capacidad esta
// rutina de interrupción se ejecutara

void UART7_Handler(void)
{
    Interrupcion = Interrupcion + 1;

     data[0] = UART7_DR_R&0xFF;;// se guarda el primer valor
     data[1] = UART7_DR_R&0xFF;// se guarda el segundo valor
     while((UART7_FR_R&UART_FR_RXFE) != 0); //Se espera a que el FIFO RX tenga un valor
     data[2] = UART7_DR_R&0xFF;// se guarda el tercer valor
     while((UART7_FR_R&UART_FR_RXFE) != 0); //Se espera a que el FIFO RX tenga un valor
     data[3] = UART7_DR_R&0xFF;// se guarda el cuarto valor
     while((UART7_FR_R&UART_FR_RXFE) != 0); //Se espera a que el FIFO RX tenga un valor
     data[4] = UART7_DR_R&0xFF;// se guarda el quinto valor
     while((UART7_FR_R&UART_FR_RXFE) != 0); //Se espera a que el FIFO RX tenga un valor
     data[5] = UART7_DR_R&0xFF;// se guarda el sexto valor

     UART7_ICR_R = 0x00000010;      // Limpia bandera de interrupción
}
