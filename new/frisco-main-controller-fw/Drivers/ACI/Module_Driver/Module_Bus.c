#include <stdint.h>
#include "Module_Bus.h"
#include "main.h"
#include "stm32f7xx_hal.h"
//#include "usbh_def.h"
//#include "usbh_cdc.h"
//#include "usb_host.h"
#include "string.h"
#include "stdlib.h"
#include "util.h"
#include "FriscoDefs.h"
#include "I2C_Bus_Master.h"
#include "Panel_Bus.h"
#include "../Middlewares/ACI/Msg/Message_Queue.h"
#include "Debug_Config.h"
#include "Timestamp.h"
#include "PowerControl.h"
#include "../UART/Module_UART.h"
#include "Module_Bus_Packet.h"
#include "../Firmware_Update/Firmware_Update_Master.h"

//extern USBH_HandleTypeDef hUsbHostFS;
#include "Stats_Buffed.h"
#include "Debug_Log_Msg.h"

#define DEBUG_REG_I2C_MASTER

extern I2C_HandleTypeDef hi2c1;

//extern ApplicationTypeDef Appli_state;
extern MODULE_DRIVER Module_Driver;

MODULE_BUS Module_Bus;

I2C_Bus_Master Module_Bus_Master;

Message_Queue_t		I2C_Msg_Queue;

const char *Module_Bus_String = {"Module Bus"};

uint8_t Module_Bus_Data_Callback(uint8_t port, I2C_Packet_Header_Type *hdr, uint8_t *pkt);

void Module_Bus_Tasks(void)
{
	I2C_Bus_Master_Tasks(&Module_Bus_Master);
}
//I2C_Bus_Master Module_I2C_Bus;

void Init_Module_Bus(void)
{
	// Delay 10 ms
	HAL_Delay(10);

	// Release all port resets
	// Set_Port_Resets(0x00); // ZGy_Note

	// Set Default to port
	Switch_Module_UART_Port(Module_Log_Port);

	I2C_Bus_Master_Init(&Module_Bus_Master, MUTEX_ID_MODULE_BUS, I2C1, Module_Bus_Data_Callback, Module_Bus_String, I2C1_SLOT_SDA_GPIO_Port, I2C1_SLOT_SDA_Pin, I2C1_SLOT_SCL_GPIO_Port, I2C1_SLOT_SCL_Pin);
	Panel_Bus_1_Init();
	Panel_Bus_2_Init();
}


MASTER_BUS_PACKET * Create_Module_Bus_Packet(eMODULE_PORTS Port, unsigned char * Data, uint8_t Data_Size)
{
	MASTER_BUS_PACKET * new_packet = (MASTER_BUS_PACKET *)pmdMalloc(sizeof(MASTER_BUS_PACKET));

	new_packet->Port = Port;
	memcpy(new_packet->Data, Data, Data_Size);
	new_packet->Data_Size = Data_Size;

	// Calculate the CRC8
	new_packet->Data[Data_Size] = Get_CRC8(new_packet->Data, Data_Size);

#ifdef LOW_LEVEL_I2C_DEBUG
	formatlog("Main_Controller","I2C","Outgoing I2C Packet\r\n");
	for(uint16_t i=0; i<Data_Size + 1; ++i)
	{
		//aci_print("%02X", new_packet->Data[i]);

		if((i & 0x0F) == 0x0F)
		{
		//	aci_print("\r\n                       ");
		}
		else if(i < Data_Size)
		{
		//	aci_print(" ");
		}
	}
	//aci_print("]\r\n");
#endif
	return new_packet;
}

MASTER_BUS_PACKET * Create_Module_Bus_Packet_Ex(eMODULE_PORTS Port, Packet_Header_Type *hdr, unsigned char * Data)
{
	MASTER_BUS_PACKET * new_packet = (MASTER_BUS_PACKET *)pmdMalloc(sizeof(MASTER_BUS_PACKET));

	new_packet->Port = Port;
	new_packet->Data_Size = hdr->Formatted_Packet_Header.Data_Length;
	if(hdr)
	{
		memcpy(new_packet->Data, hdr, I2C_PACKET_HEADER_SIZE);
	}
	if(Data)
	{
		memcpy(new_packet->Data + I2C_PACKET_HEADER_SIZE, Data, hdr->Formatted_Packet_Header.Data_Length);
		// Calculate the CRC8
		new_packet->Data[hdr->Formatted_Packet_Header.Data_Length + I2C_PACKET_HEADER_SIZE] = Get_CRC8(new_packet->Data, hdr->Formatted_Packet_Header.Data_Length);
	}

	return new_packet;
}

