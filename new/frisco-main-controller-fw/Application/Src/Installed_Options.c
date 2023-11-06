/*
 * Installed_Options.c
 *
 *  Created on: Jun 14, 2018
 *      Author: mikea
 */
#include <string.h>
#include <stdbool.h>
#include "util.h"
#include "Installed_Options.h"
#include "FW_Version.h"
#include "Registration_Data.h"
#include "App_Data.h"
#include "Debug_Log_Msg.h"

struct opt_node
{
	const AC_PMG_Option_Mod_t 	*opt;
	struct opt_node				*next;
};

static uint8_t opt_count = 0;
static struct opt_node			*opt_list_head = NULL;

/**************************************************************************************
 * @note	The Option registry will "own" the opt data passed in, so calling function
 *			should not free the structure after registration.
 **************************************************************************************/
void Register_Installed_Option(AC_PMG_Option_Mod_t *opt)
{
	// Traverse the options list and make sure the option is not already present in the list
	const AC_PMG_Option_Mod_t **ppopt;
	bool option_present = false;

	ppopt = Get_Next_Option(NULL);

	while(ppopt)
	{
		if((*ppopt)->Model_Number == opt->Model_Number && strncmp((const char*)(*ppopt)->Serial_Number, (const char*)opt->Serial_Number, sizeof(opt->Serial_Number)) == 0)
		{
			option_present = true;
			break;
		}

		ppopt = Get_Next_Option(ppopt);
	}

	if(option_present)
	{
		// If the option is present in the list, then just update the original option entry
		memcpy(*(AC_PMG_Option_Mod_t **)ppopt, opt, sizeof(AC_PMG_Option_Mod_t));
	}
	else
	{
		// Otherwise, add a new node to the list
		struct opt_node *node = (struct opt_node*)pmdMalloc(sizeof(struct opt_node));

		node->next = opt_list_head;
		opt_list_head = node;
		node->opt = opt;
		++opt_count;
	}
}

void Remove_Installed_Option(uint16_t Model_Number)
{
	if((opt_count > 0) && Is_Option_Present(Model_Number))
	{
		if((opt_count == 1) && (opt_list_head->opt->Model_Number == Model_Number))
		{
			// Delete the head
			pmdFree((void*)opt_list_head->opt);
			pmdFree(opt_list_head);
			opt_list_head = NULL;
			opt_count = 0;
		}
		else
		{
			struct opt_node * travler = opt_list_head;

			// check if it is the head
			if(travler->opt->Model_Number == Model_Number)
			{
				opt_list_head = opt_list_head->next;
				opt_count--;

				pmdFree((void*)travler->opt);
				pmdFree(travler);
			}
			else
			{
				while(travler != NULL)
				{
					if(travler->next->opt->Model_Number == Model_Number)
					{
						// Check there is something to link to
						if(travler->next->next != NULL)
						{
							struct opt_node * tmp = travler->next;
							travler->next = travler->next->next;

							pmdFree((void*)tmp->opt);
							pmdFree(tmp);
							opt_count--;
						}
						else
						{
							// The item is the last in the list
							pmdFree((void*)travler->next->opt);
							pmdFree(travler->next);
							opt_count--;
							travler->next = NULL;
						}

						break;
					}

					travler = travler->next;
				}
			}
		}
	}
}

uint8_t Get_Num_Installed_Options(void)
{
	return opt_count;
}

