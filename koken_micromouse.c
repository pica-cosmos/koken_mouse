/***********************************************************************/
/*                                                                     */
/*  FILE        :Main.c or Main.cpp                                    */
/*  DATE        :Tue, Oct 31, 2006                                     */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
//#include "typedefine.h"

//割込みについて
//IR : 割込みステータスフラグ、割込みが起こるとセット、終わるとクリアされる
//IEN : IRレジスタのステータスを割込み先(CPUに伝える機能)。許可する場合にはセット
//IPR : 割込み優先順位を設定(15~1) 15:max , 1:min , 0:割込み禁止
//IEN→IPR→IR

#include <machine.h>
#include "iodefine.h"
#include "RobotLib/Definations/system_definations.h"
#include "RobotLib/Parameters/static_parameters.h"
#include "RobotLib/Parameters/adjust_parameters.h"
#include "RobotLib/Hardwares/motor.h"
#include "RobotLib/Hardwares/adc.h"
#include "RobotLib/Hardwares/sci.h"
#include "RobotLib/Hardwares/spi.h"
#include "RobotLib/Hardwares/i2c.h"
#include "RobotLib/init.h"
#include "RobotLib/glob_var.h"
#include "RobotLib/interface.h"
#include "RobotLib/search_algo.h"
#include "RobotLib/interrupt.h"
#include "RobotLib/run.h"
#include "RobotLib/fast.h"
#include "RobotLib/test.c"

extern void wait_ms(int miribyou);

// #define TYOUSEI
// #define SISOU
#define TEST
#pragma inline(print_str)

#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif


int main(void)
{
	volatile long i,n;
	volatile long data = 0;
	volatile int sci_tend_flag = 0;
	char mode = 1;
	// wall_sensor sensor_val;

	// init_all();
	// for ( i = 0; i < 100*100*100; i++);
#ifdef TYOUSEI
	BEEP(50,440,1);
	gyro_get_ref();
	BEEP(50,440,1);
	len_mouse = 0;
	straight(SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
	MOTOR_EN_OUT = 0;
	BEEP(50,440,1);
#endif

#ifdef SISOU
	while(1){
		I_tar_ang_vel = 0;
		I_ang_vel = 0;
		I_tar_speed = 0;
		I_speed = 0;

		switch(mode){
			
			case 1:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	X	X	X	*
				*					*
				*****************************************/
				
				//足立法、探索
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,3);
					degree = 0;
					timer = 0;
					gyro_get_ref();
					BEEP(50,440,1);
					mypos.x = mypos.y = 0;			//座標を初期化
					mypos.dir = north;			//方角を初期化
					search_adachi(GOAL_X,GOAL_Y);		//ゴールまで足立法
					turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);			//ゴールしたら180度回転する
					mypos.dir = (mypos.dir+6) % 4;		//方角を更新
					BEEP(50,440,5);	//ゴールしたことをアピール
					search_adachi(0,0);			//スタート地点まで足立法で帰ってくる
					turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);			//帰ってきたら180度回転	
					MOTOR_EN_OUT = 0;
					BEEP(50,440,1);
				}
				
				break;
				
			case 2:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	X	O	X	X	*
				*					*
				*****************************************/	
			
				//最短走行
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,3);
					degree = 0;
					timer = 0;
					gyro_get_ref();
					BEEP(50,440,1);
					mypos.x = mypos.y = 0;			//座標を初期化
					mypos.dir = north;			//方角を初期化
					fast_run(GOAL_X,GOAL_Y);		//ゴールまで足立法
					turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);			//ゴールしたら180度回転する
					mypos.dir = (mypos.dir+6) % 4;		//方角を更新
					BEEP(50,440,5);	//ゴールしたことをアピール
					search_adachi(0,0);			//スタート地点まで足立法で帰ってくる
					turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);			//帰ってきたら180度回転	
					MOTOR_EN_OUT = 0;
					BEEP(50,440,1);
				}
				
				break;
				
			case 3:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	O	X	X	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 4:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	X	X	O	X	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 5:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	X	O	X	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 6:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	X	O	O	X	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 7:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	O	O	X	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 8:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	X	X	X	O	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 9:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	X	X	O	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 10:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	X	O	X	O	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 11:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	O	X	O	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 12:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	X	X	O	O	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 13:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	X	O	O	*
				*					*
				*****************************************/
				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
			case 14:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	X	O	O	O	*
				*					*
				*****************************************/

				//mode 3~14は空き
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;
				
			case 15:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	O	O	O	*
				*					*
				*****************************************/
			
				//調整モードに移行
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,1000,2);
					while(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4);
					adjust_mode_select();
				}				
				break;
				
			//mode0~15以外の場合。何もしない。
			default:
				break;
			
		}
		MOTOR_EN_OUT = 0;
		//モード切り替え用処理
		mode_change(&mode,1,15);
	
	}
#endif

#ifdef TEST
	gyro_test();
#endif

	return 0;

}

#ifdef __cplusplus
void abort(void)
{

}
#endif
