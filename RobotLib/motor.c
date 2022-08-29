#include "../iodefine.h"
#include "motor.h"
#include "static_parameters.h"

//モータ系レジスタの設定
void motor_init(void){

	MOTOR_EN_OUT = 0;	//予めmotor をSLEEPにする。モーターがハイインピーダンスで誤動作するのを防ぐ

	//モーター系ピンを出力に
	PORTC.PDR.BIT.B6 = 1;//motor SLEEP (STBY)
	PORTC.PDR.BIT.B5 = 1;//Rmotor PH
	PORTB.PDR.BIT.B3 = 1;//Rmotor EN
	PORTC.PDR.BIT.B4 = 1;//Lmotor PH
	PORTB.PDR.BIT.B1 = 1;//Lmotor EN
	
	//機能ピン設定	
	PmnPFS_allow();			//PFSWEへの書き込み許可を設定
	MPC.PB1PFS.BIT.PSEL=1;	//PWM R MTIOC0C	PB1を割り込みピンとして設定
	MPC.PB3PFS.BIT.PSEL=1;	//PWM L MTIOC0A	PB3を割り込みピンとして設定
	PmnPFS_prohibit();		//PFSWEへの書き込み禁止を設定
	
	//PB1,3を機能ポートに設定
	PORTB.PMR.BIT.B3=1;
	PORTB.PMR.BIT.B1=1;

	//MTUのイニシャライズ
	SYSTEM.PRCR.WORD = 0xA502;	//プロテクト解除
	MSTP(MTU) = 0;				// MTU (MTU0〜MTU5)のストップ状態解除
	SYSTEM.PRCR.WORD = 0xA500;	//プロテクト設定
	
	//ピンや機能設定時にはタイマストップ
	MTU.TSTR.BYTE=0;	//MTUのカウント停止
	
	//左右モータ用MTU0 PWM2 時定数τ=L/R=17uH/(1.07+0.5+0.3)=110kHz
	MTU0.TCR.BIT.TPSC=0;	//プリスケーラ― PCLK/1 48MHz
	MTU0.TCR.BIT.CCLR=6;	//PWM TGRDのコンペアマッチでTCNT(カウンタ)クリア

	MTU0.TMDR.BIT.MD=3;		//PWM2を選択。複数のPWMが生成可能

	//割り込み時のPWMの動作を設定。IOA/IOC : PWM / IOD : 周期
	MTU0.TIORH.BIT.IOA=5;	//初期出力0コンペアマッチPWMの立下り0を出力	PB3
	MTU0.TIORL.BIT.IOC=5;	//初期出力0コンペアマッチPWMの立下り0を出力 PB1
	MTU0.TIORL.BIT.IOD=2;	//初期出力0コンペアマッチPWMの周期立上り1を出力

	MTU0.TGRA = 0;			//初期化 値が4以下だと動作しない
	MTU0.TGRC = 0;			//初期化
	MTU0.TGRD = 240 - 1;	//周期 200kHz タイマカウンタは０から始まるので -1 をする

	/* 	割込み信号が立つと、TGIA0がセット。割込み先が割り込み要求を受け入れるとTGIA0がクリア
		TGIA0を使いたい場合は、下のコメントアウトを取る
	*/

	// MTU0.TIER.BIT.TGIEA = 1;	//割り込み要求（TGIA）を許可
	// IEN(MTU0,TGIA0) = 0;		//IERレジスタを操作する事で、割込みコントローラー内の割込み要求を有効にする
	// IPR(MTU0,TGIA0) = 13;		//割込みの優先順位を指定。優先順位を高くする
	// IR(MTU0,TGIA0) = 0;			//IRレジスタをクリアして、割込み要求の受け入れ体制を作る
}

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
	MTU0_ENABLE = 1; //MTU0のカウンタ動作開始

}

void motor_off(void){

	MOTOR_EN_OUT = 0;	//SLEEP設定
	MTU0_ENABLE = 0; 		//MTU0のカウンタ動作停止
}
