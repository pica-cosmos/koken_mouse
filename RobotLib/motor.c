#include "../iodefine.h"
#include "motor.h"


//モータ系レジスタの設定
void motor_init(void){

	PORTC.PODR.BIT.B6 = 0;	//motor をSLEEPにする

	//モーター系ピンを出力に
	PORTC.PDR.BIT.B6 = 1;//motor SLEEP (STBY)
	PORTC.PDR.BIT.B5 = 1;//Rmotor PH
	PORTB.PDR.BIT.B3 = 1;//Rmotor EN
	PORTC.PDR.BIT.B4 = 1;//Lmotor PH
	PORTB.PDR.BIT.B1 = 1;//Lmotor EN
	
	//機能ピン設定	
	MPC.PWPR.BIT.B0WI=0;	//PFSWEへの書き込み禁止を解除
	MPC.PWPR.BIT.PFSWE=1;	//PmnPFSレジスタへの書き込み許可を設定
	MPC.PB1PFS.BIT.PSEL=1;	//PWM R MTIOC0C	PB1を割り込みピンとして設定
	MPC.PB3PFS.BIT.PSEL=1;	//PWM L MTIOC0A	PB3を割り込みピンとして設定
	MPC.PWPR.BYTE=0x80;		//PFSWEへの書き込み禁止を設定
	
	//MTUのイニシャライズ
	SYSTEM.PRCR.WORD = 0xA502;	//プロテクト解除
	MSTP(MTU) = 0;				// MTU (MTU0〜MTU5)のストップ状態解除
	SYSTEM.PRCR.WORD = 0xA500;	//プロテクト設定
	
	//ピンや機能設定時にはタイマストップ
	MTU.TSTR.BYTE=0;	//MTUのカウント停止
	
	//左右モータ用MTU0 PWM2 時定数τ=L/R=17uH/(1.07+0.5+0.3)=110kHz
	MTU0.TCR.BIT.TPSC=0;	//PCLK/1 48MHz
	MTU0.TCR.BIT.CCLR=6;	//PWM TGRDのコンペアマッチでTCNT(カウンタ)クリア
	MTU0.TMDR.BIT.MD=3;		//PWM2を選択。複数のPWMが生成可能

	//割り込み時のPWMの動作を設定。IOA/IOC : PWM / IOD : 周期
	MTU0.TIORH.BIT.IOA=5;	//初期出力0コンペアマッチPWMの立下り0を出力	PB3
	MTU0.TIORL.BIT.IOC=5;	//初期出力0コンペアマッチPWMの立下り0を出力 PB1
	MTU0.TIORL.BIT.IOD=2;	//初期出力0コンペアマッチPWMの周期立上り1を出力

	MTU0.TGRA = 0;			//初期化
	MTU0.TGRC = 0;			//初期化
	MTU0.TGRD = 240 - 1;	//周期 200kHz タイマカウンタは０から始まるので -1 をする
}

void motor_on(int R_dir, int L_dir, int R_duty, int L_duty){

	//デューティー比を設定、カウンタをクリア
	MTU0.TGRA = L_duty;		//左のデューティー比を設定 [ TGRA(C) / TGRD ]
	MTU0.TGRC = R_duty;		//右のデューティー比を設定
	MTU0.TCNT = 0;		//割り込み開始前にカウンタをクリア

	//モーターのスリーブを解除、イネーブルをオン
	PORTC.PODR.BIT.B6 = 1;	//SLEEP解除
	PORTC.PODR.BIT.B3 = 1;	//Rmotor EN on
	PORTC.PODR.BIT.B1 = 1;	//Lmotor EN on

	//モーターの回転方向を設定
	PORTC.PODR.BIT.B5 = 1;	//R_PH 
	PORTC.PODR.BIT.B4 = 1;	//L_PH 
	MTU.TSTR.BIT.CST0 = 1; //MTU0のカウンタ動作開始

}

void motor_off(void){

	PORTC.PODR.BIT.B6 = 0;	//SLEEP設定
	PORTC.PODR.BIT.B3 = 0;	//Rmotor EN off
	PORTC.PODR.BIT.B1 = 0;	//Lmotor EN off
	MTU.TSTR.BIT.CST0 = 0; //MTU0のカウンタ動作停止
}
