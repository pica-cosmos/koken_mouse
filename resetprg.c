/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/*********************************************************************
*
* Device     : RX/RX600
*
* File Name  : resetprg.c
*
* Abstract   : Reset Program.
*
* History    : 1.00  (2009-08-07)
*            : 1.10  (2011-02-21)
*            : 1.11  (2011-06-20)
*            : 1.12  (2014-02-20)
*            : 1.20  (2014-10-22)
*            : 1.30  (2018-03-26)
*            : 1.40  (2018-09-26)
*            : 1.50  (2019-04-12)
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2009 (2011-2019) Renesas Electronics Corporation.
* All rights reserved.
*
*********************************************************************/

#include	<machine.h>
#include	<_h_c_lib.h>
//#include	<stddef.h>					// Remove the comment when you use errno
//#include 	<stdlib.h>					// Remove the comment when you use rand()
#include	"typedefine.h"		// Define Types
#include	"stacksct.h"		// Stack Sizes (Interrupt and User)

#ifdef __cplusplus
extern "C" {
#endif
void PowerON_Reset_PC(void);
#ifdef __cplusplus
}
#endif
void main(void);

//#ifdef __cplusplus				// Use SIM I/O
//extern "C" {
//#endif
//extern void _INIT_IOLIB(void);
//extern void _CLOSEALL(void);
//#ifdef __cplusplus
//}
//#endif

#define PSW_init       0x00010000	// PSW bit pattern
#define FPSW_DPSW_init 0x00000000	// FPSW/DPSW bit base pattern

//extern void srand(_UINT);		// Remove the comment when you use rand()
//extern _SBYTE *_s1ptr;				// Remove the comment when you use strtok()
		
//#ifdef __cplusplus				// Use Hardware Setup
//extern "C" {
//#endif
//extern void HardwareSetup(void);
//#ifdef __cplusplus
//}
//#endif
	
//#ifdef __cplusplus			// Remove the comment when you use global class object
//extern "C" {					// Sections C$INIT and C$END will be generated
//#endif
//extern void _CALL_INIT(void);
//extern void _CALL_END(void);
//#ifdef __cplusplus
//}
//#endif

#pragma section ResetPRG		// output PowerON_Reset to PResetPRG section

#pragma entry PowerON_Reset_PC

void PowerON_Reset_PC(void)
{ 
#if (__RX_ISA_VERSION__ >= 2) || defined(__RXV2)
	set_extb(__sectop("EXCEPTVECT"));
#endif
	set_intb(__sectop("C$VECT"));

#ifdef __FPU
#ifdef __ROZ						// Initialize FPSW/DPSW
#define _ROUND 0x00000001			// Let FPSW/DPSW RM/DRM bits=01 (round to zero)
#else
#define _ROUND 0x00000000			// Let FPSW/DPSW RM/DRM bits=00 (round to nearest)
#endif
#ifdef __DOFF
#define _DENOM 0x00000100			// Let FPSW/DPSW DN/DDN bit=1 (denormal as zero)
#else
#define _DENOM 0x00000000			// Let FPSW/DPSW DN/DDN bit=0 (denormal as is)
#endif
	set_fpsw(FPSW_DPSW_init | _ROUND | _DENOM);
#ifdef __DPFPU
	__set_dpsw(FPSW_DPSW_init | _ROUND | _DENOM);
#endif
#endif

#ifdef __TFU
	__init_tfu();					// Initialize TFU
#endif

	_INITSCT();

//	_INIT_IOLIB();					// Use SIM I/O

//	errno=0;						// Remove the comment when you use errno
//	srand((_UINT)1);					// Remove the comment when you use rand()
//	_s1ptr=NULL;					// Remove the comment when you use strtok()
		
//	HardwareSetup();				// Use Hardware Setup

//	_CALL_INIT();					// Remove the comment when you use global class object

	set_psw(PSW_init);				// Initialize Ubit & Ibit for PSW
//	chg_pmusr();					// Remove the comment when you need to change PSW PMbit (SuperVisor->User)

	main();

//	_CLOSEALL();					// Use SIM I/O
	
//	_CALL_END();					// Remove the comment when you use global class object

	brk();
}

