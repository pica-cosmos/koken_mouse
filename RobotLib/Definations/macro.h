#ifndef MACRO_H_
#define MACRO_H_

#include "system_definations.h"

#define CONV_SEN2WALL(w) ((w) ? WALL : NOWALL)			//センサ情報から壁情報へ変換
#define VATT_CALC() (2.0*3.3*(float)(DATA_VOLTAGE/4095.0))

#endif