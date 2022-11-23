#ifndef INTERRUPT_H_
#define INTERRUPT_H_

/*----------------------------------
<CMTについての説明>
CMT0は制御割り込み用タイマ
CMT1はAD変換でのセンサ読み込み用タイマ
CMT2はエンコーダ関連 値の取得/速度更新/距離積分など用タイマ
CMT3は遅延ルーチン用タイマ

CMT1の分周は128 : 0.375MHZ
0~65535までの値(16bit)でカウントされる
CMCORがmaxのとき：174.76ms / cycle
------------------------------------*/

// //CMT0を開始 / 停止
// void CMT0_START(float interrupt_intv_ms, void (*handler)(void));
// void CMT0_STOP(void);

void timer_start(void);
void timer_stop(void);

void int_cmt0(void);
void int_cmt1(void);
void int_cmt2(void);
#endif