#include "i2c.h"
#include "../../iodefine.h"
#include "../Definations/system_definations.h"
/*--------------
レジスタの説明
ICCR1 : i2cバスコントロールレジスタ(p.1487)
--IICRST : 1でi2c内部リセット
--ICE : i2c許可(1),i2c禁止(0)

ICCR2 : I2Cバスコントロールレジスタ
--i2cコンディション書き込み用

ICSR2 : i2cバスステータスレジスタ(p.1507)
--ステータス検出用
--書き込み時 0を書き込むことでクリアできる
--読み取り時 1:フラグ検出

ICIER : i2cバスインタラプトイネーブルレジスタ(p.1502)
--各種設定をすることで、i2cインタラプトを許可する事が出来る

ICDRR : I2Cバス受信データレジスタ 
--受信データが格納される
ICDRT : I2Cバス送信データレジスタ
--送信データを格納する

ICMR3 : 各種モードレジスタ,全てwrite (p.1496)
--ACKBR : 受信アクノリッジビット
--ACKBT : 送信アクノリッジビット
--ACKBP : ACKBTライトプロテクトビット
--WAIT : 1で9クロック目と1クロック目の間をLowにホールド,1byteごとの動作を可能にする。
データを受信したい場合は、ICDRRレジスタを逐一読む必要がある

[アドレス一致検出機能] manual p.1538~
ICSER : スレーブアドレスステータスレジスタ(p.1500)
--SARyEビットを1にすると、SARL/Uレジスタで設定したスレーブアドレスを比較できる
--ICRXI,ICTXI割込みが発生できる
SARL/U : スレーブアドレス設定レジスタ(p.1511,2)
ICSR1 : I2Cバスステータスレジスタ1(p.1504)
--SARL/Uで設定したスレーブアドレス検出フラグ(AASy)

[周波数と転送速度の設定(p.1515)]
ICMR1,bit0~2,CKS & ICBRL & ICBRH の3つの要素を組み合わせて転送速度を決定
ICMR1 : bitcounter=9 PCLK/4 400kbps -> 28, 100kbps -> 38
ICBRL : 転送速度=1/{[ICBRH+1)+(ICBRL+1)]/IICφ}	400kbps -> F0, 100kbps -> FD
ICBRH : IICφ=PCLKx10^6×分周比 / Tr=0の場合は400kHz	400kbps -> E7, 100kbps -> F8

※今回の設定
PCLK~=50なので、表36-5,400kbpsの欄を参考にする
----------------*/

unsigned char IOex_SWITCH(void)
{
	unsigned char who;
	I2C_START();
	I2C_PUT((0x18) << 1); //アドレス write
	I2C_PUT(0x00);		  //PWR_MGMT_2 INPUT
	I2C_RESTART();
	I2C_PUT((0x18 << 1) | 0x01); //アドレス read

	while (RIIC2.ICSR2.BIT.RDRF == 0)	//受信データがあるまで待機
		;
	if (RIIC2.ICSR2.BIT.NACKF)	//NACKが検出されたらストップコンディション検出フラグをリセット
	{
		RIIC2.ICSR2.BIT.STOP = 0;
	}

	RIIC2.ICMR3.BIT.WAIT = 1;	//ICDRRが読みだされるまでCLKをlowにホールド（1byteごとの読み出し動作を可能にする）
	RIIC2.ICMR3.BIT.ACKBT = 1;	//マスターからNACKを送信
	RIIC2.ICDRR;	//dummy

	while (RIIC2.ICSR2.BIT.RDRF == 0)
		;
	who = RIIC2.ICDRR;	//データの読み出し

	while (RIIC2.ICSR2.BIT.RDRF == 0)
		;
	RIIC2.ICSR2.BIT.STOP = 0;	//フラグをクリア
	RIIC2.ICCR2.BIT.SP = 1;		//ストップコンディションを発行
	RIIC2.ICDRR; // dummy
	RIIC2.ICMR3.BIT.WAIT = 0;

	while (RIIC2.ICSR2.BIT.STOP == 0)
		;
	RIIC2.ICSR2.BIT.NACKF = 0;	//フラグを初期化
	RIIC2.ICSR2.BIT.STOP = 0;

	return who;
}

void IOex_LED(unsigned char led_num)
{
	//ポーリングによる設定
	I2C_START();			 // Startbit発生
	I2C_PUT((0x18 << 1));	 //アドレス write
	I2C_PUT(0x01);			 // PWR_MGMT_2 OUTOUT
	I2C_PUT(0x00 | led_num); // sleep解除
	I2C_STOP();
}