void Apply_Module_Bus_CRC(MASTER_BUS_PACKET *packet)
{
	packet->Data[packet->Data_Size + I2C_PACKET_HEADER_SIZE] = Get_CRC8(packet->Data, packet->Data_Size + I2C_PACKET_HEADER_SIZE);
}

void Log_Module_Info(eMODULE_PORTS port)
{
	MODULE *mod = &Module_Driver.Ports[port].Module;
	char str[100];

	formatlog("Main_Controller","Mod_Info","Got Port Data for Port %d:\r\n", port+1);
	formatlog("Main_Controller","Mod_Info","Module Type ID:%-8ld\r\n", mod->Module_Type_ID);
	formatlog("Main_Controller","Mod_Info","Module SubType ID:%-8ld\r\n", mod->Module_SubType_ID);
	formatlog("Main_Controller","Mod_Info","Module I2C Address:0x%02X\r\n", mod->Module_I2C_Address);
	formatlog("Main_Controller","Mod_Info","Module Name:   %-23s\r\n", mod->Module_Name);
	formatlog("Main_Controller","Mod_Info","HW Revision:%-16s\r\n", mod->HW_Revision);
	sprintf(str, "%i.%i.%i.%i", mod->FW_Revision[0], mod->FW_Revision[1], mod->FW_Revision[2], mod->FW_Revision[3]);
	formatlog("Main_Controller","Mod_Info","FW Revision:%-23s\r\n", str);
	sprintf(str, "%i.%i.%i.%i", mod->ModLib_FW_Revision[0], mod->ModLib_FW_Revision[1], mod->ModLib_FW_Revision[2], mod->ModLib_FW_Revision[3]);
	formatlog("Main_Controller","Mod_Info","ModuleLib FW Revision:   %s\r\n", str);
	formatlog("Main_Controller","Mod_Info","Serial Number:%-9s\r\n", mod->Serial_Number);
}


uint8_t sta_get_module_state(void);
void Module_Bus_INT_Callback(eMODULE_PORTS port, void * Parameters)
{
	if(Module_Driver.Ports[port].Is_Module_Attached)
	{
		uint8_t ret=sta_get_module_state();
		if(ret == 0)
			return;				//Module bus busy, don't send,need to wait to avoid I2C lockup.Steve Rui.
//		return;
		// Note:  This is a deferred interrupt callback, so it occurs in the foreground, not an interrupt context.
		// Send the command to get the packet info
//		aci_print("\r\nModule CallBack %d\r\n",port);
		unsigned char data[5] = {dmc_Interrupt_Response_Msg, 0x01, 0x01, 0x00};
		MASTER_BUS_PACKET * packet = Create_Module_Bus_Packet(port, data, 4);
//		Send_Module_Data(packet, true);
		I2C_Bus_Master_Send(&Module_Bus_Master,
							Module_Driver.Ports[packet->Port].Module.Module_I2C_Address,
							packet->Port,
							(I2C_Packet_Header_Type*)packet->Data,
							&packet->Data[IBM_PACKET_HDR_SIZE],
							2);
		pmdFree(packet);
		// Kick the Wifi Processing loop
		Module_Bus_Master.KickBusProcess = 1;
	}
}

