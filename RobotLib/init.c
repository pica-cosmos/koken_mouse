#include "init.h"
#include <machine.h>
#include "../iodefine.h"
#include "Definations/system_definations.h"
#include "Parameters/static_parameters.h"
#include "Parameters/adjust_parameters.h"
#include "interrupt.h"
#include "interface.h"
#include "Hardwares/i2c.h"
#include "Hardwares/sci.h"
#include "Hardwares/spi.h"
#include "glob_var.h"

extern void wait_ms(int miribyou);


void init_all(void){

	volatile long i;
	clrpsw_i();	//割込みを禁止	

	init_clk();		//各種基本機能を先に初期化する
	init_I2C();
	init_sci(115200);

	init_motor();
	init_wallsensor();
	init_buzzer();
	init_IOex();
	init_spi_enc();
	init_spi_gyro();
	init_maze();
	init_parameters();
	LED(0);
	
	//Gyro初期設定
	preprocess_spi_gyro_2byte(0x0681);		//ジャイロリセット
	for(i = 0; i < 100*1000*10; i++);
	preprocess_spi_gyro_2byte(0x0601);		//Low Power Mode OFF
	for(i = 0; i < 100*1000*10; i++);

	//ジャイロの設定
	preprocess_spi_gyro_2byte(0x7F20);		//User Bank2に変更
	for(i = 0; i < 100*1000*10; i++);
	preprocess_spi_gyro_2byte(0x0107);		//Range を最大2000dpsへ変更
	for(i = 0; i < 100*1000*10; i++);
	preprocess_spi_gyro_2byte(0x7F00);		//User Bank0に変更
	for(i = 0; i < 100*1000*10; i++);

	preprocess_spi_gyro_2byte(0x0621);		//ジャイロスタート
	//変数初期化
	timer = 0;

	init_CMT();
	setpsw_i();		//割込み許可
	timer_start();
}

//クロック設定
void init_clk(void){

    SYSTEM.PRCR.WORD = 0xa50b; //クロックソース選択の保護の解除

    SYSTEM.PLLCR.WORD = 0x0F00; /* PLL 逓倍×16 入力1分周 (12.000MHz * 16 = 192MHz)*/
                                // SYSTEM.PLLWTCR.BYTE = 0x0F;	    // 4194304cycle(Default) PLLが安定した周波数になるまでwait ※内部で制御されるので必要ない
    SYSTEM.PLLCR2.BYTE = 0x00;  // PLLEN : enable / PLL enable

    // ICK   : 192/2 = 96MHz 		// システムクロック CPU DMAC DTC ROM RAM
    // PCLKA : 192/2 = 96MHz 		// 周辺モジュールクロックA ETHERC、EDMAC、DEU
    // PCLKB : 192/4 = 48MHz 		// 周辺モジュールクロックB 上記以外 PCLKB=PCLK

    // SYSTEM.SCKCR.BIT.FCK=0x02;		//FCLK MAX 50MHz  192/4
    // SYSTEM.SCKCR.BIT.ICK=0x01;		//ICLK MAX 100MHz 192/2
    // SYSTEM.SCKCR.BIT.PSTOP1=0x01;		//BCLK 出力停止
    // SYSTEM.SCKCR.BIT.PSTOP0=0x01;		//SDCLK 出力停止
    // SYSTEM.SCKCR.BIT.BCK=0x02;		//BCLK MAX 100MHz ICLK以下にする必要がある192/4
    // SYSTEM.SCKCR.BIT.PCKA=0x01;		//PCLKA MAX 100MHz 192/2
    // SYSTEM.SCKCR.BIT.PCKB=0x02;		//PCLKB MAX 50MHz 192/4
    // 上記の設定では正しくclock設定ができないため下記のように一括で設定すること

    SYSTEM.SCKCR.LONG = 0x21C21211; // FCK1/4 ICK1/2 BCLK停止 SDCLK停止 BCK1/4 PCLKA1/2 PCLKB1/4

    // SYSTEM.SCKCR2.BIT.UCK=0x03;		//UCLK MAX 48MHz 192/4
    // SYSTEM.SCKCR2.BIT.IEBCK=0x02;		//IECLK MAX 50MHz 192/4

    SYSTEM.SCKCR2.WORD = 0x0032; /* UCLK1/4 IEBCK1/4 */
    SYSTEM.BCKCR.BYTE = 0x01;    /* BCLK = 1/2 */

    SYSTEM.SCKCR3.WORD = 0x0400; // PLL回路選択
}

