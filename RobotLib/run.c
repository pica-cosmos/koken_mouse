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
	//�ϕ��v�f�̏�����
	I_tar_ang_vel = 0;
	I_ang_vel = 0;
	I_tar_speed = 0;
	I_speed = 0;
	//���s���[�h�𒼐��ɂ���
	run_mode = STRAIGHT_MODE;
	//�ǐ����L���ɂ���
	con_wall.enable = true;
	//�ڕW�������O���[�o���ϐ��ɑ������
	len_target = len;
	//�ڕW���x��ݒ�
	end_speed = end_sp;
	//�����x��ݒ�
	accel = acc;
	//�ō����x��ݒ�
	max_speed = max_sp;
	
	//���[�^�o�͂�ON
	MOTOR_EN_OUT = 1;	
	
	if(end_speed == 0){	//�ŏI�I�ɒ�~����ꍇ
		//�����������n�߂�ׂ��ʒu�܂ŉ����A�葬��Ԃ𑱍s
		//[-10]�͌�쓮�h�~�i�ڂ����̓O���t�������āj
		while( ((len_target -10) - len_mouse) >  1000.0*((float)(tar_speed * tar_speed) - (float)(end_speed * end_speed))/(float)(2.0*accel));
		//���������J�n
		accel = -acc;					//�������邽�߂ɉ����x�𕉂̒l�ɂ���	
		while(len_mouse < len_target -1){		//��~�����������̏�����O�܂Ōp��
			//��葬�x�܂Ō���������Œ�쓮�g���N�ő��s
			if(tar_speed <= MIN_SPEED){	//�ڕW���x���Œᑬ�x�ɂȂ�����A�����x��0�ɂ���
				accel = 0;
				tar_speed = MIN_SPEED;	//�ᑬ�^��
			}
		}
		accel = 0;
		tar_speed = 0;	//�ڕW���x���O��
		//���x��0�ȉ��ɂȂ�܂ŋt�]����
		while(speed >= 0.0);
			
	}else{
		//�����������n�߂�ׂ��ʒu�܂ŉ����A�葬��Ԃ𑱍s
		while( ((len_target-10) - len_mouse) >  1000.0*((float)(tar_speed * tar_speed) - (float)(end_speed * end_speed))/(float)(2.0*accel));
		
		//���������J�n
		accel = -acc;					//�������邽�߂ɉ����x�𕉂̒l�ɂ���	
		while(len_mouse < len_target){		//��~�����������̏�����O�܂Ōp��
			//��葬�x�܂Ō���������Œ�쓮�g���N�ő��s
			if(tar_speed <= end_speed){	//�ڕW���x���Œᑬ�x�ɂȂ�����A�����x��0�ɂ���
				accel = 0;
				tar_speed = end_speed; 	//�ᑬ�^��
			}
		}
	}
	//�����x��0�ɂ���
	accel = 0;
	//���݋�����0�Ƀ��Z�b�g
	len_mouse = 0;
}

void turn(int deg, float ang_accel, float max_ang_velocity, short dir){
	float start_degree = 0;
	
	wait_ms(WAIT_TIME);
	//�ϕ��v�f�̏�����
	I_tar_ang_vel = 0;
	I_ang_vel = 0;
	I_tar_speed = 0;
	I_speed = 0;

	//�ԑ̂̑��x�͈��A�����x�O�ŉ^������
	tar_speed = 0;
	accel = 0;
	//�ڕW�p�����[�^�̏�����
	tar_degree = 0;	//�����g�p �ړ��p�x�̖ڕW�l
	tar_ang_vel = 0;
	//���s���[�h�𒴐i�n���񃂁[�h�ɂ���
	run_mode = TURN_MODE;

	//��]������`
	TURN_DIR = dir;	
	
	//�ԑ̂̌��݊p�x���擾
	start_degree = degree;
	
	//�p�����x�A�����x�A�ō��p���x�ݒ�
	MOTOR_EN_OUT = 1;	//���[�^�[��sleep������
	if(dir == LEFT){
		ang_acc = ang_accel;			//�p�����x��ݒ�
		max_ang_vel = max_ang_velocity;
		max_degree = deg;
		while( (max_degree - (degree - start_degree))*PI/180.0 > (tar_ang_vel*tar_ang_vel/(2.0 * ang_acc)));
		
	}else if(dir == RIGHT){
		ang_acc = -ang_accel;			//�p�����x��ݒ�
		max_ang_vel = -max_ang_velocity;
		max_degree = -deg;
		//���ӂɃ}�C�i�X���|���邱�ƂŁA�^���I��dir==LEFT�̏ꍇ�Ɠ����悤�ɂ��ď���
		while(-(float)(max_degree - (degree - start_degree))*PI/180.0 > (float)(tar_ang_vel*tar_ang_vel/(float)(2.0 * -ang_acc)));
	}

	//�p������Ԃɓ��邽�߁A�p�����x�ݒ�
	MOTOR_EN_OUT = 1;	//���[�^�[��sleep������
	if(dir == LEFT){
		ang_acc = -ang_accel;			//�p�����x��ݒ�
		//������ԑ��s
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
		ang_acc = +ang_accel;			//�p�����x��ݒ�
		//������ԑ��s
		while((degree - start_degree) > max_degree){
			if(-tar_ang_vel < TURN_MIN_SPEED){
				ang_acc = 0;
				tar_ang_vel = -TURN_MIN_SPEED;
			}
		}
		ang_acc = 0;
		tar_ang_vel = 0;	//�ڕW�p���x���O��
		tar_degree = max_degree;


	}
	//�p���x���������Ȃ�܂ŋt�]
	while(ang_vel >= 0.05 || ang_vel <= -0.05 );
	
	tar_ang_vel = 0;	//�ڕW�p���x����U������(�ǐ���̎��ɕϓ�)
	ang_acc = 0;
	//���݋�����0�Ƀ��Z�b�g
	len_mouse = 0;
	wait_ms(WAIT_TIME);
}
