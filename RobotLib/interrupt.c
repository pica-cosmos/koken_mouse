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

// void (*cmt0_cmi0_interrupt_handler)(void); // CMI0�̊����ݗp�n���h��

// void CMT0_START(float interrupt_intv_ms, void (*handler)(void)){

//     cmt0_cmi0_interrupt_handler = handler; // CMI0�̊��荞�݃n���h���̐ݒ�

//     CMT.CMSTR0.BIT.STR0 = 0;
//     CMT0.CMCR.BIT.CMIE = 1;                                  //�R���y�A�}�b�`���荞�݋���
//     CMT0.CMCNT = 0;                                          //�J�E���^���N���A
//     CMT0.CMCOR = (int)((float)cmt0_cycle_per_ms * interrupt_intv_ms) - 1; //�����ݎ������v�Z
//     CMT.CMSTR0.BIT.STR0 = 1;
// }

// void CMT0_STOP(void){

//     CMT.CMSTR0.BIT.STR0 = 0; //�J�E���^��~
//     CMT0.CMCR.BIT.CMIE = 0;  //�����݋֎~
//     CMT0.CMCNT = 0;          //�J�E���^���N���A
// }

void timer_start(void){
	
	CMT.CMSTR0.BIT.STR0=1;	//�J�E���g�X�^�[�g
	CMT.CMSTR0.BIT.STR1=1;	//�J�E���g�X�^�[�g
	CMT.CMSTR1.BIT.STR2=1;	//�J�E���g�X�^�[�g
	CMT.CMSTR1.BIT.STR3=1;	//�J�E���g�X�^�[�g
}

void timer_stop(void){
	
	CMT.CMSTR0.BIT.STR0=0;	//�J�E���g��~
	CMT.CMSTR0.BIT.STR1=0;	//�J�E���g��~
	CMT.CMSTR1.BIT.STR2=0;	//�J�E���g��~
	CMT.CMSTR1.BIT.STR3=0;	//�J�E���g��~
	
}

