/***********************************************************************/
/*                                                                     */
/*  FILE        :spi.c			                               */
/*  DATE        :Tue, Jun 08, 2017                                     */
/*  DESCRIPTION :SPI Program                                           */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
#include "../iodefine.h"
#include "mathf.h"
#include "Hardwares/spi.h"
#include "Parameters/adjust_parameters.h"
#include "Parameters/static_parameters.h"
#include "Definations/system_definations.h"
#include "glob_var.h"

extern wait_ms(int miribyou);

void straight(float len, float acc, float max_sp, float end_sp){
	//積分要素の初期化
	I_tar_ang_vel = 0;
	I_ang_vel = 0;
	I_tar_speed = 0;
	I_speed = 0;
	//走行モードを直線にする
	run_mode = STRAIGHT_MODE;
	//壁制御を有効にする
	con_wall.enable = true;
	//目標距離をグローバル変数に代入する
	len_target = len;
	//目標速度を設定
	end_speed = end_sp;
	//加速度を設定
	accel = acc;
	//最高速度を設定
	max_speed = max_sp;
	
	//モータ出力をON
	MOTOR_EN_OUT = 1;	
	
	if(end_speed == 0){	//最終的に停止する場合
		//減速処理を始めるべき位置まで加速、定速区間を続行
		//[-10]は誤作動防止（詳しくはグラフを書いて）
		while( ((len_target -10) - len_mouse) >  1000.0*((float)(tar_speed * tar_speed) - (float)(end_speed * end_speed))/(float)(2.0*accel));
		//減速処理開始
		accel = -acc;					//減速するために加速度を負の値にする	
		while(len_mouse < len_target -1){		//停止したい距離の少し手前まで継続
			//一定速度まで減速したら最低駆動トルクで走行
			if(tar_speed <= MIN_SPEED){	//目標速度が最低速度になったら、加速度を0にする
				accel = 0;
				tar_speed = MIN_SPEED;	//低速運動
			}
		}
		accel = 0;
		tar_speed = 0;	//目標速度を０に
		//速度が0以下になるまで逆転する
		while(speed >= 0.0);
			
	}else{
		//減速処理を始めるべき位置まで加速、定速区間を続行
		while( ((len_target-10) - len_mouse) >  1000.0*((float)(tar_speed * tar_speed) - (float)(end_speed * end_speed))/(float)(2.0*accel));
		
		//減速処理開始
		accel = -acc;					//減速するために加速度を負の値にする	
		while(len_mouse < len_target){		//停止したい距離の少し手前まで継続
			//一定速度まで減速したら最低駆動トルクで走行
			if(tar_speed <= end_speed){	//目標速度が最低速度になったら、加速度を0にする
				accel = 0;
				tar_speed = end_speed; 	//低速運動
			}
		}
	}
	//加速度を0にする
	accel = 0;
	//現在距離を0にリセット
	len_mouse = 0;
}

void turn(int deg, float ang_accel, float max_ang_velocity, short dir){
	float start_degree = 0;
	
	wait_ms(WAIT_TIME);
	//積分要素の初期化
	I_tar_ang_vel = 0;
	I_ang_vel = 0;
	I_tar_speed = 0;
	I_speed = 0;

	//車体の速度は一定、加速度０で運動する
	tar_speed = 0;
	accel = 0;
	//目標パラメータの初期化
	tar_degree = 0;	//☆未使用 移動角度の目標値
	tar_ang_vel = 0;
	//走行モードを超進地旋回モードにする
	run_mode = TURN_MODE;

	//回転方向定義
	TURN_DIR = dir;	
	
	//車体の現在角度を取得
	start_degree = degree;
	
	//角加速度、加速度、最高角速度設定
	MOTOR_EN_OUT = 1;	//モーターのsleepを解除
	if(dir == LEFT){
		ang_acc = ang_accel;			//角加速度を設定
		max_ang_vel = max_ang_velocity;
		max_degree = deg;
		while( (max_degree - (degree - start_degree))*PI/180.0 > (tar_ang_vel*tar_ang_vel/(2.0 * ang_acc)));
		
	}else if(dir == RIGHT){
		ang_acc = -ang_accel;			//角加速度を設定
		max_ang_vel = -max_ang_velocity;
		max_degree = -deg;
		//両辺にマイナスを掛けることで、疑似的にdir==LEFTの場合と同じようにして処理
		while(-(float)(max_degree - (degree - start_degree))*PI/180.0 > (float)(tar_ang_vel*tar_ang_vel/(float)(2.0 * -ang_acc)));
	}

	//角減速区間に入るため、角加速度設定
	MOTOR_EN_OUT = 1;	//モーターのsleepを解除
	if(dir == LEFT){
		ang_acc = -ang_accel;			//角加速度を設定
		//減速区間走行
		while((degree - start_degree) < max_degree){
			if(tar_ang_vel < TURN_MIN_SPEED){
				ang_acc = 0;
				tar_ang_vel = TURN_MIN_SPEED;
			}
		}
		
		ang_acc = 0;
		tar_ang_vel = 0;
		tar_degree = max_degree;
		
	}else if(dir == RIGHT){
		ang_acc = +ang_accel;			//角加速度を設定
		//減速区間走行
		while((degree - start_degree) > max_degree){
			if(-tar_ang_vel < TURN_MIN_SPEED){
				ang_acc = 0;
				tar_ang_vel = -TURN_MIN_SPEED;
			}
		}
		ang_acc = 0;
		tar_ang_vel = 0;	//目標角速度を０に
		tar_degree = max_degree;


	}
	//角速度が小さくなるまで逆転
	while(ang_vel >= 0.05 || ang_vel <= -0.05 );
	
	tar_ang_vel = 0;	//目標角速度を一旦初期化(壁制御の時に変動)
	ang_acc = 0;
	//現在距離を0にリセット
	len_mouse = 0;
	wait_ms(WAIT_TIME);
}