void Save_Controller_As_Option(void)
{
	AC_PMG_Option_Mod_t *popt = (AC_PMG_Option_Mod_t*)pmdMalloc(sizeof(AC_PMG_Option_Mod_t));

#ifdef DISPLAY_PANEL_12_INCH
	popt->Model_Number = 5500;
	popt->Hardware_Version[0] = '0';
	popt->Hardware_Version[1] = 'B';
	popt->Hardware_Version[2] = '\0';
#elif defined(DISPLAY_PANEL_15_INCH)
	popt->Model_Number = 5542;
	popt->Hardware_Version[0] = '0';
	popt->Hardware_Version[1] = 'A';
	popt->Hardware_Version[2] = '\0';
#else	// DISPLAY_PANEL_18_INCH
	popt->Model_Number = 5543;
	popt->Hardware_Version[0] = '0';
	popt->Hardware_Version[1] = 'A';
	popt->Hardware_Version[2] = '\0';
#endif

	popt->Subtype = 0;
	popt->Instance = 0;
	memcpy((char*)popt->Serial_Number, Regn_Data_Image.SerNum, 8);
	popt->Serial_Number[8] = '\0';
	popt->Firmware_Version_Major = MAJOR_FW_VERSION+1;		//+1 means using eos!
	popt->Firmware_Version_Minor = MINOR_FW_VERSION;
	popt->Firmware_Version_Sub = SUB_FW_VERSION;
	popt->Firmware_Version_Test = TEST_FW_VERSION;
	popt->ModuleLib_Version_Major = 0xFF;
	popt->ModuleLib_Version_Minor = 0xFF;
	popt->ModuleLib_Version_Sub = 0xFF;
	popt->ModuleLib_Version_Test = 0xFF;

	Register_Installed_Option(popt);

	// Save off any enabled software features
	if(App_Data.Enabled_Features.Features.Text_Graphics_Animations)
	{
		popt = (AC_PMG_Option_Mod_t*)pmdMalloc(sizeof(AC_PMG_Option_Mod_t));

		popt->Model_Number = 126;
		popt->Subtype = 1;
		popt->Instance = 0;
		popt->Hardware_Version[0] = '\0';
		popt->Hardware_Version[1] = '\0';
		popt->Hardware_Version[2] = '\0';

		for(uint8_t counter = 0; counter < sizeof(popt->Serial_Number); counter++)
		{
			popt->Serial_Number[counter] = '\0';
		}

		popt->Firmware_Version_Major = 0xFF;
		popt->Firmware_Version_Minor = 0xFF;
		popt->Firmware_Version_Sub = 0xFF;
		popt->Firmware_Version_Test = 0xFF;
		popt->ModuleLib_Version_Major = 0xFF;
		popt->ModuleLib_Version_Minor = 0xFF;
		popt->ModuleLib_Version_Sub = 0xFF;
		popt->ModuleLib_Version_Test = 0xFF;

		Register_Installed_Option(popt);
	}

	if(App_Data.Enabled_Features.Features.NTCIP_Support)
	{
		AC_PMG_Option_Mod_t * popt = (AC_PMG_Option_Mod_t*)pmdMalloc(sizeof(AC_PMG_Option_Mod_t));

		popt->Model_Number = ModId_NTCIP_Feature;
		popt->Subtype = 0;
		popt->Instance = 0;
		popt->Hardware_Version[0] = '\0';
		popt->Hardware_Version[1] = '\0';
		popt->Hardware_Version[2] = '\0';

		memset(popt->Serial_Number, '\0', sizeof(popt->Serial_Number));

		popt->Firmware_Version_Major = 0xFF;
		popt->Firmware_Version_Minor = 0xFF;
		popt->Firmware_Version_Sub = 0xFF;
		popt->Firmware_Version_Test = 0xFF;
		popt->ModuleLib_Version_Major = 0xFF;
		popt->ModuleLib_Version_Minor = 0xFF;
		popt->ModuleLib_Version_Sub = 0xFF;
		popt->ModuleLib_Version_Test = 0xFF;

		Register_Installed_Option(popt);
	}
}


/*************************************************************************************
 * @brief	Iterate over installed options list
 *
 * @param	iter	pass last return value to get next item in list.
 * 					pass NULL to get first item in list.
 * @return	Pointer to pointer to AC_PMG_Option_Mod_t struct
 *************************************************************************************/
