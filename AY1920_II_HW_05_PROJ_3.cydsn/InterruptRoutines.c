/* ========================================
 *
 * Interrupt routines for sending the data at at constant rate 
 *
 * Author: Ilaria Catarrasa
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
int16_t Out_ms2;
int32 Out_mms2;
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
                        Out_ms2 = (int16)((xyz_positioning[i/2] | (xyz_positioning[i/2+1]<<8))) >> 4; //
                        
                        Out_ms2 = (float) Out_ms2*2*9.81*0.001*1000;     /*conversion from mg into m/s2: 
                                                                        * 2 is Typ. spec. from datasheet,                                                            
                                                                        * 9.8 is the conversion factor to m/s2 
                                                                        * 0.001 is to convert from msec to sec
                                                                        * 1000 conversion into mm/s2 for saving decimal values*/
                        
                        Out_mms2 = (int32) Out_ms2;             //
                        OutArray[i+1] = (uint8_t)(Out_mms2 & 0xFF);  //I send the data in 4 byte to save also the decimals
                        OutArray[i+2] = (uint8_t)(Out_mms2 >> 8);
                        OutArray[i+3] = (uint8_t)(Out_mms2 >> 16);
                        OutArray[i+4] = (uint8_t)(Out_mms2 >> 24);
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
