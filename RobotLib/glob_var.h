//グローバル変数を一箇所で宣言する。
//glob_var.cで実体を作り(_GLOB_VARをdefine)、他のグローバル変数を使用するファイルからはglob_var.hをincludeする。

#include "Parameters/static_parameters.h"
#include "Definations/original_typedef.h"

//global変数を一括で実体とextern宣言する。
//include元で_GLOB_VARが定義されていれば実体宣言、定義されていなければ外部宣言となる。
#ifdef _GLOB_VAR
#define GLOBAL
#else
#define GLOBAL extern
#endif

//構造体系のグローバル変数
GLOBAL t_sensor		sen_r, sen_l, sen_fr, sen_fl;		//センサ構造体
GLOBAL t_control		con_wall;				//制御構造体
GLOBAL t_position		mypos;					//自己座標
GLOBAL t_wall			wall[MAZESIZE_X][MAZESIZE_Y];		//壁の情報を格納する構造体配列
GLOBAL unsigned char	map[MAZESIZE_X][MAZESIZE_Y];		//歩数マップ

//走行系のグローバル変数
GLOBAL int run_mode;							//真っ直ぐか回転かの走行モード（制御系の切り替えに使用）

//車体速度系のグローバル変数
GLOBAL float			fast_speed;				//最短走行時の最高速度	[m/s]
GLOBAL float			tar_ang_vel;			//目標角速度		[rad/s]
GLOBAL float			tar_degree;				//☆未使用 目標角度(移動角度の目標値)		[deg]
GLOBAL float			max_degree;				//旋回時の最大角度	[deg]
GLOBAL float			start_degree;			//走行進入時の車体角度	[deg]
GLOBAL float			ang_vel;				//現在角速度		[rad/s]
GLOBAL float			p_ang_vel;				//過去角速度		[rad/s]
GLOBAL float			max_ang_vel;			//最高角速度		[rad/s]
GLOBAL float			ang_acc;				//角加速度		[rad/ss]
GLOBAL float			accel;					//加速度		[m/ss]
GLOBAL float			max_speed;				//最高速度		[m/s]
GLOBAL float			speed_r;				//現在の右タイヤ速度	[m/s]
GLOBAL float			speed_l;				//現在の左タイヤ速度	[m/s]
GLOBAL float			speed_old_r;			//右タイヤの過去の速度	[m/s]
GLOBAL float			speed_new_r;			//右タイヤの最新の速度	[m/s]
GLOBAL float			speed_old_l;			//左タイヤの過去の速度	[m/s]
GLOBAL float			speed_new_l;			//左タイヤの最新の速度	[m/s]				
GLOBAL float			speed;					//現在車体速度		[m/s]
GLOBAL float			p_speed;				//過去の車体速度	[m/s]
GLOBAL float			tar_speed;				//目標車体速度		[m/s]
GLOBAL float			end_speed;				//終端車体速度		[m/s]
GLOBAL float			V_r;					//右モータの出力電圧	[V]
GLOBAL float			V_l;					//左モータの出力電圧	[V]

//エンコーダ角度系のグローバル変数
GLOBAL unsigned int			angle;					//☆未使用 現在の車軸角度	[deg]
GLOBAL unsigned int			locate_l;				//現在の車軸位置 エンコーダの値	[無次元]
GLOBAL unsigned int			locate_r;				//現在の車軸位置 エンコーダの値	[無次元]
GLOBAL unsigned int			before_locate_r;		//過去の車軸位置 エンコーダの値	[無次元]
GLOBAL unsigned int			before_locate_l;		//過去の車軸位置 エンコーダの値	[無次元]
GLOBAL int			        diff_pulse_r;			//車軸位置の微分値(車軸の回転速度[pulse/ms])
GLOBAL int			        diff_pulse_l;			//車軸位置の微分値(車軸の回転速度[pulse/ms])
				
//タイマ系グローバル変数
GLOBAL unsigned int		timer;					//☆未使用 @cmt0 1msごとにカウントアップされる変数. wait_ms用の変数だった

//電圧監視用グローバル変数
GLOBAL long 			cnt;					//☆未使用 @cmt0 割り込み回数カウント変数
GLOBAL float			V_bat;					//電源電圧[V]

//ジャイロ系のグローバル変数
GLOBAL float			gyro_x;					//ヨー軸ジャイロの現在の値	[無次元]
GLOBAL float			gyro_x_new;				//ヨー軸ジャイロの最新の値	[無次元]
GLOBAL float			gyro_ref;				//ヨー軸ジャイロのリファレンス値[無次元]
GLOBAL float			degree;					//現在の車体角度 積算の絶対値		[degree]

//車体移動距離系のグローバル変数
GLOBAL float			len_mouse;				//マウスの移動距離		[mm]
GLOBAL float			len_target;				//マウスの目標移動距離		[mm]

//フラグ系のグローバル変数
GLOBAL char			TURN_DIR;				//ターン方向フラグ

//モータのデューティ決定用グローバル変数
GLOBAL float			Duty_r;					//出力のデューティ比				[%]
GLOBAL float			Duty_l;					//出力のデューティ比				[%]

//I制御用グローバル変数
GLOBAL float			I_tar_speed;			//目標速度のI成分
GLOBAL float			I_speed;				//実速度のI成分
GLOBAL float			I_tar_ang_vel;			//目標角速度のI成分
GLOBAL float			I_ang_vel;				//実角速度のI成分