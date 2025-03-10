#ifndef INTERFACE_H_
#define INTERFACE_H_

#define DUTY_VOLUME_MAX	90
#define DUTY_VOLUME_MIN	10
#define FREQUENCY_MAX	10000
#define FREQUENCY_MIN	200
#define MTU_CNT_MAX 	65536

/*-------------------------------------------------------
各スイッチについて、押されているかどうかの有無を調べるマクロ
1:押されれている 0:押されていない
(詳しくはi2c.hを参照)
--------------------------------------------------------*/
#define SWITCH_2(sw2) ((sw2 & (1<<6)) >> 6) //スイッチ２の状態を調べる
#define SWITCH_3(sw3) ((sw3 & (1<<5)) >> 5) //スイッチ３の状態を調べる
#define SWITCH_4(sw4) ((sw4 & (1<<4)) >> 4) //スイッチ４の状態を調べる

//☆buzzerのdutyを小さくすると音量が大きくなる

/*-------------------------------
IOexのLEDを光らせる関数 16進数をled_numに代入
0b 	0  0  0  0
	D6 D5 D4 D3 
led_numに値を入れると、この順にLEDが点灯するので、値を読む
光らせたいビットを1にする
--------------------------------*/
void LED(unsigned char led_num);

/*---------------------------------
buzzer:PB5 / MTU2,MTIOC2A
PB5 1:low / 0:high
ブザーのオン/オフ 引数１にデューティー比, 引数２に周波数を設定
----------------------------------*/
void buzzer_on(int duty_percent, int circuit_hz);
void buzzer_off(void);

/*------------------------------------------------------------------------
引数１にデューティー比、引数２に周波数、引数３にブザーを鳴らす回数を指定
100ms間隔でブザーを鳴らす
------------------------------------------------------------------------*/
void BEEP(int duty_percent, int circuit_hz, int count);

/*----------------------------------------------
モード変更のプログラム。現在のモードを引数に代入
modeの最高値をmode_max, modeの最低値をmode_minに代入する
右のタイヤのスピードが正:mode++
右のタイヤのスピードが負:mode--
frequency [mode increase:1500hz,decrease:300hz]
-----------------------------------------------*/
void mode_change( char* mode, char mode_min, char mode_max);

/*----------------------------------------------
調整モードの、モード変更用プログラム mode : 1~7
mode 1 : パラメーターをUARTで表示
mode 2 : 一区画前進
mode 3 : 90度右に旋回
mode 4~7 : 空きモード

SW2を押すと、mode1から抜け出す[freq=1000,cnt=3]
SW3を押すと、調整モードから抜け出す[freq=1000,cnt=5]
-----------------------------------------------*/
void adjust_mode_select(void);

#endif