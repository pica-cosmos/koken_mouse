#include "interrupt.h"
#include "../iodefine.h"
#include <machine.h>
#include "Definations/original_typedef.h"
#include "Definations/macro.h"
#include "Definations/system_definations.h"
#include "Parameters/static_parameters.h"
#include "Parameters/adjust_parameters.h"
#include "Hardwares/spi.h"
#include "Hardwares/i2c.h"
#include "run.h"
#include "interface.h"
#include "glob_var.h"

// static const int cmt0_cycle_per_ms = 375;

// void (*cmt0_cmi0_interrupt_handler)(void); // CMI0の割込み用ハンドラ

// void CMT0_START(float interrupt_intv_ms, void (*handler)(void)){

//     cmt0_cmi0_interrupt_handler = handler; // CMI0の割り込みハンドラの設定

//     CMT.CMSTR0.BIT.STR0 = 0;
//     CMT0.CMCR.BIT.CMIE = 1;                                  //コンペアマッチ割り込み許可
//     CMT0.CMCNT = 0;                                          //カウンタをクリア
//     CMT0.CMCOR = (int)((float)cmt0_cycle_per_ms * interrupt_intv_ms) - 1; //割込み周期を計算
//     CMT.CMSTR0.BIT.STR0 = 1;
// }

// void CMT0_STOP(void){

//     CMT.CMSTR0.BIT.STR0 = 0; //カウンタ停止
//     CMT0.CMCR.BIT.CMIE = 0;  //割込み禁止
//     CMT0.CMCNT = 0;          //カウンタをクリア
// }

void timer_start(void){
	
	CMT.CMSTR0.BIT.STR0=1;	//カウントスタート
	CMT.CMSTR0.BIT.STR1=1;	//カウントスタート
	CMT.CMSTR1.BIT.STR2=1;	//カウントスタート
	CMT.CMSTR1.BIT.STR3=1;	//カウントスタート
}

void timer_stop(void){
	
	CMT.CMSTR0.BIT.STR0=0;	//カウント停止
	CMT.CMSTR0.BIT.STR1=0;	//カウント停止
	CMT.CMSTR1.BIT.STR2=0;	//カウント停止
	CMT.CMSTR1.BIT.STR3=0;	//カウント停止
	
}