void IOex_init(void)
{
	I2C_START();		  // Startbit発生
	I2C_PUT((0x18 << 1)); //アドレス write
	I2C_PUT(0x03);		  // PWR_MGMT_2 init
	I2C_PUT(0xF0);		  // sleep解除
	I2C_STOP();
}

void I2C_init(void)
{

	// I2Cのイニシャライズ
	PORT1.PCR.BIT.B6 = 0; //入力プルアップ無効
	PORT1.PDR.BIT.B6 = 0; // Input
	PORT1.PCR.BIT.B7 = 0; //入力プルアップ無効
	PORT1.PDR.BIT.B7 = 0; // Input

	// // I2Cピンのモードを初期化
	// PORT1.PMR.BIT.B6 = 0; // SCL 汎用ポート
	// PORT1.PMR.BIT.B7 = 0; // SDA 汎用ポート
	// MPC.PWPR.BIT.B0WI = 0;
	// MPC.PWPR.BIT.PFSWE = 1;
	// MPC.P16PFS.BIT.PSEL = 0; //汎用ポート
	// MPC.P17PFS.BIT.PSEL = 0; //汎用ポート
	// MPC.PWPR.BIT.PFSWE = 0x80;

	
	// I2C用のピンとして設定
	PmnPFS_allow();
	MPC.P16PFS.BIT.PSEL = 0x0F; // SCL2-DS
	MPC.P17PFS.BIT.PSEL = 0x0F; // SDA2-DS
	PmnPFS_prohibit();

	//汎用ポートから機能ポートへ切り替え
	PORT1.PMR.BIT.B6 = 1;
	PORT1.PMR.BIT.B7 = 1;

	//システムプロテクトを解除 i2cを有効化
	system_protect_allow();
	MSTP(RIIC2) = 0;
	system_protect_prohibit();

	// RIIC2全レジスタをリセット IICRST=1 & ICE=0
	RIIC2.ICCR1.BIT.ICE = 0;	// SCLn SDAn端子非駆動状態
	RIIC2.ICCR1.BIT.IICRST = 1; // IICRST、設定用全レジスタをリセット
	
	//パワーオン時の暴走対策で割り込み禁止に設定
	IEN(RIIC2, EEI2) = 0; //割込みベクタを禁止
	IEN(RIIC2, RXI2) = 0;
	IEN(RIIC2, TXI2) = 0;
	IEN(RIIC2, TEI2) = 0;
	IPR(RIIC2, EEI2) = 10; //割り込み順位
	IPR(RIIC2, RXI2) = 10;
	IPR(RIIC2, TXI2) = 10;
	IPR(RIIC2, TEI2) = 10;
	IR(RIIC2, EEI2) = 0;
	IR(RIIC2, RXI2) = 0; //ステータスフラグをclear
	IR(RIIC2, TXI2) = 0;
	IR(RIIC2, TEI2) = 0;
	
	RIIC2.ICSER.BYTE = 0x00;	// スレーブアドレス一致検出機能を無効化(SARL,SARU)

	//周波数と転送速度の設定 転送速度:400kbps
	RIIC2.ICMR1.BYTE = 0x28; 
	RIIC2.ICBRL.BYTE = 0xF0; 
	RIIC2.ICBRH.BYTE = 0xE7; 

	//その他様々な設定 タイムアウト機能は使わない（後々...）
	RIIC2.ICMR2.BYTE = 0x00;
	RIIC2.ICMR3.BYTE = 0x00;
	RIIC2.ICMR3.BIT.ACKWP = 1; // ACKBT(アクノリッジ)の書き込み許可
	RIIC2.ICFER.BYTE = 0x72;

	//i2cの実行設定
	RIIC2.ICIER.BYTE = 0x00; 	//全割込みを禁止
	//	RIIC2.ICIER.BYTE =0xfc;//送信データエンプティ、送信終了割り込み 受信データフル割り込み、NACK受信割り込み、ストップコンディション検出割り込み、スタートコンディション検出割り込み
	RIIC2.ICCR1.BIT.IICRST = 0;	//i2cリセット状態を解除
	RIIC2.ICCR1.BIT.ICE = 0; // ICE 内部I2C転送動作禁止
}

void I2C_START(void)
{
	RIIC2.ICCR1.BIT.ICE = 1; // ICE 内部I2C転送動作許可
	while (RIIC2.ICCR2.BIT.BBSY == 1) // 1:バスビジー 0:バスフリー パスがフリーになるまで待つ
		;
	RIIC2.ICSR2.BIT.START = 0; // STARTコンディション検出フラグをクリア
	RIIC2.ICCR2.BIT.ST = 1;	   //スタートコンディション発行
}

