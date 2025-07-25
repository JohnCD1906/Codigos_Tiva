// LIBRERIAS QUE UTILIZA EL PROGRAMA

#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"



// RUTINA DE SERVICIO DE INTERRUPCIï¿½N
void Timer03AIntHandler(void)
{
    //LIMPIA BANDERA
    TIMER3_ICR_R= 0X00000001 ; //LIMPIA BANDERA DE TIMER3
    GPIO_PORTN_DATA_R ^= 0x01;
}



//*********************************************
// CONFIGURACIï¿½N PARA TIMER DE 16 BITS con PREESCALADOR
 main(void) {

     //habilita PORTN
      SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R12;

         SYSCTL_RCGCTIMER_R |= 0X08; //HABILITA TIMER 3

         //retardo para que el reloj alcance el PORTN Y TIMER 3
         while ((SYSCTL_PRGPIO_R & 0X1000) == 0){};  // reloj listo?
         TIMER3_CTL_R=0X00000000; //DESHABILITA TIMER EN LA CONFIGURACION (p.986)
         TIMER3_CFG_R= 0X00000004; //CONFIGURAR PARA 16 BITS (p. 976)
         TIMER3_TAMR_R= 0X00000002; //CONFIGURAR PARA MODO PERIODICO CUENTA HACIA ABAJO (p. 977)
         //TIMER3_TAMR_R= 0X00000012; //CONFIGURAR PARA MODO PERIODICO CUENTA HACIA ARRIBA (p. 977)
         /*TIMER3_TAILR_R= 0X00FFFF; // VALOR DE RECARGA (p.1004)
         TIMER3_TAPR_R= 0X0F; // VALOR DE PREESCALADOR TIMER A,  16 MHZ /16 =1MHz (p.1008)*/
         TIMER3_TAILR_R = 62499;          // Valor base
         TIMER3_TAPR_R = 31;              // Preescaler
         TIMER3_ICR_R= 0X00000001 ; //LIMPIA POSIBLE BANDERA PENDIENTE DE TIMER3 (p.1002)
         TIMER3_IMR_R |= 0X00000001; //ACTIVA INTRRUPCION DE TIMEOUT (p.993)
         NVIC_EN1_R= 1<<(35-32); //HABILITA LA INTERRUPCION 35 DE  TIMER3 A
         TIMER3_CTL_R |= 0X00000001; //HABILITA TIMERA EN LA CONFIGURACION (p.986)


         //
         // habilita el bit 0 como digital
         // configura como salida
         //
         GPIO_PORTN_DIR_R = 0x01;
         GPIO_PORTN_DEN_R = 0x01;
         GPIO_PORTN_DATA_R = 0x00;

        // IntMasterEnable();

         while(1);

}
