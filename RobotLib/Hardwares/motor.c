#include "motor.h"
#include "../../iodefine.h"
#include "../Definations/system_definations.h"

void motor_on(int R_dir, int L_dir, int R_duty, int L_duty){

	motor_off();	//念のためモーターをオフにする

	//デューティー比を設定、カウンタをクリア
	MTU0.TGRA = L_duty;		//左のデューティー比を設定 [ TGRA(C) / TGRD ]
	MTU0.TGRC = R_duty;		//右のデューティー比を設定
	MTU0.TCNT = 0;		//割り込み開始前にカウンタをクリア

	MOTOR_EN_OUT = 1;	//SLEEP解除

	//モーターの回転方向を設定
	MOTOR_RIGHT_DIR_OUT =R_dir;	//R_PH 
	MOTOR_LEFT_DIR_OUT = L_dir;	//L_PH 
	MTU.TSTR.BIT.CST0 = 1; 		//MTU0のカウンタ動作開始

}

void motor_off(void){

	MOTOR_EN_OUT = 0;			//SLEEP設定
	MTU.TSTR.BIT.CST0 = 0; 		//MTU0のカウンタ動作停止
}