void I2C_RESTART(void)
{
	RIIC2.ICCR1.BIT.ICE = 1; // ICE 内部I2C転送動作許可
	while (RIIC2.ICSR2.BIT.TDRE == 0)
		; // ICDRRレジスタに送信データが無くなるまで待機
	while (RIIC2.ICSR2.BIT.TEND == 0)
		; //データの送信が終了するまで待機
	
	RIIC2.ICSR2.BIT.START = 0; // STARTコンディション検出フラグをクリア
	RIIC2.ICCR2.BIT.RS = 1;	   //リスタートコンディションの発行
	while (RIIC2.ICCR2.BIT.RS)
		; //リスタートコンディションが発行されるまで待機
}

void I2C_STOP(void)
{
	while (RIIC2.ICSR2.BIT.TDRE == 0)
		; // ICDRRレジスタに送信データが無くなるまで待機
	while (RIIC2.ICSR2.BIT.TEND == 0)
		; //データの送信が終了するまで待機

	RIIC2.ICSR2.BIT.STOP = 0;	//検出フラグをクリア
	RIIC2.ICCR2.BIT.SP = 1; //ストップコンディションの発行

	while (RIIC2.ICSR2.BIT.STOP == 0)
		;	//ストップコンディションが発行されるまで待機

	RIIC2.ICSR2.BIT.NACKF = 0; //コンディション検出用レジスタをクリア
	RIIC2.ICSR2.BIT.STOP = 0;
	RIIC2.ICCR1.BIT.ICE = 0; // ICE 内部I2C転送動作禁止
}

void I2C_PUT(unsigned char aaa)
{
	if (RIIC2.ICSR2.BIT.NACKF == 1)	// NACK検出ならstopコンディションを未検出にする
	{
		RIIC2.ICSR2.BIT.STOP = 0;
	}
	while (RIIC2.ICSR2.BIT.TDRE == 0)	// ICDRRレジスタに送信データが無くなるまで待機
		;
	RIIC2.ICDRT = aaa;
}

#ifdef IIC_INTERRUPT
// ===================================割り込み===================================
// 通信エラー/イベント発生による割り込み
// AL    AL=1 & ALIE=1
// NACKF NACKF=1 & NAKIE=1
// TMOF  TMOF=1 & TMOIE=1
// START START=1 & STIE=1
// STOP  STOP=1 & SPIE=1
void int_i2c_ee(void)
{
	/* Check stop condition detection */
	if ((RIIC2.ICSR2.BIT.STOP != 0) && (RIIC2.ICIER.BIT.SPIE != 0))
	{
		/* Clear each status flag */
		RIIC2.ICSR2.BIT.NACKF = 0;
		RIIC2.ICSR2.BIT.STOP = 0;
		/* Enable/Disable each interrupt */
		RIIC2.ICIER.BYTE = 0xBB; // TEIEとSTIEの割り込みを禁止

		/* Initialize ram for RIIC */
		iic_buff.trm_cnt = 0; /* Clear the internal transmission counter for IIC */
		iic_buff.rcv_cnt = 0; /* Clear the internal reception counter for IIC */
	}

	/* Check NACK reception */
	if ((RIIC2.ICSR2.BIT.NACKF != 0) && (RIIC2.ICIER.BIT.NAKIE != 0))
	{
		RIIC2.ICIER.BIT.NAKIE = 0;
		/* Generate Stop Condition */
		RIIC2.ICSR2.BIT.STOP = 0;
		RIIC2.ICCR2.BIT.SP = 1;
	}

	/* Check start condition detection restart*/
	if ((RIIC2.ICSR2.BIT.START != 0) && (RIIC2.ICIER.BIT.STIE != 0))
	{
		/* Disable Start Condition Detection Interrupt */
		RIIC2.ICSR2.BIT.START = 0;
		RIIC2.ICIER.BIT.STIE = 0;
		RIIC2.ICIER.BIT.TIE = 1;
		/* Transfer slave device address */
		if ((iic_buff.trm_cnt == 0) && (iic_buff.rcv_cnt == 0))
		{										  //最初のスタートコンディション
			RIIC2.ICDRT = (iic_buff.SlvAdr << 1); /* When master transfer, b0 must be '1' */
		}
		else
		{
			RIIC2.ICDRT = (iic_buff.SlvAdr << 1) | 0x01; /* When master transfer, b0 must be '1' */
		}
	}
}