const AC_PMG_Option_Mod_t **Get_Next_Option(const AC_PMG_Option_Mod_t **last_iter)
{
	struct opt_node *iter;

	if(opt_list_head == NULL)
	{
		return NULL;
	}
	else if(last_iter == NULL)
	{
		return &(opt_list_head->opt);
	}
	else
	{
		iter = ((struct opt_node *)last_iter)->next;

		if(iter)
		{
			return &iter->opt;
		}
		else
		{
			return NULL;
		}
	}
}



bool Is_Option_Present(Module_Id_Type_t id)
{
	const AC_PMG_Option_Mod_t **ppopt;

	ppopt = Get_Next_Option(NULL);

	while(ppopt)
	{
		if((*ppopt)->Model_Number == id)
		{
			return true;
		}

		ppopt = Get_Next_Option(ppopt);
	}

	return false;
}

const AC_PMG_Option_Mod_t *Get_Option(Module_Id_Type_t id, uint8_t instance)
{
	const AC_PMG_Option_Mod_t **ppopt;

	ppopt = Get_Next_Option(NULL);

	while(ppopt)
	{
		if((*ppopt)->Model_Number == id && (*ppopt)->Instance == instance)
		{
			return *ppopt;
		}

		ppopt = Get_Next_Option(ppopt);
	}

	return NULL;
}

void Log_Option(const AC_PMG_Option_Mod_t *popt)
{
	char strval[16];

	sprintf(strval, "%4d-%02d", popt->Model_Number, popt->Subtype);
	formatlog("Main_Controller", "Installed_Opt","Model Number:%s\r\n",strval);

	formatlog("Main_Controller", "Installed_Opt","Port Number:%d\r\n", popt->Instance);

	if(popt->Hardware_Version[0] == 255)
	{
		strncpy(strval, "N/A", 3);
		strval[3] = '\0';
	}
	else
	{
		sprintf(strval, "%2s", popt->Hardware_Version);
	}
	formatlog("Main_Controller", "Installed_Opt","Hardware_Version:%s\r\n", strval);
	if(strncmp((const char*)popt->Serial_Number, "None", 4) == 0)
	{
		strncpy(strval, "N/A", 3);
		strval[3] = '\0';
	}
	else
	{
		strncpy(strval, (const char *)popt->Serial_Number, 8);
		strval[8] = '\0';
	}
	formatlog("Main_Controller", "Installed_Opt","Serial_Number:%s\r\n", strval);

	if(popt->Firmware_Version_Major == 255)
	{
		strncpy(strval, "N/A", 3);
		strval[3] = '\0';
	}
	else
	{
		sprintf(strval, "%d.%d.%d.%d", popt->Firmware_Version_Major, popt->Firmware_Version_Minor, popt->Firmware_Version_Sub, popt->Firmware_Version_Test);
	}
	formatlog("Main_Controller", "Installed_Opt","Firmware Version:%s\r\n", strval);
	if(popt->ModuleLib_Version_Major == 255)
	{
		strncpy(strval, "N/A", 3);
		strval[3] = '\0';
	}
	else
	{
		sprintf(strval, "%d.%d.%d.%d", popt->ModuleLib_Version_Major, popt->ModuleLib_Version_Minor, popt->ModuleLib_Version_Sub, popt->ModuleLib_Version_Test);
	}
	formatlog("Main_Controller", "Installed_Opt","ModuleLib Version:%s\r\n", strval);
}

void Log_Installed_Options(void)
{
	const AC_PMG_Option_Mod_t **ppopt = Get_Next_Option(NULL);

	// Give the UART some time to send text
	HAL_Delay(5);

	while(ppopt)
	{
		const AC_PMG_Option_Mod_t *popt = *ppopt;

		Log_Option(popt);

		ppopt = Get_Next_Option(ppopt);

		// Give the UART some time to send text
		HAL_Delay(18);
	}

}

void Test_Get_Inst_Opt(void)
{
	const AC_PMG_Option_Mod_t *popt = Get_Option(5709, 0);

	if(popt)
	{
		Log_Option(popt);
	}
}