//1kHz(1ms)
void int_cmt0(void)
{
	/*****************************************************************************************
	目標速度生成
	tar_speedの生成
	など
	*****************************************************************************************/
	//直線の場合の目標速度生成
	if(run_mode == STRAIGHT_MODE){
		tar_speed += accel/1000.0;	//目標速度を設定加速度で更新 1msごとに計測する
		//最高速度制限
		if(tar_speed > max_speed){
			tar_speed = max_speed;	//目標速度を設定最高速度に設定
		}
				
	
	}else if(run_mode == TURN_MODE){
		
		//車体中心速度更新
		tar_speed += accel/1000;	//1msごとに読むので1000で割る
		//最高速度制限
		if(tar_speed > max_speed){
			tar_speed = max_speed;	//目標速度を設定最高速度に設定
		}
		
		//角加速度更新
		tar_ang_vel += ang_acc/1000.0;	//目標角速度を設定加速度で更新
		tar_degree  += (tar_ang_vel*180.0/PI)/1000.0;	//1msごとに読むので1000で割る radをdegreeに変換する

		
		if(TURN_DIR == LEFT){
			//左回転の場合 : ang_vel, tar_degree : 正 ( a>b → aの方が角速度・移動角度は大きい)
			//最高角速度制限
			if(tar_ang_vel > max_ang_vel){
				tar_ang_vel = max_ang_vel;	//目標速度を設定最高速度に設定
			}
			if(tar_degree > max_degree){
				tar_degree = max_degree;
			}
		}else if(TURN_DIR == RIGHT){
			//右回転の場合 : ang_vel, tar_degree : 負 ( a<b → aの方が角速度・移動角度は大きい)
			//最高角速度制限
			if(tar_ang_vel < max_ang_vel){
				tar_ang_vel = max_ang_vel;	//目標速度を設定最高速度に設定
			}
			if(tar_degree < max_degree){
				tar_degree = max_degree;
			}
		}
		
	}else if(run_mode == NON_CON_MODE){
		//何もしない
		nop();	
	}else{
		//何もしない
		nop();	
	}
	
	/*****************************************************************************************
	壁制御
	横壁センサによる目標角度生成
	mode == TURN_MODE であれば何もしない（tar_ang_velは一定)
	*****************************************************************************************/
	if(run_mode == STRAIGHT_MODE){
		if(con_wall.enable == true && sen_fr.value + sen_fl.value <= (TH_SEN_FR+TH_SEN_FL)*5 )		//壁制御が許可されているかチェック
		{
			
			con_wall.p_error = con_wall.error;	//過去の偏差を保存
			
			
			//左右のセンサが、それぞれ使える状態であるかどうかチェックして、姿勢制御の偏差を計算
			if( ( sen_r.is_control == true ) && ( sen_l.is_control == true ) )
			{									//両方とも有効だった場合の偏差を計算
				con_wall.error = sen_r.error - sen_l.error;
			}
			else								//片方もしくは両方のセンサが無効だった場合の偏差を計算
			{									//一方のセンサが無効であった場合、そのセンサのerrorも0になる
				con_wall.error = 2.0 * (sen_r.error - sen_l.error);	//片方しか使用しないので2倍する
			}
			
			
			//DI制御計算
			con_wall.diff = con_wall.error - con_wall.p_error;	//偏差の微分値を計算
			con_wall.sum += con_wall.error;				//偏差の積分値を計算
			
			if(con_wall.sum > con_wall.sum_max)			//偏差の積分値の最大値を制限
			{
				con_wall.sum = con_wall.sum_max;
			}
			else if(con_wall.sum < (-con_wall.sum_max))		//偏差の積分値の最低値を制限
			{
				con_wall.sum = -con_wall.sum_max;
			}

			con_wall.p_omega = con_wall.omega;	//過去の目標角速度を保存
			// con_wall.omega = con_wall.kp * con_wall.error;
			con_wall.omega = con_wall.kp * con_wall.error * 0.5 + con_wall.p_omega * 0.5;	//現在の目標角速度[rad/s]を計算
			tar_ang_vel = con_wall.omega;		//目標角速度を、omegaで更新する
		}else{
			tar_ang_vel = 0;
		}
		
	}else if(run_mode == NON_CON_MODE){	
		//何もしない
		nop();
	}else{
		nop();	
	}
	
	/*****************************************************************************************
	壁制御かスラロームの理想値によって生成された
	目標速度と目標角角度の積分
	*****************************************************************************************/
	I_tar_speed += tar_speed;
	if(I_tar_speed >30*10000000000){
		I_tar_speed = 30*10000000000;
	}else if(I_tar_speed < -1*10000000000){
		I_tar_speed = -1*10000000000;
	}
	
	I_tar_ang_vel += tar_ang_vel;
	if(I_tar_ang_vel >30*10000000000){
		I_tar_ang_vel = 30*10000000000;
	}else if(I_tar_ang_vel < -1*10000000000){
		I_tar_ang_vel = -1*10000000000;
	}
	/*****************************************************************************************
	目標速度の偏差から出力電圧にフィードバック
	角速度が正：CCW , V_r / increase , V_l / defrease
	角速度が負：CW , V_r / decrease , V_l / increase
	*****************************************************************************************/
	//フィードバック制御
	V_r = V_l = 0.0;
	if(run_mode == STRAIGHT_MODE || run_mode == TURN_MODE){
		//直進時のフィードバック制御
		//左右モータのフィードバック

		//速度に対するP制御
		V_r += 1 * (tar_speed - speed) *SPEED_KP/1.0; //15目標値付近で発振
		V_l += 1 * (tar_speed - speed) *SPEED_KP/1.0;
		//速度に対するI制御
		V_r += 1 * (I_tar_speed - I_speed) *SPEED_KI/1.0; //(0.4-0.3)*0.1 -> 0.01 
		V_l += 1 * (I_tar_speed - I_speed) *SPEED_KI/1.0;
		//速度に対するD制御
		V_r -= 1 * (p_speed - speed) *SPEED_KD/1.0; //(0.4-0.3)*0.1 -> 0.01 
		V_l -= 1 * (p_speed - speed) *SPEED_KD/1.0;

		//角速度に対するP制御
		V_r += 1 * (tar_ang_vel - ang_vel) *(OMEGA_KP/100.0);
		V_l -= 1 * (tar_ang_vel - ang_vel) *(OMEGA_KP/100.0);
		//角速度に対するI制御
		V_r += 1 * (I_tar_ang_vel - I_ang_vel) *(OMEGA_KI/100.0); //(0.4-0.3)*0.1 -> 0.01 
		V_l -= 1 * (I_tar_ang_vel - I_ang_vel) *(OMEGA_KI/100.0);
		//角速度に対するD制御
		V_r -= 1 * (p_ang_vel - ang_vel) *(OMEGA_KD/100.0); //(0.4-0.3)*0.1 -> 0.01 
		V_l += 1 * (p_ang_vel - ang_vel) *(OMEGA_KD/100.0);

	}else if(run_mode == NON_CON_MODE){
		//何もしない
		nop();
	}else{
		//何もしない
		nop();	
	}
	
	/*****************************************************************************************
	出力電圧からモータの回転方向を決定
		電圧は絶対値を入力するため、
		正の値の時はモータを正転設定にして、電圧はそのまま
		負の値の時はモータを逆転設定にして、電圧を反転する
	*****************************************************************************************/
	if(run_mode != TEST_MODE){
		//右モータの出力電圧が正の場合
		if(V_r > 0){
			//モータを正転に設定
			MOTOR_RIGHT_DIR_OUT = MOT_R_FORWARD;	//右モータを正転に設定
			//V_r = V_r;			//電圧は正なのでそのまま
		}else{
		//右モータの出力電圧が負の場合
			MOTOR_RIGHT_DIR_OUT  = MOT_R_BACK;	//右モータを逆回転に設定
			V_r = -V_r;			//電圧を正の値へ反転
		}
		
		//左モータの出力電圧が正の場合
		if(V_l > 0){
			//モータを正転に設定
			MOTOR_LEFT_DIR_OUT = MOT_L_FORWARD;	//左モータを正転に設定
			//V_l = V_l;			//電圧は正なのでそのまま
		}else{
		//左モータの出力電圧が負の場合
			MOTOR_LEFT_DIR_OUT  = MOT_L_BACK;	//左モータを逆回転に設定
			V_l = -V_l;			//電圧を正の値へ反転
		}
	}	
	/*****************************************************************************************
	出力電圧の制限
		モータへの出力電圧の上限を2Vに制限
	*****************************************************************************************/
	//右モータに印加する電圧が2.0Vを超えたら強制的に2.0Vに変更
	if(V_r > 2.0){
		V_r = 2.0;
	}
	//左モータに印加する電圧が2.0Vを超えたら強制的に2.0Vに変更
	if(V_l > 2.0){
		V_l = 2.0;
	}
	
	/*****************************************************************************************
	モータへ出力
		
	*****************************************************************************************/
	
	//バッテリー電圧からデューティを計算
	Duty_r = V_r/V_bat;
	Duty_l = V_l/V_bat;
	
	//モータにPWMを出力
	if(run_mode != TEST_MODE){
		MOT_OUT_R =(short)(240.0 * Duty_r);
		MOT_OUT_L =(short)(240.0 * Duty_l);
	}
	
	/*****************************************************************************************
	タイマのカウント
		
	*****************************************************************************************/
	timer++;
	cnt++;
	
}

