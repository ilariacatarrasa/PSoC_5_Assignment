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
int i = 0, j = 0;


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
        if(status_register & 0x08)
        {        
             //If overrun occurred and new values are available read and send UART values
                error = I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
                                                LIS3DH_OUT_X_L,
                                                6,
                                                &xyz_positioning[0]);
                if (error == NO_ERROR)
                {
                    for (i = 0; i<LENGTH_BYTE; i+=4)
                    {                                                 
                        Out = (int16)((xyz_positioning[i/2] | (xyz_positioning[i/2+1]<<8))) >> 4; //
                        
                        Out = (float) Out*2*9.81*0.001;     /*conversion from mg into m/s2: 
                                                            * 2 is Typ. spec. from datasheet,                                                            
                                                            * 9.8 is the conversion factor to m/s2 
                                                            * 0.001 is to convert from msec to sec*/
                        
                        Out = (int32) Out*1000;             //conversion into mm/s2 for saving decimal values
                        OutArray[i+1] = (uint8_t)(Out & 0xFF);  //I send the data in 4 byte to save also the decimals
                        OutArray[i+2] = (uint8_t)(Out >> 8);
                        OutArray[i+3] = (uint8_t)(Out >> 16);
                        OutArray[i+4] = (uint8_t)(Out >> 24);
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
