#ifndef RUN_H_
#define RUN_H_

/*---------------------------------------------------------------
直進用の関数。フォア処理。等加速度直線運動
len : 移動距離[mm], acc : 加速度[m/ss], max_sp : 最高速度[m/s], end_sp : 終端速度[m/s]
---------------------------------------------------------------*/
void straight(float len, float acc, float max_sp, float end_sp);

/*--------------------------------------------------------------
回転用の関数。フォア処理。車体の目標速度(speed)は一定。等角加速度回転運動
deg : 移動角度[degree], ang_acc : 角加速度[rad/ss], max_om : 最高角速度[rad/s], dir : 回転方向(0:right, 1:left)
---------------------------------------------------------------*/
void turn(int deg, float ang_acc, float max_om, short dir);

#endif