void init_CMT(void){

    //CMTのプロテクトを解除
    system_protect_allow();
		MSTP(CMT0) = 0;
		MSTP(CMT1) = 0;
		MSTP(CMT2) - 0;
		MSTP(CMT3) = 0;
    system_protect_prohibit();

    //カウント停止
    CMT.CMSTR0.BIT.STR0 = 0;
	CMT.CMSTR0.BIT.STR1 = 0;
	CMT.CMSTR1.BIT.STR2 = 0;
	CMT.CMSTR1.BIT.STR3 = 0;

    //CMT0は制御割り込み用タイマとして使用
	CMT0.CMCR.BIT.CKS=1;	// PCLK/32 1.5MHz
	CMT0.CMCR.BIT.CMIE=1;	//割り込みを許可
	CMT0.CMCNT=0;			//カウンターのクリア
	CMT0.CMCOR=1500-1;		//1kHz

	IEN(CMT0,CMI0) = 1;		//割り込み要求を許可 
	IPR(CMT0,CMI0) = 15;	//割り込み優先度 15が最高
	IR(CMT0,CMI0)=0;		//割り込みステータフラグをクリア
	
	//CMT1はセンサー制御用タイマとして使用
	CMT1.CMCR.BIT.CKS=1;	// PCLK/32 1.5MHz
	CMT1.CMCR.BIT.CMIE=1;	//割り込みを許可
	CMT1.CMCNT=0;			//カウンターのクリア
	CMT1.CMCOR=(1500/4)-1;	//4kHz

	IEN(CMT1,CMI1) = 1;		//割り込み要求を許可 
	IPR(CMT1,CMI1) = 14;	//割り込み優先度を次点に設定
	IR(CMT1,CMI1)=0;		//割り込みステータフラグをクリア

	//CMT2はセンサー制御用タイマとして使用
	CMT2.CMCR.BIT.CKS=1;	// PCLK/32 1.5MHz
	CMT2.CMCR.BIT.CMIE=1;	//割り込みを許可
	CMT2.CMCNT=0;			//カウンターのクリア
	CMT2.CMCOR=(1500/2)-1;	//2kHz

	IEN(CMT2,CMI2) = 1;		//割り込み要求を許可 
	IPR(CMT2,CMI2) = 13;	//割り込み優先度を次点に設定
	IR(CMT2,CMI2)=0;		//割り込みステータフラグをクリア
	
    //CMT3は遅延ルーチン用タイマとして使用
    CMT3.CMCR.BIT.CKS = 2;      // PCLK 1/128 0.375MHZ
    CMT3.CMCR.BIT.CMIE = 1;     //コンペアマッチ割り込み許可
    IEN(CMT3, CMI3) = 1;		//割り込み要求を許可
    IPR(CMT3, CMI3) = 2;        //割り込み優先度を２に設定
    IR(CMT3, CMI3) = 0;			//割り込みステータフラグをクリア
}

void init_buzzer(void){

    PORTB.PODR.BIT.B5 = 0;
    PORTB.PDR.BIT.B5 = 1;       // output
    PORTB.PMR.BIT.B5 = 0;

    PmnPFS_allow();
    MPC.PB5PFS.BIT.PSEL = 1;
    PmnPFS_prohibit();
    PORTB.PMR.BIT.B5 = 1;       // PWM用機能ピンに設定

    system_protect_allow();
    MSTP(MTU) = 0;
    system_protect_prohibit();
	
    MTU.TSTR.BIT.CST2 = 0; // MTU2カウント停止
    MTU2.TCR.BIT.CCLR = 2; // TGRBのコンペアマッチでTCNTクリア
    MTU2.TCR.BIT.TPSC = 1; // TCLK/4 12MHZ
    MTU2.TMDR.BIT.MD = 2;  // mode : PWM1
    MTU2.TIOR.BIT.IOA = 2; //初期low コンペアマッチhigh
    MTU2.TIOR.BIT.IOB = 1; //初期low コンペアマッチlow
    MTU2.TCNT = 0;         //カウンタをクリア
}

//モータ系レジスタの設定
void init_motor(void){

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
	MSTP(MTU) = 0;				// MTU (MTU0?MTU5)のストップ状態解除
	SYSTEM.PRCR.WORD = 0xA500;	//プロテクト設定
	
	//ピンや機能設定時にはタイマストップ
	MTU.TSTR.BIT.CST0=0;	//MTU0のカウント停止
	
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

	MTU0.TIER.BIT.TGIEA = 1;	//割り込み要求（TGIA）を許可。割込みベクタは "Excep_MTU0_TGIA0"
	IEN(MTU0,TGIA0) = 1;		//IERレジスタを操作する事で、割込みコントローラー内の割込み要求を有効にする
	IPR(MTU0,TGIA0) = 12;		//割込みの優先順位を指定。優先順位を高くする
	IR(MTU0,TGIA0) = 0;			//IRレジスタをクリアして、割込み要求の受け入れ体制を作る

	MTU0.TIER.BIT.TGIEC = 1;	//割り込み要求(TGIC)を許可
	IEN(MTU0,TGIC0) = 1;
	IPR(MTU0,TGIC0) = 12;
	IR(MTU0,TGIC0) = 0;
}

