#ifndef INIT_H_
#define INIT_H_

//クロックとCMTの初期化
void clk_init(void);
void CMT_init(void);

//ブザーの初期化 MTU2を使用
void buzzer_init(void);

void motor_init(void);

void init_adc(void);
void infrared_sensor_init(void);    //赤外線センサーの初期化

//迷路情報の初期化
void init_maze(void);

// ジャイロのリファレンス取得
void gyro_get_ref(void);

/*****************************************************************************************
光センサー系のパラメータ初期化
リファレンスとか壁の閾値とか
*****************************************************************************************/
void init_parameters(void);

void init_all(void);

#endif