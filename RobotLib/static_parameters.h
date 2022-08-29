#ifndef STATIC_PARAMETERS_H_
#define STATIC_PARAMETERS_H_

#include "../iodefine.h"

//モーター関連ピンの定義
#define MOTOR_EN_OUT PORTC.PODR.BIT.B6
#define MOTOR_RIGHT_PWM_OUT PORTC.PODR.BIT.B3
#define MOTOR_RIGHT_DIR_OUT PORTC.PODR.BIT.B5
#define MOTOR_LEFT_PWM_OUT PORTC.PODR.BIT.B1
#define MOTOR_LEFT_DIR_OUT PORTC.PODR.BIT.B4

#define TRUE 1
#define FALSE 0

#define MTU0_ENABLE MTU.TSTR.BIT.CST0

//PFSWEへの書き込み禁止を解除
//PmnPFSレジスタへの書き込み許可を設定
#define PmnPFS_allow() {\
    MPC.PWPR.BIT.B0WI=0;\
	MPC.PWPR.BIT.PFSWE=1;\
}

//PFSWEへの書き込み禁止を設定
#define PmnPFS_prohibit(){\
    MPC.PWPR.BYTE=0x80;\
}

#endif