//1kHz(1ms)
void int_cmt0(void)
{
	/*****************************************************************************************
	�ڕW���x����
	tar_speed�̐���
	�Ȃ�
	*****************************************************************************************/
	//�����̏ꍇ�̖ڕW���x����
	if(run_mode == STRAIGHT_MODE){
		tar_speed += accel/1000.0;	//�ڕW���x��ݒ�����x�ōX�V 1ms���ƂɌv������
		//�ō����x����
		if(tar_speed > max_speed){
			tar_speed = max_speed;	//�ڕW���x��ݒ�ō����x�ɐݒ�
		}
				
	
	}else if(run_mode == TURN_MODE){
		
		//�ԑ̒��S���x�X�V
		tar_speed += accel/1000;	//1ms���Ƃɓǂނ̂�1000�Ŋ���
		//�ō����x����
		if(tar_speed > max_speed){
			tar_speed = max_speed;	//�ڕW���x��ݒ�ō����x�ɐݒ�
		}
		
		//�p�����x�X�V
		tar_ang_vel += ang_acc/1000.0;	//�ڕW�p���x��ݒ�����x�ōX�V
		tar_degree  += (tar_ang_vel*180.0/PI)/1000.0;	//1ms���Ƃɓǂނ̂�1000�Ŋ��� rad��degree�ɕϊ�����

		
		if(TURN_DIR == LEFT){
			//����]�̏ꍇ : ang_vel, tar_degree : �� ( a>b �� a�̕����p���x�E�ړ��p�x�͑傫��)
			//�ō��p���x����
			if(tar_ang_vel > max_ang_vel){
				tar_ang_vel = max_ang_vel;	//�ڕW���x��ݒ�ō����x�ɐݒ�
			}
			if(tar_degree > max_degree){
				tar_degree = max_degree;
			}
		}else if(TURN_DIR == RIGHT){
			//�E��]�̏ꍇ : ang_vel, tar_degree : �� ( a<b �� a�̕����p���x�E�ړ��p�x�͑傫��)
			//�ō��p���x����
			if(tar_ang_vel < max_ang_vel){
				tar_ang_vel = max_ang_vel;	//�ڕW���x��ݒ�ō����x�ɐݒ�
			}
			if(tar_degree < max_degree){
				tar_degree = max_degree;
			}
		}
		
	}else if(run_mode == NON_CON_MODE){
		//�������Ȃ�
		nop();	
	}else{
		//�������Ȃ�
		nop();	
	}
	
	/*****************************************************************************************
	�ǐ���
	���ǃZ���T�ɂ��ڕW�p�x����
	mode == TURN_MODE �ł���Ή������Ȃ��itar_ang_vel�͈��)
	*****************************************************************************************/
	if(run_mode == STRAIGHT_MODE){
		if(con_wall.enable == true && sen_fr.value + sen_fl.value <= (TH_SEN_FR+TH_SEN_FL)*5 )		//�ǐ��䂪������Ă��邩�`�F�b�N
		{
			
			con_wall.p_error = con_wall.error;	//�ߋ��̕΍���ۑ�
			
			
			//���E�̃Z���T���A���ꂼ��g�����Ԃł��邩�ǂ����`�F�b�N���āA�p������̕΍����v�Z
			if( ( sen_r.is_control == true ) && ( sen_l.is_control == true ) )
			{									//�����Ƃ��L���������ꍇ�̕΍����v�Z
				con_wall.error = sen_r.error - sen_l.error;
			}
			else								//�Е��������͗����̃Z���T�������������ꍇ�̕΍����v�Z
			{									//����̃Z���T�������ł������ꍇ�A���̃Z���T��error��0�ɂȂ�
				con_wall.error = 2.0 * (sen_r.error - sen_l.error);	//�Е������g�p���Ȃ��̂�2�{����
			}
			
			
			//DI����v�Z
			con_wall.diff = con_wall.error - con_wall.p_error;	//�΍��̔����l���v�Z
			con_wall.sum += con_wall.error;				//�΍��̐ϕ��l���v�Z
			
			if(con_wall.sum > con_wall.sum_max)			//�΍��̐ϕ��l�̍ő�l�𐧌�
			{
				con_wall.sum = con_wall.sum_max;
			}
			else if(con_wall.sum < (-con_wall.sum_max))		//�΍��̐ϕ��l�̍Œ�l�𐧌�
			{
				con_wall.sum = -con_wall.sum_max;
			}

			con_wall.p_omega = con_wall.omega;	//�ߋ��̖ڕW�p���x��ۑ�
			// con_wall.omega = con_wall.kp * con_wall.error;
			con_wall.omega = con_wall.kp * con_wall.error * 0.5 + con_wall.p_omega * 0.5;	//���݂̖ڕW�p���x[rad/s]���v�Z
			tar_ang_vel = con_wall.omega;		//�ڕW�p���x���Aomega�ōX�V����
		}else{
			tar_ang_vel = 0;
		}
		
	}else if(run_mode == NON_CON_MODE){	
		//�������Ȃ�
		nop();
	}else{
		nop();	
	}
	
	/*****************************************************************************************
	�ǐ��䂩�X�����[���̗��z�l�ɂ���Đ������ꂽ
	�ڕW���x�ƖڕW�p�p�x�̐ϕ�
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
	�ڕW���x�̕΍�����o�͓d���Ƀt�B�[�h�o�b�N
	�p���x�����FCCW , V_r / increase , V_l / defrease
	�p���x�����FCW , V_r / decrease , V_l / increase
	*****************************************************************************************/
	//�t�B�[�h�o�b�N����
	V_r = V_l = 0.0;
	if(run_mode == STRAIGHT_MODE || run_mode == TURN_MODE){
		//���i���̃t�B�[�h�o�b�N����
		//���E���[�^�̃t�B�[�h�o�b�N

		//���x�ɑ΂���P����
		V_r += 1 * (tar_speed - speed) *SPEED_KP/1.0; //15�ڕW�l�t�߂Ŕ��U
		V_l += 1 * (tar_speed - speed) *SPEED_KP/1.0;
		//���x�ɑ΂���I����
		V_r += 1 * (I_tar_speed - I_speed) *SPEED_KI/1.0; //(0.4-0.3)*0.1 -> 0.01 
		V_l += 1 * (I_tar_speed - I_speed) *SPEED_KI/1.0;
		//���x�ɑ΂���D����
		V_r -= 1 * (p_speed - speed) *SPEED_KD/1.0; //(0.4-0.3)*0.1 -> 0.01 
		V_l -= 1 * (p_speed - speed) *SPEED_KD/1.0;

		//�p���x�ɑ΂���P����
		V_r += 1 * (tar_ang_vel - ang_vel) *(OMEGA_KP/100.0);
		V_l -= 1 * (tar_ang_vel - ang_vel) *(OMEGA_KP/100.0);
		//�p���x�ɑ΂���I����
		V_r += 1 * (I_tar_ang_vel - I_ang_vel) *(OMEGA_KI/100.0); //(0.4-0.3)*0.1 -> 0.01 
		V_l -= 1 * (I_tar_ang_vel - I_ang_vel) *(OMEGA_KI/100.0);
		//�p���x�ɑ΂���D����
		V_r -= 1 * (p_ang_vel - ang_vel) *(OMEGA_KD/100.0); //(0.4-0.3)*0.1 -> 0.01 
		V_l += 1 * (p_ang_vel - ang_vel) *(OMEGA_KD/100.0);

	}else if(run_mode == NON_CON_MODE){
		//�������Ȃ�
		nop();
	}else{
		//�������Ȃ�
		nop();	
	}
	
	/*****************************************************************************************
	�o�͓d�����烂�[�^�̉�]����������
		�d���͐�Βl����͂��邽�߁A
		���̒l�̎��̓��[�^�𐳓]�ݒ�ɂ��āA�d���͂��̂܂�
		���̒l�̎��̓��[�^���t�]�ݒ�ɂ��āA�d���𔽓]����
	*****************************************************************************************/
	if(run_mode != TEST_MODE){
		//�E���[�^�̏o�͓d�������̏ꍇ
		if(V_r > 0){
			//���[�^�𐳓]�ɐݒ�
			MOTOR_RIGHT_DIR_OUT = MOT_R_FORWARD;	//�E���[�^�𐳓]�ɐݒ�
			//V_r = V_r;			//�d���͐��Ȃ̂ł��̂܂�
		}else{
		//�E���[�^�̏o�͓d�������̏ꍇ
			MOTOR_RIGHT_DIR_OUT  = MOT_R_BACK;	//�E���[�^���t��]�ɐݒ�
			V_r = -V_r;			//�d���𐳂̒l�֔��]
		}
		
		//�����[�^�̏o�͓d�������̏ꍇ
		if(V_l > 0){
			//���[�^�𐳓]�ɐݒ�
			MOTOR_LEFT_DIR_OUT = MOT_L_FORWARD;	//�����[�^�𐳓]�ɐݒ�
			//V_l = V_l;			//�d���͐��Ȃ̂ł��̂܂�
		}else{
		//�����[�^�̏o�͓d�������̏ꍇ
			MOTOR_LEFT_DIR_OUT  = MOT_L_BACK;	//�����[�^���t��]�ɐݒ�
			V_l = -V_l;			//�d���𐳂̒l�֔��]
		}
	}	
	/*****************************************************************************************
	�o�͓d���̐���
		���[�^�ւ̏o�͓d���̏����2V�ɐ���
	*****************************************************************************************/
	//�E���[�^�Ɉ������d����2.0V�𒴂����狭���I��2.0V�ɕύX
	if(V_r > 2.0){
		V_r = 2.0;
	}
	//�����[�^�Ɉ������d����2.0V�𒴂����狭���I��2.0V�ɕύX
	if(V_l > 2.0){
		V_l = 2.0;
	}
	
	/*****************************************************************************************
	���[�^�֏o��
		
	*****************************************************************************************/
	
	//�o�b�e���[�d������f���[�e�B���v�Z
	Duty_r = V_r/V_bat;
	Duty_l = V_l/V_bat;
	
	//���[�^��PWM���o��
	if(run_mode != TEST_MODE){
		MOT_OUT_R =(short)(240.0 * Duty_r);
		MOT_OUT_L =(short)(240.0 * Duty_l);
	}
	
	/*****************************************************************************************
	�^�C�}�̃J�E���g
		
	*****************************************************************************************/
	timer++;
	cnt++;
	
}

