/* ========================================
 *
 * Interrupt routines header file for sending the data at at constant rate 
 *
 * Author: Ilaria Catarrasa
 * ========================================
*/
#ifndef __SEND_DATA_UART__
    #define __SEND_DATA_UART__
    #include "project.h"
    
    /*Define Byte and Buffer Size*/
    #define LENGTH_BYTE 6
    #define TRANSMIT_BUFFER_SIZE 1+LENGTH_BYTE+1
    
    /*send buffer array*/
    uint8_t OutArray[TRANSMIT_BUFFER_SIZE];   
          
    /*ISR prototype declaration*/
    CY_ISR_PROTO(isr_SendData);
    
#endif  

/* [] END OF FILE */