//4kHz : ローテーションしながらセンサの値を読む(1msごと)
void int_cmt1(void)
{
	/*****************************************************************************************
	A/D変換
		センサとバッテリー電圧取得
	*****************************************************************************************/
	static int state = 0;	//読み込むセンサのローテーション管理用変数
	int i;
	
	switch(state)
	{
		case 0:		//右センサ読み込み

			//差分フィルタ
			S12AD.ADANS0.BIT.ANS0=0x0004;			//AN002
			ADC_START=1;				//AD変換開始
			while(ADC_START);			//AD変換終了まで待つ
			sen_r.d_value = DATA_SENSOR_R;			//環境値を保存
			
			LED_RIGHT_OUT = 1;					//LED点灯
			for(i = 0; i < WAITLOOP_SLED; i++);		//フォトトランジスタの応答待ちループ
			S12AD.ADANS0.BIT.ANS0=0x0004;			//AN002
			ADC_START=1;				//AD変換開始
			while(ADC_START);			//AD変換終了まで待つ
			LED_RIGHT_OUT = 0;					//LED消灯
			
			sen_r.value = (DATA_SENSOR_R - sen_r.d_value);	//値を保存

			if(sen_r.value > sen_r.th_wall)			//壁の有無を判断
			{
				sen_r.is_wall = true;			//右壁あり
			}
			else
			{
				sen_r.is_wall = false;			//右壁なし
			}
			
			if(sen_r.value > sen_r.th_control)		//制御をかけるか否かを判断
			{
				sen_r.error = sen_r.value - sen_r.ref;	//制御をかける場合は偏差を計算
				sen_r.is_control = true;		//右センサを制御に使う
			}
			else
			{
				sen_r.error = 0;				//制御に使わない場合は偏差を0にしておく
				sen_r.is_control = false;		//右センサを制御に使わない
			}			
			break;


		case 1:		//前左センサ読み込み

			//差分フィルタ
			S12AD.ADANS0.BIT.ANS0=0x0001;			//AN000
			ADC_START=1;				//AD変換開始
			while(ADC_START);			//AD変換終了まで待つ
			sen_fl.d_value = DATA_SENSOR_FL;			//値を保存
		
			//LED点灯消灯処理
			LED_LEFT_FORWARD_OUT = 1;					//LED点灯
			for(i = 0; i < WAITLOOP_SLED; i++);		//フォトトランジスタの応答待ちループ
			S12AD.ADANS0.BIT.ANS0=0x0001;			//AN000
			ADC_START=1;				//AD変換開始
			while(ADC_START);			//AD変換終了まで待つ
			LED_LEFT_FORWARD_OUT = 0;					//LED消灯

			sen_fl.value = (DATA_SENSOR_FL - sen_fl.d_value);	//値を保存

			if(sen_fl.value > sen_fl.th_wall)		//閾値により壁の有無を判断
			{
				sen_fl.is_wall = true;			//左前壁あり
			}
			else
			{
				sen_fl.is_wall = false;			//左前壁なし
			}
			break;


		case 2:		//前右センサ読み込み
		
			//差分フィルタ
			S12AD.ADANS0.BIT.ANS0=0x0040;			//AN006
			ADC_START=1;				//AD変換開始
			while(ADC_START);			//AD変換終了まで待つ
			sen_fr.d_value = DATA_SENSOR_FR;			//値を保存

			//LED点灯消灯処理
			LED_RIGHT_FORWARD_OUT = 1;					//LED点灯
			for(i = 0; i < WAITLOOP_SLED; i++);		//フォトトランジスタの応答待ちループ
			S12AD.ADANS0.BIT.ANS0=0x0040;			//AN006
			ADC_START=1;				//AD変換開始
			while(ADC_START);			//AD変換終了まで待つ
			LED_RIGHT_FORWARD_OUT = 0;					//LED消灯
			
			sen_fr.value = (DATA_SENSOR_FR - sen_fr.d_value);	//値を保存

			if(sen_fr.value > sen_fr.th_wall)		//壁の有無を判断
			{
				sen_fr.is_wall = true;			//右前壁あり
			}
			else
			{
				sen_fr.is_wall = false;			//右前壁なし
			}			
			break;


		case 3:		//左センサ読み込み
		
			//差分フィルタ
			S12AD.ADANS0.BIT.ANS0=0x0002;			//AN001
			ADC_START=1;				//AD変換開始
			while(ADC_START);			//AD変換終了まで待つ
			sen_l.d_value = DATA_SENSOR_L;			//値を保存

			//LED点灯消灯処理
			LED_LEFT_OUT = 1;					//LED点灯
			for(i = 0; i < WAITLOOP_SLED; i++)	;	//フォトトランジスタの応答待ちループ
			S12AD.ADANS0.BIT.ANS0=0x0002;			//AN001
			ADC_START=1;				//AD変換開始
			while(ADC_START);			//AD変換終了まで待つ
			LED_LEFT_OUT = 0;					//LED消灯
			
			sen_l.value = (DATA_SENSOR_L - sen_l.d_value);	//値を保存
			
			if(sen_l.value > sen_l.th_wall)			//壁の有無を判断
			{
				sen_l.is_wall = true;			//左壁あり
			}
			else
			{
				sen_l.is_wall = false;			//左壁なし
			}
			
			if(sen_l.value > sen_l.th_control)		//制御をかけるか否かを判断
			{
				sen_l.error = sen_l.value - sen_l.ref;	//制御をかける場合は偏差を計算する
				sen_l.is_control = true;		//左センサを制御に使う
			}
			else
			{
				sen_l.error = 0;			//制御に使わない場合は偏差を0にしておく
				sen_l.is_control = false;		//左センサを制御に使わない
			}

			break;
	}
	
	state++;		//4回ごとに繰り返す
	if(state > 3)
	{
		state = 0;
	}
	
	S12AD.ADANS0.BIT.ANS0=0x0200;			//AN009
	ADC_START=1;				//AD変換開始
	while(ADC_START);			//AD変換終了まで待つ
	V_bat = VATT_CALC();

	if(V_bat < 3.3){
		
		//モータ止める
		Duty_r = 0;
		Duty_l = 0;
		MOTOR_EN_OUT = 0;	//PC6(SLEEPピン)
		
		//ブザー鳴らし続ける
		while(1){
			BEEP(50,440,1);
		}
	}
}