//4kHz : ���[�e�[�V�������Ȃ���Z���T�̒l��ǂ�(1ms����)
void int_cmt1(void)
{
	/*****************************************************************************************
	A/D�ϊ�
		�Z���T�ƃo�b�e���[�d���擾
	*****************************************************************************************/
	static int state = 0;	//�ǂݍ��ރZ���T�̃��[�e�[�V�����Ǘ��p�ϐ�
	int i;
	
	switch(state)
	{
		case 0:		//�E�Z���T�ǂݍ���

			//�����t�B���^
			S12AD.ADANS0.BIT.ANS0=0x0004;			//AN002
			ADC_START=1;				//AD�ϊ��J�n
			while(ADC_START);			//AD�ϊ��I���܂ő҂�
			sen_r.d_value = DATA_SENSOR_R;			//���l��ۑ�
			
			LED_RIGHT_OUT = 1;					//LED�_��
			for(i = 0; i < WAITLOOP_SLED; i++);		//�t�H�g�g�����W�X�^�̉����҂����[�v
			S12AD.ADANS0.BIT.ANS0=0x0004;			//AN002
			ADC_START=1;				//AD�ϊ��J�n
			while(ADC_START);			//AD�ϊ��I���܂ő҂�
			LED_RIGHT_OUT = 0;					//LED����
			
			sen_r.value = (DATA_SENSOR_R - sen_r.d_value);	//�l��ۑ�

			if(sen_r.value > sen_r.th_wall)			//�ǂ̗L���𔻒f
			{
				sen_r.is_wall = true;			//�E�ǂ���
			}
			else
			{
				sen_r.is_wall = false;			//�E�ǂȂ�
			}
			
			if(sen_r.value > sen_r.th_control)		//����������邩�ۂ��𔻒f
			{
				sen_r.error = sen_r.value - sen_r.ref;	//�����������ꍇ�͕΍����v�Z
				sen_r.is_control = true;		//�E�Z���T�𐧌�Ɏg��
			}
			else
			{
				sen_r.error = 0;				//����Ɏg��Ȃ��ꍇ�͕΍���0�ɂ��Ă���
				sen_r.is_control = false;		//�E�Z���T�𐧌�Ɏg��Ȃ�
			}			
			break;


		case 1:		//�O���Z���T�ǂݍ���

			//�����t�B���^
			S12AD.ADANS0.BIT.ANS0=0x0001;			//AN000
			ADC_START=1;				//AD�ϊ��J�n
			while(ADC_START);			//AD�ϊ��I���܂ő҂�
			sen_fl.d_value = DATA_SENSOR_FL;			//�l��ۑ�
		
			//LED�_����������
			LED_LEFT_FORWARD_OUT = 1;					//LED�_��
			for(i = 0; i < WAITLOOP_SLED; i++);		//�t�H�g�g�����W�X�^�̉����҂����[�v
			S12AD.ADANS0.BIT.ANS0=0x0001;			//AN000
			ADC_START=1;				//AD�ϊ��J�n
			while(ADC_START);			//AD�ϊ��I���܂ő҂�
			LED_LEFT_FORWARD_OUT = 0;					//LED����

			sen_fl.value = (DATA_SENSOR_FL - sen_fl.d_value);	//�l��ۑ�

			if(sen_fl.value > sen_fl.th_wall)		//臒l�ɂ��ǂ̗L���𔻒f
			{
				sen_fl.is_wall = true;			//���O�ǂ���
			}
			else
			{
				sen_fl.is_wall = false;			//���O�ǂȂ�
			}
			break;


		case 2:		//�O�E�Z���T�ǂݍ���
		
			//�����t�B���^
			S12AD.ADANS0.BIT.ANS0=0x0040;			//AN006
			ADC_START=1;				//AD�ϊ��J�n
			while(ADC_START);			//AD�ϊ��I���܂ő҂�
			sen_fr.d_value = DATA_SENSOR_FR;			//�l��ۑ�

			//LED�_����������
			LED_RIGHT_FORWARD_OUT = 1;					//LED�_��
			for(i = 0; i < WAITLOOP_SLED; i++);		//�t�H�g�g�����W�X�^�̉����҂����[�v
			S12AD.ADANS0.BIT.ANS0=0x0040;			//AN006
			ADC_START=1;				//AD�ϊ��J�n
			while(ADC_START);			//AD�ϊ��I���܂ő҂�
			LED_RIGHT_FORWARD_OUT = 0;					//LED����
			
			sen_fr.value = (DATA_SENSOR_FR - sen_fr.d_value);	//�l��ۑ�

			if(sen_fr.value > sen_fr.th_wall)		//�ǂ̗L���𔻒f
			{
				sen_fr.is_wall = true;			//�E�O�ǂ���
			}
			else
			{
				sen_fr.is_wall = false;			//�E�O�ǂȂ�
			}			
			break;


		case 3:		//���Z���T�ǂݍ���
		
			//�����t�B���^
			S12AD.ADANS0.BIT.ANS0=0x0002;			//AN001
			ADC_START=1;				//AD�ϊ��J�n
			while(ADC_START);			//AD�ϊ��I���܂ő҂�
			sen_l.d_value = DATA_SENSOR_L;			//�l��ۑ�

			//LED�_����������
			LED_LEFT_OUT = 1;					//LED�_��
			for(i = 0; i < WAITLOOP_SLED; i++)	;	//�t�H�g�g�����W�X�^�̉����҂����[�v
			S12AD.ADANS0.BIT.ANS0=0x0002;			//AN001
			ADC_START=1;				//AD�ϊ��J�n
			while(ADC_START);			//AD�ϊ��I���܂ő҂�
			LED_LEFT_OUT = 0;					//LED����
			
			sen_l.value = (DATA_SENSOR_L - sen_l.d_value);	//�l��ۑ�
			
			if(sen_l.value > sen_l.th_wall)			//�ǂ̗L���𔻒f
			{
				sen_l.is_wall = true;			//���ǂ���
			}
			else
			{
				sen_l.is_wall = false;			//���ǂȂ�
			}
			
			if(sen_l.value > sen_l.th_control)		//����������邩�ۂ��𔻒f
			{
				sen_l.error = sen_l.value - sen_l.ref;	//�����������ꍇ�͕΍����v�Z����
				sen_l.is_control = true;		//���Z���T�𐧌�Ɏg��
			}
			else
			{
				sen_l.error = 0;			//����Ɏg��Ȃ��ꍇ�͕΍���0�ɂ��Ă���
				sen_l.is_control = false;		//���Z���T�𐧌�Ɏg��Ȃ�
			}

			break;
	}
	
	state++;		//4�񂲂ƂɌJ��Ԃ�
	if(state > 3)
	{
		state = 0;
	}
	
	S12AD.ADANS0.BIT.ANS0=0x0200;			//AN009
	ADC_START=1;				//AD�ϊ��J�n
	while(ADC_START);			//AD�ϊ��I���܂ő҂�
	V_bat = VATT_CALC();

	if(V_bat < 3.3){
		
		//���[�^�~�߂�
		Duty_r = 0;
		Duty_l = 0;
		MOTOR_EN_OUT = 0;	//PC6(SLEEP�s��)
		
		//�u�U�[�炵������
		while(1){
			BEEP(50,440,1);
		}
	}
}

