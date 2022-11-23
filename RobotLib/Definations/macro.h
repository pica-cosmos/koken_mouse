#ifndef MACRO_H_
#define MACRO_H_

#include "system_definations.h"

#define CONV_SEN2WALL(w) ((w) ? WALL : NOWALL)			//センサ情報から壁情報へ変換
#define VATT_CALC() (2.0*3.3*(float)(DATA_VOLTAGE/4095.0))
/*-------------------------------------------------------
各スイッチについて、押されているかどうかの有無を調べるマクロ
1:押されれている 0:押されていない
(詳しくはi2c.hを参照)
--------------------------------------------------------*/

#define SWITCH_2(sw2) ((sw2 & (1<<6)) >> 6) //スイッチ２の状態を調べる
#define SWITCH_3(sw3) ((sw3 & (1<<5)) >> 5) //スイッチ３の状態を調べる
#define SWITCH_4(sw4) ((sw4 & (1<<4)) >> 4) //スイッチ４の状態を調べる

#endif