#ifndef STATIC_DEFINATIONS_H_
#define STATIC_DEFINATIONS_H_

#include "../../iodefine.h"

//PFSWEへの書き込み禁止を解除
//PmnPFSレジスタへの書き込み許可を設定
#define PmnPFS_allow()  {   MPC.PWPR.BIT.B0WI=0;\
	                        MPC.PWPR.BIT.PFSWE=1;\
                        }

//PFSWEへの書き込み禁止を設定
#define PmnPFS_prohibit()   { MPC.PWPR.BYTE=0x80; }

#define system_protect_allow()      SYSTEM.PRCR.WORD = 0xA502
#define system_protect_prohibit()   SYSTEM.PRCR.WORD = 0xA500

//モーター関連ピンの定義
#define MOTOR_EN_OUT            PORTC.PODR.BIT.B6
#define MOTOR_RIGHT_PWM_OUT     PORTC.PODR.BIT.B3
#define MOTOR_RIGHT_DIR_OUT     PORTC.PODR.BIT.B5
#define MOTOR_LEFT_PWM_OUT      PORTC.PODR.BIT.B1
#define MOTOR_LEFT_DIR_OUT      PORTC.PODR.BIT.B4

#define MOT_OUT_R	MTU0.TGRA
#define MOT_OUT_L	MTU0.TGRC

#define ADC_START S12AD.ADCSR.BIT.ADST

#define LED_LEFT_OUT            PORTA.PODR.BIT.B3
#define LED_LEFT_FORWARD_OUT    PORT1.PODR.BIT.B5
#define LED_RIGHT_OUT           PORT1.PODR.BIT.B4
#define LED_RIGHT_FORWARD_OUT   PORT3.PODR.BIT.B1

#define DATA_SENSOR_FR      S12AD.ADDR6
#define DATA_SENSOR_R       S12AD.ADDR2
#define DATA_SENSOR_L       S12AD.ADDR1
#define DATA_SENSOR_FL      S12AD.ADDR0

#define DATA_VOLTAGE        S12AD.ADDR9

#endif