/*-------------------------------------------------------------------------
���� : 2kHz
���E�̃G���R�[�_��data��1ms���ƂɎ擾 2ms���ƂɃG���R�[�_�̒l������
�W���C���Z���T��data��2ms���ƂɎ擾 ��������
-------------------------------------------------------------------------*/
void int_cmt2(void)
{
	static unsigned int	enc_data_r;	//�G���R�[�_�̐��f�[�^
	static unsigned int	enc_data_l;	//�G���R�[�_�̐��f�[�^ 
	static short	state;
	/*****************************************************************************************
	�G���R�[�_�֘A
		�l�̎擾 ���x�X�V �����ϕ��Ȃ�
	*****************************************************************************************/	
	if(state == 0){
		ENC_SS = 	ENC_RIGHT_SS;	//SSL�M���A�T�[�g�ݒ�(SSL0���g��)
		preprocess_spi_enc(0x1400);	//Read Angle
		enc_data_r = Get_enc_data();
		state = 1;
	}else{
		ENC_SS = 	ENC_LEFT_SS;	//SSL�M���A�T�[�g�ݒ�(SSL2���g��)
		preprocess_spi_enc(0x1300);	//Read Angle
		enc_data_l = Get_enc_data();
		
		//���E�G���R�[�_����p�x�擾
		//4096�ň��](360deg = 0deg)
		locate_r = enc_data_r;
		locate_l = enc_data_l;
		
		//�E�G���R�[�_�̌��݂̈ʒu��,1msec�O�̈ʒu�Ƃ̍������v�Z
		//�P�ʎ��ԁi1msec�j������̕ψʗʂ��v�Z
		diff_pulse_r = (locate_r - before_locate_r);
		//�ω��_��1023����0//�ֈړ������Ƃ��̕␳
		if((diff_pulse_r > ENC_RES_HALF || diff_pulse_r < -ENC_RES_HALF) && before_locate_r >ENC_RES_HALF){
			diff_pulse_r = (((ENC_RES_MAX - 1) - before_locate_r) + locate_r);
		}
		//�ω��_��0����1023�ֈړ������Ƃ��̕␳
		else if((diff_pulse_r > ENC_RES_HALF || diff_pulse_r < -ENC_RES_HALF) && before_locate_r <=ENC_RES_HALF){
			diff_pulse_r = 1*(before_locate_r + ((ENC_RES_MAX - 1) - locate_r));
		}
		
		//���G���R�[�_�̌��݂̈ʒu��,1msec�O�̈ʒu�Ƃ̍������v�Z
		//�P�ʎ��ԁi1msec�j������̕ψʗʂ��v�Z
		diff_pulse_l = (-locate_l + before_locate_l);
		//�ω��_��1023����0//�ֈړ������Ƃ��̕␳
		if((diff_pulse_l > ENC_RES_HALF || diff_pulse_l < -ENC_RES_HALF) && before_locate_l >ENC_RES_HALF){
			diff_pulse_l = 1*(((ENC_RES_MAX - 1) - before_locate_l) + locate_l);
		}
		//�ω��_��0����1023�ֈړ������Ƃ��̕␳
		else if((diff_pulse_l > ENC_RES_HALF || diff_pulse_l < -ENC_RES_HALF) && before_locate_l <=ENC_RES_HALF){
			diff_pulse_l = (before_locate_l + ((ENC_RES_MAX - 1) - locate_l));
		}
				
		//���ݑ��x���Z�o
		speed_new_r = (float)((float)diff_pulse_r * (float)MMPP * 1000/1000); //�P�ʂ�[m/s]�ɕϊ�����
		speed_new_l = (float)((float)diff_pulse_l * (float)MMPP * 1000/1000);
		
		//�ߋ��̒l��ۑ�
		speed_old_r= speed_r;
		speed_old_l= speed_l;
		
		//���x�̃��[�p�X�t�B���^
		speed_r = speed_new_r * 0.1 + speed_old_r * 0.9;
		speed_l = speed_new_l * 0.1 + speed_old_l * 0.9;
		
		p_speed = speed;
		//�ԑ̑��x���v�Z
		speed = ((speed_r + speed_l)/2.0);
		
		//I�����̃I�[�o�[�t���[�ƃA���_�[�t���[�΍�
		I_speed += speed;
		if(I_speed >30*10000000000){
			I_speed = 30*10000000000;
		}else if(I_speed < -1*10000000000){
			I_speed = -1*10000000000;
		}

		//�����̌v�Z
		len_mouse += (speed_new_r + speed_new_l)/2.0;
		
		//�ߋ��̒l��ۑ�
		before_locate_r = locate_r;
		before_locate_l = locate_l;
				
		state = 0;
	}
	
	
	/*****************************************************************************************
	�W���C���֘A(���[��)
		�l�̎擾 �p�x�̐ϕ� 
	*****************************************************************************************/
	if(state == 1){
		//�W���C���Z���T�̒l�̍X�V
		preprocess_spi_gyro_2byte(0xB70000);
		
		//LowPass Filter
		gyro_x_new = (float)((short)(read_gyro_data() & 0x0000FFFF));
		gyro_x = (gyro_x_new -gyro_ref );
		
		//�p���x�̍X�V
		p_ang_vel = ang_vel;
		ang_vel = ((2000.0*(gyro_x)/32767.0))*PI/180.0;	//�W���C���̒l��degree�ɕϊ�������Arad�ɕϊ�
		//�ϕ��l�̍X�V
		I_ang_vel += ang_vel;
		if(I_ang_vel >30*10000000000){
			I_ang_vel = 30*10000000000;
		}else if(I_ang_vel < -1*10000000000){
			I_ang_vel = -1*10000000000;
		}
		
		//�W���C���̒l���p�x�ɕϊ�
		degree += (2.0*(gyro_x_new - gyro_ref)/32767.0);
		
	}	
	
}

