/*
 * mainApp.c
 *
 *  Created on: Sep 8, 2017
 *      Author: dkhairnar
 */

#include "mainApp.h"
#include "sx1276_7_8.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "main.h"
#define printUSB(x) CDC_Transmit_FS((uint8_t*)x,strlen((char*)x))

char strBuf[64];
uint16_t SysTime;
u16 time2_count;
u16 key1_time_count;
u16 key2_time_count;
u8 rf_rx_packet_length;

u8 mode;//lora--1/FSK--0
u8 Freq_Sel;//
u8 Power_Sel;//
u8 Lora_Rate_Sel;//
u8 BandWide_Sel;//

u8 Fsk_Rate_Sel;//

u8 key1_count;
/*key1_count = 0----------->lora master
key1_count = 1----------->lora slaver
key1_count = 2----------->FSK TX test
key1_count = 3----------->FSK RX test
*/
u8 time_flag;
/*{
bit0 time_1s;
bit1 time_2s;
bit2 time_50ms;
bit3 ;
bit4 ;
bit5 ;
bit6 ;
bit7 ;
}*/
u8	operation_flag;
/*typedef struct
{
	uchar	:RxPacketReceived-0;
	uchar	:
	uchar	:
	uchar	:
	uchar	:
	uchar	:key2_down;
	uchar	:key1_down;
	uchar	;
} operation_flag;*/
u8 key_flag;
/*{
	uchar	:key1_shot_down;
	uchar	:key1_long_down;
	uchar	:key2_short_down;
	uchar	:key2_long_down
	uchar	:
	uchar	:;
	uchar	:;
	uchar	;
}*/
uint8_t TOUCH1_Status=0;
uint8_t TOUCH2_Status=0;
///////////////////HTTU test////////////////
//////////////////data:21072018////////////
/////////Des: test transmit "hello stm32 lora"
///////
void mainTest()
{
		uint8_t Ver;
		Ver=SPIRead(REG_LR_VERSION);
		while(1)
		{
			Ver=SPIRead(REG_LR_VERSION);
			HAL_GPIO_TogglePin(LED_RED_GPIO_Port,LED_RED_Pin);
			HAL_Delay(500);

			sprintf((char*)sx1276_7_8Data,"Version=%d",Ver);
					printUSB(sx1276_7_8Data);

		}
}
void mainApp()
{
	SysTime = 0;
	operation_flag = 0x00;
	key1_count = 0x00;
	mode = 0x01;//lora mode
	Freq_Sel = 0x00;//433M
	Power_Sel = 0x02;//
	Lora_Rate_Sel = 0x05;	// 6:SF=12
							// 5:SF=11
	BandWide_Sel = 0x07; //BW 125Khz
	Fsk_Rate_Sel = 0x00;

	RED_LED_L();
	HAL_Delay(500);
	RED_LED_H();

	HAL_GPIO_WritePin(Reset_GPIO_Port,Reset_Pin,GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(Reset_GPIO_Port,Reset_Pin,GPIO_PIN_SET);

	sx1276_7_8_Config();//
	sx1276_7_8_LoRaEntryRx();


	while (1)
	{
		key1_count = 0;
		//HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
		switch(key1_count)
		{
			case 0://lora master Tx

				{

					if(HAL_GPIO_ReadPin(TOUCH1_GPIO_Port,TOUCH1_Pin)==GPIO_PIN_SET)
					{
						HAL_Delay(300);
						if(HAL_GPIO_ReadPin(TOUCH1_GPIO_Port,TOUCH1_Pin)==GPIO_PIN_SET)
						{
							if(TOUCH1_Status==0)
							{
								sprintf((char *)sx1276_7_8Data,"D_STM/1/ON");
								TOUCH1_Status=1;
							}
							else
							{
								sprintf((char*)sx1276_7_8Data,"D_STM/1/OFF");
								TOUCH1_Status=0;
							}

							printUSB(sx1276_7_8Data);
							RED_LED_H();
							sx1276_7_8_LoRaEntryTx();
							sx1276_7_8_LoRaTxPacket();
							RED_LED_L();
						}
					}

					if(HAL_GPIO_ReadPin(TOUCH2_GPIO_Port,TOUCH2_Pin)==GPIO_PIN_SET)
										{
											HAL_Delay(20);
											if(HAL_GPIO_ReadPin(TOUCH2_GPIO_Port,TOUCH2_Pin)==GPIO_PIN_SET)
											{
												sprintf((char*)sx1276_7_8Data,"TOUCH2 pressed!!!");
												printUSB(sx1276_7_8Data);
											}
										}


				}

				break;
			case 1://lora slaver Rx

				if(sx1276_7_8_LoRaRxPacket())
				{

					int16_t RSSI_value=0;

					sprintf(strBuf,"Received Data: %s\n\r",(char*)RxData);
					printUSB(strBuf);


					HAL_Delay(10);
					RSSI_value=sx1276_7_8_LoRaReadPacketRSSI();
					sprintf(strBuf,"RSSI Packet=%d\n\r",RSSI_value);
					printUSB(strBuf);
					HAL_Delay(500);

				}

			break;

			case 2:


			break;
			case 3:


			break;
			default:

			break;
		}
	}
}
