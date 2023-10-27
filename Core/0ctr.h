
/*------------------------------------
 * 0ctr.h
 * Create:  2023-05-31
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */



#ifndef Core_0CTR_H_
#define Core_0CTR_H_
/*================================================================*/
#define BSP_ACI_4G_NM	0		//NimbeLink
#define BSP_ACI_4G_EVL	0
#if BSP_ACI_4G_EVL
#define BSP_ACI_RV5		1
#endif
#define BSP_ACI_PMG		1
#if BSP_ACI_4G_NM
#endif
#if BSP_ACI_4G_EVL
#endif
#if BSP_ACI_PMG
#endif
/*------------------------------------*/
#define SOC_STM_L0		BSP_ACI_4G_NM
#define SOC_STM_L4		BSP_ACI_4G_EVL
#define SOC_STM_F7		BSP_ACI_PMG
#if SOC_STM_L0
#endif
#if SOC_STM_L4
#endif
#if SOC_STM_F7
#endif
/*------------------------------------*/
#define CPU_ARM_M0		SOC_STM_L0
#define CPU_ARM_M4		SOC_STM_L4
#define CPU_ARM_M7		SOC_STM_F7
#if CPU_ARM_M0
#endif
#if CPU_ARM_M4
#endif
#if CPU_ARM_M7
#endif
/*================================================================*/
#endif

/* end of 0ctr.h */