bool Is_Module_Bus_Ready(void)
{
	// Make sure both RX & TX queues are not full
	if(IMsgQ_Queue_Space_Remaining(&Module_Bus_Master.RxIMsgQ) && Get_MsgQ_Space_Remaining(Module_Bus_Master.TxMsgQ))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Send_Module_Data(MASTER_BUS_PACKET * packet, uint8_t response_expected)
{
	// This is not necessary unless we decide to use the UART to send/receive messages to/from modules.
//	if(packet->Port != (eMODULE_PORTS)Module_Bus.Active_Module_Port)
//		Switch_Module_Port(packet->Port);

	I2C_Bus_Master_Send(&Module_Bus_Master,
						Module_Driver.Ports[packet->Port].Module.Module_I2C_Address,
						packet->Port,
						(I2C_Packet_Header_Type*)packet->Data,
						&packet->Data[IBM_PACKET_HDR_SIZE],
						response_expected);

	// I2C_Bus_Master_Send makes a copy of the packet, so we can pmdFree it now
	pmdFree(packet);
}

void Send_Module_Data_Ex(I2C_Bus_Master *bus, MASTER_BUS_PACKET * packet, uint8_t address, uint8_t response_expected)
{
	I2C_Bus_Master_Send(bus,
						address,
						packet->Port,
						(I2C_Packet_Header_Type*)packet->Data,
						&packet->Data[IBM_PACKET_HDR_SIZE],
						response_expected);

	// I2C_Bus_Master_Send makes a copy of the packet, so we can pmdFree it now
	pmdFree(packet);
}

#include <ctype.h>
//fix display issue
static void check_msg_char(char *msg,int len);
void check_msg_char(char *msg,int len)
{
	int i;
	char c;
	for(i=0;i<len;i++)
	{
		c=msg[i];
		if(isprint(c))
			continue;
		else
			msg[i]=' ';
	}
	msg[i]='\0';
}

extern void i2c_bus_send_msg(I2C_Bus_Master *bm,uint8_t port, uint8_t *packet);
uint8_t Process_Message(I2C_Bus_Master *bm, uint8_t port, uint8_t *packet)
{
	uint8_t result = 1;
	i2c_bus_send_msg(bm,port, packet);
	return result;
}

#include "eos/eos.h"
void I2C_Bus_Continue_Process_Message(I2C_Bus_Master *bm, uint8_t port, uint8_t *packet)
{
	int len;

	I2C_Packet_Header_Type *hdr = (I2C_Packet_Header_Type*)packet;

	switch(hdr->Command)
	{
	// Handle generic responses
	case dmc_Start_Firmware_Update:
		formatlog("Main_Controller","Mod_Bus","Received Firmware Update Request\r\n");
		Process_Firmware_Update_Request(bm, port, packet);
		eos_async_send(9,EOS_USER_EVENT,port,packet);
		break;
	case dmc_Timestamp:
		formatlog("Main_Controller","Mod_Bus","Received Timestamp Request\r\n");
		Process_Timestamp_Request(bm, port, packet);
		break;
	case 0x15:
		len=hdr->Data_Length;			//Steve Rui, Print message from module,for debug
		len &= 0xff;
//		len += sizeof(I2C_Packet_Header_Type);
//		packet[len-1]='\0';
		char *msg;
		msg = (char *)packet+sizeof(I2C_Packet_Header_Type);
		check_msg_char(msg,len);
		formatlog("Main_Controller","Mod_Bus","Module %d Print,%s\r\n",port, msg);
		break;
	case dmc_PowerCommand:
		formatlog("Main_Controller","Mod_Bus","Received Power Command Message\r\n");
		Process_Module_Power_Command_Message(bm, port, packet);
		break;
	default:
		if(bm->RxData_Callback)
		{
			bm->RxData_Callback(port, hdr, (uint8_t*)&(packet[IBM_PACKET_HDR_SIZE]));
		}
		break;
	}

	pmdFree(packet);
}

void Disable_All_Module_UART_Ports(void)
{
	Disable_Port(eMODULE_PORT_1);
	Disable_Port(eMODULE_PORT_2);
	Disable_Port(eMODULE_PORT_3);
	Disable_Port(eMODULE_PORT_4);
	Disable_Port(eMODULE_PORT_5);
	Disable_Port(eMODULE_PORT_6);
	Disable_Port(eMODULE_PORT_7);
}

void Switch_Module_UART_Port(eMODULE_PORTS port)
{
	Module_Bus.Active_Module_Port = port;

	switch(port)
	{
	case eMODULE_PORT_1:
		{
			Disable_Port(eMODULE_PORT_2);
			Disable_Port(eMODULE_PORT_3);
			Disable_Port(eMODULE_PORT_4);
			Disable_Port(eMODULE_PORT_5);
			Disable_Port(eMODULE_PORT_6);
			Disable_Port(eMODULE_PORT_7);
			Enable_Port(eMODULE_PORT_1);
		}
		break;
	case eMODULE_PORT_2:
		{
			Disable_Port(eMODULE_PORT_1);
			Disable_Port(eMODULE_PORT_3);
			Disable_Port(eMODULE_PORT_4);
			Disable_Port(eMODULE_PORT_5);
			Disable_Port(eMODULE_PORT_6);
			Disable_Port(eMODULE_PORT_7);
			Enable_Port(eMODULE_PORT_2);
		}
		break;
	case eMODULE_PORT_3:
		{
			Disable_Port(eMODULE_PORT_1);
			Disable_Port(eMODULE_PORT_2);
			Disable_Port(eMODULE_PORT_4);
			Disable_Port(eMODULE_PORT_5);
			Disable_Port(eMODULE_PORT_6);
			Disable_Port(eMODULE_PORT_7);
			Enable_Port(eMODULE_PORT_3);
		}
		break;
	case eMODULE_PORT_4:
		{
			Disable_Port(eMODULE_PORT_1);
			Disable_Port(eMODULE_PORT_2);
			Disable_Port(eMODULE_PORT_3);
			Disable_Port(eMODULE_PORT_5);
			Disable_Port(eMODULE_PORT_6);
			Disable_Port(eMODULE_PORT_7);
			Enable_Port(eMODULE_PORT_4);
		}
		break;
	case eMODULE_PORT_5:
		{
			Disable_Port(eMODULE_PORT_1);
			Disable_Port(eMODULE_PORT_2);
			Disable_Port(eMODULE_PORT_3);
			Disable_Port(eMODULE_PORT_4);
			Disable_Port(eMODULE_PORT_6);
			Disable_Port(eMODULE_PORT_7);
			Enable_Port(eMODULE_PORT_5);
		}
		break;
	case eMODULE_PORT_6:
		{
			Disable_Port(eMODULE_PORT_1);
			Disable_Port(eMODULE_PORT_2);
			Disable_Port(eMODULE_PORT_3);
			Disable_Port(eMODULE_PORT_4);
			Disable_Port(eMODULE_PORT_5);
			Disable_Port(eMODULE_PORT_7);
			Enable_Port(eMODULE_PORT_6);
		}
		break;
	case eMODULE_PORT_7:
		{
			Disable_Port(eMODULE_PORT_1);
			Disable_Port(eMODULE_PORT_2);
			Disable_Port(eMODULE_PORT_3);
			Disable_Port(eMODULE_PORT_4);
			Disable_Port(eMODULE_PORT_5);
			Disable_Port(eMODULE_PORT_6);
			Enable_Port(eMODULE_PORT_7);
		}
		break;
	default:
		// If port number is out of bounds, disable all ports
		Disable_Port(eMODULE_PORT_1);
		Disable_Port(eMODULE_PORT_2);
		Disable_Port(eMODULE_PORT_3);
		Disable_Port(eMODULE_PORT_4);
		Disable_Port(eMODULE_PORT_5);
		Disable_Port(eMODULE_PORT_6);
		Disable_Port(eMODULE_PORT_7);
		break;
	}
}



void Enable_Port(eMODULE_PORTS port)
{
	switch(port)
	{
	case eMODULE_PORT_1:
		{
			HAL_GPIO_WritePin(SLOT1_nOE_GPIO_Port, SLOT1_nOE_Pin, GPIO_PIN_RESET);
		}
		break;
	case eMODULE_PORT_2:
		{
			HAL_GPIO_WritePin(SLOT2_nOE_GPIO_Port, SLOT2_nOE_Pin, GPIO_PIN_RESET);
		}
		break;
	case eMODULE_PORT_3:
		{
			HAL_GPIO_WritePin(SLOT3_nOE_GPIO_Port, SLOT3_nOE_Pin, GPIO_PIN_RESET);
		}
		break;
	case eMODULE_PORT_4:
		{
			HAL_GPIO_WritePin(SLOT4_nOE_GPIO_Port, SLOT4_nOE_Pin, GPIO_PIN_RESET);
		}
		break;
	case eMODULE_PORT_5:
		{
			HAL_GPIO_WritePin(SLOT5_nOE_GPIO_Port, SLOT5_nOE_Pin, GPIO_PIN_RESET);
		}
		break;
	case eMODULE_PORT_6:
		{
			if(NUM_MODULE_PORTS_PRESENT > 5)
			{
				HAL_GPIO_WritePin(SLOT6_nOE_GPIO_Port, SLOT6_nOE_Pin, GPIO_PIN_RESET);
			}
		}
		break;
	case eMODULE_PORT_7:
		{
			if(NUM_MODULE_PORTS_PRESENT > 6)
			{
				HAL_GPIO_WritePin(SLOT7_nOE_GPIO_Port, SLOT7_nOE_Pin, GPIO_PIN_RESET);
			}
		}
		break;
	}

}

extern uint8_t Slot_X_Enum_Flag[];

void Set_Port_Resets(uint8_t portmask)
{

	if(portmask & (1 << eMODULE_PORT_1))
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_1] = 0; // ZGy_Note
		SLOT1_nRESET_GPIO_Port->ODR &= ~SLOT1_nRESET_Pin;
	}
	else
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_1] = 1; // ZGy_Note
		SLOT1_nRESET_GPIO_Port->ODR |= SLOT1_nRESET_Pin;
	}

	if(portmask & (1 << eMODULE_PORT_2))
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_2] = 0; // ZGy_Note
		SLOT2_nRESET_GPIO_Port->ODR &= ~SLOT2_nRESET_Pin;
	}
	else
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_2] = 1; // ZGy_Note
		SLOT2_nRESET_GPIO_Port->ODR |= SLOT2_nRESET_Pin;
	}

	if(portmask & (1 << eMODULE_PORT_3))
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_3] = 0; // ZGy_Note
		SLOT3_nRESET_GPIO_Port->ODR &= ~SLOT3_nRESET_Pin;
	}
	else
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_3] = 1; // ZGy_Note
		SLOT3_nRESET_GPIO_Port->ODR |= SLOT3_nRESET_Pin;
	}

	if(portmask & (1 << eMODULE_PORT_4))
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_4] = 0; // ZGy_Note
		SLOT4_nRESET_GPIO_Port->ODR &= ~SLOT4_nRESET_Pin;
	}
	else
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_4] = 1; // ZGy_Note
		SLOT4_nRESET_GPIO_Port->ODR |= SLOT4_nRESET_Pin;
	}

	if(portmask & (1 << eMODULE_PORT_5))
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_5] = 1; // ZGy_Note
		SLOT5_nRESET_GPIO_Port->ODR &= ~SLOT5_nRESET_Pin;
	}
	else
	{
		//Slot_X_Enum_Flag[eMODULE_PORT_5] = 0; // ZGy_Note
		SLOT5_nRESET_GPIO_Port->ODR |= SLOT5_nRESET_Pin;
	}

	if(NUM_MODULE_PORTS_PRESENT > 5)
	{
		if(portmask & (1 << eMODULE_PORT_6))
		{
			//Slot_X_Enum_Flag[eMODULE_PORT_6] = 0; // ZGy_Note
			SLOT6_nRESET_GPIO_Port->ODR &= ~SLOT6_nRESET_Pin;
		}
		else
		{
			//Slot_X_Enum_Flag[eMODULE_PORT_6] = 1; // ZGy_Note
			SLOT6_nRESET_GPIO_Port->ODR |= SLOT6_nRESET_Pin;
		}
	}

	if(NUM_MODULE_PORTS_PRESENT > 6)
	{
		if(portmask & (1 << eMODULE_PORT_7))
		{
			//Slot_X_Enum_Flag[eMODULE_PORT_7] = 0; // ZGy_Note
			SLOT7_nRESET_GPIO_Port->ODR &= ~SLOT7_nRESET_Pin;
		}
		else
		{
			//Slot_X_Enum_Flag[eMODULE_PORT_7] = 1; // ZGy_Note
			SLOT7_nRESET_GPIO_Port->ODR |= SLOT7_nRESET_Pin;
		}
	}

}

