/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
/*Include headers*/
#include "InterruptRoutines.h"
#include "I2C_Interface.h"
#include "project.h"
#include "stdio.h"

/*Include required header files*/
#include "project.h"

#define LIS3DH_DEVICE_ADDRESS 0x18
#define LIS3DH_OUT_X_L 0x28

#define LIS3DH_STATUS_REG 0x27

uint8_t xyz_positioning[LENGTH_BYTE];             
int16_t Out;
uint8_t header = 0xA0;
uint8_t footer = 0xC0;
 /*legth wuthout header and footer*/
int i = 0;


CY_ISR(isr_SendData)
{    
    OutArray[0] = header;
    OutArray[TRANSMIT_BUFFER_SIZE-1] = footer;
    
    uint8_t status_register; 
    
    ErrorCode error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_STATUS_REG,
                                        &status_register);
     
    if (error == NO_ERROR)
    {
        if(status_register & 0x88)
        {        
             //If overrun occurred and new values are available read and send UART values
                error = I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
                                                LIS3DH_OUT_X_L,
                                                TRANSMIT_BUFFER_SIZE-2,
                                                &xyz_positioning[0]);
                if (error == NO_ERROR)
                {
                    for (i = 0; i<LENGTH_BYTE; i+=2)
                        {
                            Out = (int16)((xyz_positioning[i] | (xyz_positioning[i+1]<<8)))>>6;
                            Out *= 8;//conversion into mg
                            OutArray[i+1] = (uint8_t)(Out & 0xFF);
                            OutArray[i+2] = (uint8_t)(Out >> 8);                      
                        }
                    //UART_Debug_PutString("Read data complete.\r\n");    
                    UART_Debug_PutArray(OutArray, TRANSMIT_BUFFER_SIZE); 
                }
                              
                
        }
        else
        {
           //UART_Debug_PutString("Data Not available.\r\n "); 
        }
        
        
    
    }
    
    

}
/* [] END OF FILE */
