#ifndef SCI_H_
#define SCI_H_

/*-----------------------------
[Tera Term の設定]
speed:115200bps(任意)       ☆サンプルプログラムでは38400
改行コード R:auto, M:LF
漢字-受信/送信 : SJIS
ロケール : Japanese
[接続設定]
mode:シリアル / port:COM14
------------------------------*/

#define BUFF_SIZE 512

#define ESC_KEY     0x1b 
#define ENTER_KEY   0x0a

//SCIの初期化
void init_sci(long baud);

//☆SCI_printfでは割込みを使っているが、この割込みが予期せぬトラブルを生む可能性があるため、printfの後に必ずsend_checkを入れる
int SCI_printf(const char *format, ...);
void SCI_gets( volatile unsigned char *p);
//画面クリア用関数
void SCI_clear(void);

//送信中割込み
void int_sci1_txi(void);
//送信完了割込み
void int_sci_tei(void);
void int_scr1_rxi(void);
int int_scr1_eri(void);

//SCIが送信中であれば１,送信中で無ければ０を返す
int SCI_send_check(void);
void sci_wait_send_check(void);
#endif