void int_i2c_rx(void)
{
	volatile unsigned char tmp;

	/* Increase internal reception counter on RAM. It is a receive data counter. */
	iic_buff.rcv_cnt++;

	if (iic_buff.rcv_cnt == iic_buff.RWCnt)
	{
		RIIC2.ICMR3.BIT.WAIT = 1;
		RIIC2.ICMR3.BIT.ACKBT = 1;
	}

	if (iic_buff.rcv_cnt == 1)
	{
		/* dummy read */
		tmp = RIIC2.ICDRR;
	}
	else if (iic_buff.rcv_cnt == (iic_buff.RWCnt + 1))
	{ //データ受信おわり
		if (iic_buff.SlvAdr == 0x08)
		{
			if (iic_buff.RA == 0x04)
			{
				iic_data.Touch_Data = RIIC2.ICDRR;
			}
			else if (iic_buff.RA == 0x02)
			{
				iic_data.TCNT.BYTE.TCNT_H = RIIC2.ICDRR;
			}
			else if (iic_buff.RA == 0x03)
			{
				iic_data.TCNT.BYTE.TCNT_L = RIIC2.ICDRR;
			}
		}
		RIIC2.ICSR2.BIT.STOP = 0;
		RIIC2.ICCR2.BIT.SP = 1;
		RIIC2.ICMR3.BIT.WAIT = 0;
	}
	else
	{
		if (iic_buff.SlvAdr == 0x08)
		{
			iic_data.TCNT.BYTE.TCNT_H = RIIC2.ICDRR;
		}
	}

	//
	//	}else if(iic_buff.RWCnt == 2){
	//		RIIC2.ICMR3.BIT.WAIT = 1;
	//		RIIC2.ICMR3.BIT.ACKBT = 1;
	//		*iic_buff.pRWData++ = RIIC2.ICDRR;
	//		iic_buff.RWCnt--;
	//	}else if(iic_buff.RWCnt == 1){
	//		RIIC2.ICSR2.BIT.STOP = 0;
	//		RIIC2.ICCR2.BIT.SP = 1;
	//		*iic_buff.pRWData++ = RIIC2.ICDRR;	/* Read final data */
	//		iic_buff.RWCnt--;
	//		RIIC2.ICMR3.BIT.WAIT = 0;
	//		tmp2=(I2C_BUF[1]<<8) & 0xff00;
	//		tmp3=I2C_BUF[0] & 0x00ff;
	//		if(GYRO_ACCE_F==0){
	//			GYRO16=( tmp2 | tmp3) >> 4;
	//		}else{
	//			ACCE16=( tmp2 | tmp3) >> 4;
	//		}
	//
	//	}else{
	//		*iic_buff.pRWData++ = RIIC0.ICDRR;
	//		iic_buff.RWCnt--;
	//	}
}

//送信データ エンプティ
// TDRE=1 & TIE=1
void int_i2c_tx(void)
{
	/* Increase internal transmission counter */
	iic_buff.trm_cnt++;
	if (iic_buff.trm_cnt == 1)
	{
		/* Transfer slave device register address */
		RIIC2.ICDRT = iic_buff.RA;
		if (iic_buff.RW == 1)
		{							  // read のためリスタートの準備
			RIIC2.ICIER.BIT.TEIE = 1; /* Enable Transmit End Interrupt */
		}
	}
	else if (iic_buff.trm_cnt == 2)
	{
		if (iic_buff.RW == 0)
		{
			RIIC2.ICDRT = iic_buff.write_buf[0];
		}
		//		if(GYRO_ACCE_F==0){
		//			RIIC2.ICDRT = 0xac;//out_z_L ??read 最上位アドレスが1の場合、アドレスがauto incr
		//		}else{
		//			RIIC2.ICDRT = 0xaa;//out_y_L ??read
		//		}
	}
	else if (iic_buff.trm_cnt == (iic_buff.RWCnt + 2))
	{ //書き込みデータ終わり
		if (iic_buff.RW == 0)
		{
			RIIC2.ICIER.BIT.TEIE = 1; /* Enable Transmit End Interrupt */
		}
	}
}

void int_i2c_te(void)
{
	if (iic_buff.RW == 0)
	{ // writeのみ
		RIIC2.ICIER.BIT.SPIE = 1;
		RIIC2.ICSR2.BIT.STOP = 0; // STOPコンディション発光
		RIIC2.ICCR2.BIT.SP = 1;
	}
	else
	{							   // read
		RIIC2.ICSR2.BIT.START = 0; /* Clear Start Condition Detection flag */
		RIIC2.ICIER.BIT.STIE = 1;  /* Enable Start Condition Detection Interrupt */
		RIIC2.ICIER.BIT.TEIE = 0;  /* Disable Transmit End Interrupt */
		/* Generate restart condition */
		RIIC2.ICCR2.BIT.RS = 1;
	}
}

#endif
