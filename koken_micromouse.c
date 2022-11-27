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

//�����݂ɂ���
//IR : �����݃X�e�[�^�X�t���O�A�����݂��N����ƃZ�b�g�A�I���ƃN���A�����
//IEN : IR���W�X�^�̃X�e�[�^�X�������ݐ�(CPU�ɓ`����@�\)�B������ꍇ�ɂ̓Z�b�g
//IPR : �����ݗD�揇�ʂ�ݒ�(15~1) 15:max , 1:min , 0:�����݋֎~
//IEN��IPR��IR

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
				
				//�����@�A�T��
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,3);
					degree = 0;
					timer = 0;
					gyro_get_ref();
					BEEP(50,440,1);
					mypos.x = mypos.y = 0;			//���W��������
					mypos.dir = north;			//���p��������
					search_adachi(GOAL_X,GOAL_Y);		//�S�[���܂ő����@
					turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);			//�S�[��������180�x��]����
					mypos.dir = (mypos.dir+6) % 4;		//���p���X�V
					BEEP(50,440,5);	//�S�[���������Ƃ��A�s�[��
					search_adachi(0,0);			//�X�^�[�g�n�_�܂ő����@�ŋA���Ă���
					turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);			//�A���Ă�����180�x��]	
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
			
				//�ŒZ���s
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,3);
					degree = 0;
					timer = 0;
					gyro_get_ref();
					BEEP(50,440,1);
					mypos.x = mypos.y = 0;			//���W��������
					mypos.dir = north;			//���p��������
					fast_run(GOAL_X,GOAL_Y);		//�S�[���܂ő����@
					turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);			//�S�[��������180�x��]����
					mypos.dir = (mypos.dir+6) % 4;		//���p���X�V
					BEEP(50,440,5);	//�S�[���������Ƃ��A�s�[��
					search_adachi(0,0);			//�X�^�[�g�n�_�܂ő����@�ŋA���Ă���
					turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);			//�A���Ă�����180�x��]	
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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
				//mode 3~14�͋�
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

				//mode 3~14�͋�
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
			
				//�������[�h�Ɉڍs
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,1000,2);
					while(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4);
					adjust_mode_select();
				}				
				break;
				
			//mode0~15�ȊO�̏ꍇ�B�������Ȃ��B
			default:
				break;
			
		}
		MOTOR_EN_OUT = 0;
		//���[�h�؂�ւ��p����
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
