#ifndef INIT_H_
#define INIT_H_

void init_clk(void);            //クロックの初期化
void init_CMT(void);            //CMTの初期化
void init_buzzer(void);         //ブザーの初期化 MTU2を使用
void init_motor(void);          //モーターの初期化
void init_wallsensor(void);     //壁センサ/バッテリーチェックの初期化
void init_maze(void);           //迷路情報の初期化
void gyro_get_ref(void);        //ジャイロのリファレンス取得

/*****************************************************************************************
光センサー系のパラメータ初期化
リファレンスとか壁の閾値とか
*****************************************************************************************/
void init_parameters(void);

void init_all(void);

#endif