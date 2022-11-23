#include "motor.h"
#include "../../iodefine.h"
#include "../Definations/system_definations.h"

void motor_on(int R_dir, int L_dir, int R_duty, int L_duty){

	motor_off();	//�O�̂��߃��[�^�[���I�t�ɂ���

	//�f���[�e�B�[���ݒ�A�J�E���^���N���A
	MTU0.TGRA = L_duty;		//���̃f���[�e�B�[���ݒ� [ TGRA(C) / TGRD ]
	MTU0.TGRC = R_duty;		//�E�̃f���[�e�B�[���ݒ�
	MTU0.TCNT = 0;		//���荞�݊J�n�O�ɃJ�E���^���N���A

	MOTOR_EN_OUT = 1;	//SLEEP����

	//���[�^�[�̉�]������ݒ�
	MOTOR_RIGHT_DIR_OUT =R_dir;	//R_PH 
	MOTOR_LEFT_DIR_OUT = L_dir;	//L_PH 
	MTU.TSTR.BIT.CST0 = 1; 		//MTU0�̃J�E���^����J�n

}

void motor_off(void){

	MOTOR_EN_OUT = 0;			//SLEEP�ݒ�
	MTU.TSTR.BIT.CST0 = 0; 		//MTU0�̃J�E���^�����~
}