void Disable_Port(eMODULE_PORTS port)
{
	switch(port)
	{
	case eMODULE_PORT_1:
		{
			HAL_GPIO_WritePin(SLOT1_nOE_GPIO_Port, SLOT1_nOE_Pin, GPIO_PIN_SET);
		}
		break;
	case eMODULE_PORT_2:
		{
			HAL_GPIO_WritePin(SLOT2_nOE_GPIO_Port, SLOT2_nOE_Pin, GPIO_PIN_SET);
		}
		break;
	case eMODULE_PORT_3:
		{
			HAL_GPIO_WritePin(SLOT3_nOE_GPIO_Port, SLOT3_nOE_Pin, GPIO_PIN_SET);
		}
		break;
	case eMODULE_PORT_4:
		{
			HAL_GPIO_WritePin(SLOT4_nOE_GPIO_Port, SLOT4_nOE_Pin, GPIO_PIN_SET);
		}
		break;
	case eMODULE_PORT_5:
		{
			HAL_GPIO_WritePin(SLOT5_nOE_GPIO_Port, SLOT5_nOE_Pin, GPIO_PIN_SET);
		}
		break;
	case eMODULE_PORT_6:
		{
			if(NUM_MODULE_PORTS_PRESENT > 5)
			{
				HAL_GPIO_WritePin(SLOT6_nOE_GPIO_Port, SLOT6_nOE_Pin, GPIO_PIN_SET);
			}
		}
		break;
	case eMODULE_PORT_7:
		{
			if(NUM_MODULE_PORTS_PRESENT > 6)
			{
				HAL_GPIO_WritePin(SLOT7_nOE_GPIO_Port, SLOT7_nOE_Pin, GPIO_PIN_SET);
			}
		}
		break;
	}
}