/*-------------------------------------------------------------------------
周期 : 2kHz
左右のエンコーダのdataを1msごとに取得 2msごとにエンコーダの値を処理
ジャイロセンサのdataを2msごとに取得 処理する
-------------------------------------------------------------------------*/
void int_cmt2(void)
{
	static unsigned int	enc_data_r;	//エンコーダの生データ
	static unsigned int	enc_data_l;	//エンコーダの生データ 
	static short	state;
	/*****************************************************************************************
	エンコーダ関連
		値の取得 速度更新 距離積分など
	*****************************************************************************************/	
	if(state == 0){
		ENC_SS = 	ENC_RIGHT_SS;	//SSL信号アサート設定(SSL0を使う)
		preprocess_spi_enc(0x1400);	//Read Angle
		enc_data_r = Get_enc_data();
		state = 1;
	}else{
		ENC_SS = 	ENC_LEFT_SS;	//SSL信号アサート設定(SSL2を使う)
		preprocess_spi_enc(0x1300);	//Read Angle
		enc_data_l = Get_enc_data();
		
		//左右エンコーダから角度取得
		//4096で一回転(360deg = 0deg)
		locate_r = enc_data_r;
		locate_l = enc_data_l;
		
		//右エンコーダの現在の位置と,1msec前の位置との差分を計算
		//単位時間（1msec）あたりの変位量を計算
		diff_pulse_r = (locate_r - before_locate_r);
		//変化点を1023から0//へ移動したときの補正
		if((diff_pulse_r > ENC_RES_HALF || diff_pulse_r < -ENC_RES_HALF) && before_locate_r >ENC_RES_HALF){
			diff_pulse_r = (((ENC_RES_MAX - 1) - before_locate_r) + locate_r);
		}
		//変化点を0から1023へ移動したときの補正
		else if((diff_pulse_r > ENC_RES_HALF || diff_pulse_r < -ENC_RES_HALF) && before_locate_r <=ENC_RES_HALF){
			diff_pulse_r = 1*(before_locate_r + ((ENC_RES_MAX - 1) - locate_r));
		}
		
		//左エンコーダの現在の位置と,1msec前の位置との差分を計算
		//単位時間（1msec）あたりの変位量を計算
		diff_pulse_l = (-locate_l + before_locate_l);
		//変化点を1023から0//へ移動したときの補正
		if((diff_pulse_l > ENC_RES_HALF || diff_pulse_l < -ENC_RES_HALF) && before_locate_l >ENC_RES_HALF){
			diff_pulse_l = 1*(((ENC_RES_MAX - 1) - before_locate_l) + locate_l);
		}
		//変化点を0から1023へ移動したときの補正
		else if((diff_pulse_l > ENC_RES_HALF || diff_pulse_l < -ENC_RES_HALF) && before_locate_l <=ENC_RES_HALF){
			diff_pulse_l = (before_locate_l + ((ENC_RES_MAX - 1) - locate_l));
		}
				
		//現在速度を算出
		speed_new_r = (float)((float)diff_pulse_r * (float)MMPP * 1000/1000); //単位を[m/s]に変換する
		speed_new_l = (float)((float)diff_pulse_l * (float)MMPP * 1000/1000);
		
		//過去の値を保存
		speed_old_r= speed_r;
		speed_old_l= speed_l;
		
		//速度のローパスフィルタ
		speed_r = speed_new_r * 0.1 + speed_old_r * 0.9;
		speed_l = speed_new_l * 0.1 + speed_old_l * 0.9;
		
		p_speed = speed;
		//車体速度を計算
		speed = ((speed_r + speed_l)/2.0);
		
		//I成分のオーバーフローとアンダーフロー対策
		I_speed += speed;
		if(I_speed >30*10000000000){
			I_speed = 30*10000000000;
		}else if(I_speed < -1*10000000000){
			I_speed = -1*10000000000;
		}

		//距離の計算
		len_mouse += (speed_new_r + speed_new_l)/2.0;
		
		//過去の値を保存
		before_locate_r = locate_r;
		before_locate_l = locate_l;
				
		state = 0;
	}
	
	
	/*****************************************************************************************
	ジャイロ関連(ヨー軸)
		値の取得 角度の積分 
	*****************************************************************************************/
	if(state == 1){
		//ジャイロセンサの値の更新
		preprocess_spi_gyro_2byte(0xB70000);
		
		//LowPass Filter
		gyro_x_new = (float)((short)(read_gyro_data() & 0x0000FFFF));
		gyro_x = (gyro_x_new -gyro_ref );
		
		//角速度の更新
		p_ang_vel = ang_vel;
		ang_vel = ((2000.0*(gyro_x)/32767.0))*PI/180.0;	//ジャイロの値をdegreeに変換した後、radに変換
		//積分値の更新
		I_ang_vel += ang_vel;
		if(I_ang_vel >30*10000000000){
			I_ang_vel = 30*10000000000;
		}else if(I_ang_vel < -1*10000000000){
			I_ang_vel = -1*10000000000;
		}
		
		//ジャイロの値を角度に変換
		degree += (2.0*(gyro_x_new - gyro_ref)/32767.0);
		
	}	
	
}

