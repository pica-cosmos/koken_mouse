#include "../iodefine.h"
#include "Parameters/static_parameters.h"

#define CMT3_CYCLE_PER_MS   375         //1msあたり何回カウントされるか 48(MHz)/PCLK(1/128)/1000(ms)
#define CMT3_CNT_MAX        65536       //カウントできる最大の数

volatile int cmt3_compa_times = 0;              // CMT1コンペアマッチ時のフラグ用

//遅延ルーチン 引数にミリ秒数を指定する
//CMT3 PCLK:1/128
void wait_ms(int miribyou){

    int interrupt_cycles = miribyou * CMT3_CYCLE_PER_MS;
    volatile int max_interrupt_times = interrupt_cycles / CMT3_CNT_MAX;
    int remained_interrupt_cycles = (interrupt_cycles - (CMT3_CNT_MAX * max_interrupt_times));
    
    //指定された回数分だけ、カウンタを最大値までカウントして、時間を稼ぐ
    cmt3_compa_times = 0;

    CMT.CMSTR1.BIT.STR3 = 0;    //カウント停止
    CMT3.CMCNT = 0;             //カウンタをクリア
    CMT3.CMCOR = CMT3_CNT_MAX -1;
    CMT3.CMCR.BIT.CMIE = 1;     //コンペアマッチ割り込み許可
    CMT.CMSTR1.BIT.STR3 = 1;    //割込み許可

    while(cmt3_compa_times < max_interrupt_times)
        ;

    //一度だけ、残ったサイクルの数だけカウントして、時間を稼ぐ
    cmt3_compa_times = 0;

    CMT.CMSTR1.BIT.STR3 = 0;    //カウント停止
    CMT3.CMCNT = 0;             //カウンタをクリア
    CMT3.CMCOR = remained_interrupt_cycles -1;
    CMT3.CMCR.BIT.CMIE = 1;     //コンペアマッチ割り込み許可
    CMT.CMSTR1.BIT.STR3 = 1;    //割込み許可

    while(cmt3_compa_times == 0)
        ;

    //割込み停止
    CMT.CMSTR1.BIT.STR3 = 0;    //カウント停止
    CMT3.CMCR.BIT.CMIE = 0;  //割込み禁止
    CMT3.CMCNT = 0;          //カウンタをクリア
}