void init_wallsensor(void){

	//ADCの設定
	system_protect_allow();		//A/Dコンバータに電力を供給
	MSTP(S12AD) = 0;
	system_protect_prohibit();

	S12AD.ADCSR.BIT.CKS = 3;	//A/D変換のクロックをPCLKの1分周(48M[Hz])に設定
	S12AD.ADCSR.BIT.ADCS = 0;	//シングルスキャンモードに設定
	
    //赤外LEDのピン設定
	PORTA.PDR.BIT.B3 = 1;	//PA3を出力用に設定
	PORT1.PDR.BIT.B5 = 1;	//P15を出力用に設定
	PORT1.PDR.BIT.B4 = 1;	//P14を出力用に設定
	PORT3.PDR.BIT.B1 = 1;	//P31を出力用に設定

    //A/DポートのPMR設定
	PORT4.PMR.BIT.B6=1;	//P46を周辺機能ポートに設定
	PORT4.PMR.BIT.B2=1;	//P46を周辺機能ポートに設定
	PORT4.PMR.BIT.B1=1;	//P46を周辺機能ポートに設定
	PORT4.PMR.BIT.B0=1;	//P46を周辺機能ポートに設定
	PORTE.PMR.BIT.B1=1;	//PE1を周辺機能ポートに設定 
	
    //A/DポートのPFS設定 アナログ端子として使用する
	PmnPFS_allow();		//プロテクト解除
	MPC.P46PFS.BIT.ASEL=1;	//A/D SEN_FR	AN006を使用
	MPC.P42PFS.BIT.ASEL=1;	//A/D SEN_R 	AN002を使用
	MPC.P41PFS.BIT.ASEL=1;	//A/D SEN_FR	AN001を使用
	MPC.P40PFS.BIT.ASEL=1;	//A/D SEN_R 	AN000を使用
	MPC.PE1PFS.BIT.ASEL=1;	//A/D V_BAT		AN009を使用
	PmnPFS_prohibit();	//プロテクト設定

}

void init_maze(void)
{
	int i,j;
	
	for(i = 0; i < MAZESIZE_X; i++)
	{
		for(j = 0; j < MAZESIZE_Y; j++)
		{
			wall[i][j].north = wall[i][j].east = wall[i][j].south = wall[i][j].west = UNKNOWN;	//迷路の全体がわからない事を設定する
		}
	}
	
	for(i = 0; i < MAZESIZE_X; i++)
	{
		wall[i][0].south = WALL;		//四方の壁を追加する(南)
		wall[i][MAZESIZE_Y-1].north = WALL;	//四方の壁を追加する(北)
	}
	
	for(j = 0; j < MAZESIZE_Y; j++)
	{
		wall[0][j].west = WALL;			//四方の壁を追加する(西)
		wall[MAZESIZE_X-1][j].east = WALL;	//四方の壁を追加する(東)
	}
	
	wall[0][0].east = wall[1][0].west = WALL;	//スタート地点の右の壁を追加する
	
}

void gyro_get_ref(void){
	long i = 0;
	float gyro_ref_temp = 0;
	gyro_ref = 0;
	//ジャイロのリファレンス取得
	for(i = 0; i < 2500; i++){
		gyro_ref_temp += (float)gyro_x_new;
		wait_ms(1);
	}
	gyro_ref = (gyro_ref_temp/2500.0);
	degree = 0;
	wait_ms(100);
}

void init_parameters(void)
{
			
	sen_r.ref = REF_SEN_R;				//右センサのリファレンス値を初期化
	sen_l.ref = REF_SEN_L;				//左センサのリファレンス値を初期化
	
	sen_r.th_wall = TH_SEN_R;			//右センサの壁有無判断の閾値を初期化
	sen_l.th_wall = TH_SEN_L;			//左センサの壁有無判断の閾値を初期化
	
	sen_fr.th_wall = TH_SEN_FR;			//右前センサの壁有無判断の閾値を初期化
	sen_fl.th_wall = TH_SEN_FL;			//左前センサの壁有無判断の閾値を初期化
	
	sen_r.th_control = CONTH_SEN_R;			//右センサの壁制御かけるか否かの閾値を初期化
	sen_l.th_control = CONTH_SEN_L;			//左センサの壁制御かけるか否かの閾値を初期化
	
	con_wall.kp = CON_WALL_KP/10000.0;			//壁比例制御の比例定数を初期化
}