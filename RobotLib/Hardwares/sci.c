/*☆TXI,TEI割込みについてもう少し詳しく書きたい*/
#include "sci.h"
#include <machine.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../iodefine.h"
#include "../Definations/system_definations.h"

#pragma inline(SCI_send_check)

static char sci1_buff[BUFF_SIZE];	//TX用の文字列を格納するための配列。配列のサイズがBUFF_SIZEを超えるとバッファオーバーのリスクがある
static int sci1_length;		//sci1_buffの長さを格納
static int sci1_point;		//要素数カウンタ

unsigned char get_buff;
static unsigned char get_buff_m[BUFF_SIZE];
static char get_buff_2[BUFF_SIZE];

static int receve_data_num, string_num;

// #define SCI_ERROR
#ifdef SCI_ERROR
void sci_error(void){

}
#endif

void init_sci(long baud){

	system_protect_allow();	//プロテクト解除
    MSTP(SCI1) = 0;
	system_protect_prohibit();

	SCI1.SCR.BYTE = 0x00;
	while (0x00 != (SCI1.SCR.BYTE & 0xF0));	//割り込み要求が禁止されるまで待つ
	PORT2.PODR.BIT.B6 = 1;			//TXDのDirctionの切り替え後の値をhigh

	//PORT26をTX , PORT30をRXに設定
	PORT2.PDR.BIT.B6 = 1;			//出力に設定
	PORT3.PDR.BIT.B0 = 0;			//入力に設定
	PORT2.PMR.BIT.B6 = 0;			//汎用ポートに設定
	PORT3.PMR.BIT.B0 = 0;			//汎用ポートに設定

	PmnPFS_allow();
	MPC.P26PFS.BIT.PSEL = 0x0A;		//TXD1
	MPC.P30PFS.BIT.PSEL = 0x0A;		//RXD1
	PmnPFS_prohibit();

	PORT2.PMR.BIT.B6 = 1;	//TXD1 機能ポートに設定
	PORT3.PMR.BIT.B0 = 1;	//RXD1 機能ポートに設定

	//シリアルコミュニケーションインターフェイスモード SCMR.SMIF = 1
	SCI1.SCR.BIT.CKE = 0;
	SCI1.SMR.BYTE = 0x00;			//1stopbit parityなし 8bit 調歩同期
	SCI1.SCMR.BYTE = 0xF2;			//S=32clock
	SCI1.SEMR.BYTE = 0x10;			//8サイクル
	// SCI1.BRR = (int)(((long)48000000 / (32*1/2*baud)) - 1); //@48MHz 通信速度を計算する
	SCI1.BRR = 25;
	SCI1.SSR.BYTE &= 0xC7;			// ORER,PER,FERをクリアする

	//送受信割込みを設定、初期設定では、IENで割込みを禁止する。優先順位は最低
	SCI1.SCR.BYTE =	0x00;			//送受信での割り込み動作を禁止(割込みがオンになるわけではない)
	IEN(SCI1, TXI1) = 1;
	IEN(SCI1, TEI1) = 1;
	IPR(SCI1, TXI1) = 1;			//優先順位を最低にする
	IPR(SCI1, TEI1) = 1;
	IR(SCI1, TXI1) = 0;
	IR(SCI1, TEI1) = 0;

}

int SCI_printf(const char *format, ...){

	va_list arg;

	if(SCI1.SSR.BIT.TEND == 0) return 0;

	va_start(arg, format);
	sci1_length = vsprintf(sci1_buff, format, arg);
	va_end(arg);

	if(sci1_length < 1) return 0;
	sci1_point = 0;

	SCI1.SCR.BIT.TE = 1;	//シリアル送信動作許可
	SCI1.SCR.BIT.TIE = 1;	//TXI 割り込み要求を許可
	
	SCI1.TDR = sci1_buff[sci1_point++];
	return 1;
}


void SCI_gets( volatile unsigned char *p){

	volatile long n;
	
	SCI1.SCR.BIT.RE = 1;
	SCI1.SCR.BIT.RIE = 1;
	
    for( n=0 ;  (get_buff_2[n]!='\0')&&(n<=sizeof(get_buff_2)); n++){	//get_buff_2の値がNULLではない場合
        *(p+n) = get_buff_2[n];     //配列のエイリアスpに値を代入
    }
}

void SCI_clear(void){

	//画面クリアシーケンス
	SCI_printf("\x1b[2J");
	while(SCI_send_check());
	SCI_printf("\x1b[0;0H");
}

void int_sci1_txi(void){
	
	if (sci1_point == sci1_length)
	{
		SCI1.SCR.BIT.TIE = 0;
		SCI1.SCR.BIT.TEIE = 1;
	}else{
		while (SCI1.SSR.BIT.TEND == 0)	//dataの送信中はwait
			;
		SCI1.TDR = sci1_buff[sci1_point++];
	}
	
}

void int_sci_tei(void){
	
	SCI1.SCR.BIT.TE = 0;	//TX禁止
	SCI1.SCR.BIT.TIE = 0;	//TXI割込み禁止
	SCI1.SCR.BIT.TEIE = 0;	//TEI割込み禁止
}

//intprg.cで呼び出す
void int_scr1_rxi(void){

    int n;
    get_buff_m[receve_data_num++] = SCI1.RDR;

    if(receve_data_num >= sizeof(get_buff_m)-1 )
		receve_data_num=0;  // -1があるのは、文字列の最後に空文字(NULL)があるため。配列の添字＞配列の要素数になった場合
    
	switch(SCI1.RDR){
        case ESC_KEY:

            receve_data_num = 0;

            for( n=0; n<=sizeof(get_buff_m); n++ ){		//get_buff_mを初期化
                get_buff_m[n]='\0';
            }for( n=0; n<=sizeof(get_buff_2); n++ ){	//get_buff_2を初期化
                get_buff_2[n]='\0';
            }
            break;
        case ENTER_KEY:

            string_num = receve_data_num;
            receve_data_num = 0;

            for( n=0; n<=sizeof(get_buff_2); n++ ){	//get_buff_2を初期化
                get_buff_2[n]='\0';
            }
            for( n=0; n<string_num-1 ; n++ ){	//get_buff_mをget_buff_2にコピー
                get_buff_2[n]=get_buff_m[n];
            }
            get_buff_2[string_num]='\0';	//get_buff_2の末尾にNULLを追加
            for(n=0 ;n<sizeof(get_buff_m) ;n++){	//get_buff_mを初期化
                get_buff_m[n]='\0';
            }
            break;
		default :
			break;
    }
}

int int_scr1_eri(void){

    unsigned char er=1;
    if(SCI1.SSR.BIT.PER == 1){
        SCI1.SSR.BIT.PER=0;///パリティエラーフラグ
        er= 2;
    }
    if(SCI1.SSR.BIT.FER == 1){
        SCI1.SSR.BIT.FER=0;///フレームエラーフラグ
        er= 3;
    }
    if(SCI1.SSR.BIT.ORER == 1){
        SCI1.SSR.BIT.ORER=0;//オーバランエラーフラグ
        er = 4;
    }
    return er;
}

int SCI_send_check(void){
	//トランスミットエンドフラグ
	if (SCI1.SSR.BIT.TEND == 0)
	{
		return 1;	//データ送信中
	}else{
		return 0;	//データ送信完了
	}
}