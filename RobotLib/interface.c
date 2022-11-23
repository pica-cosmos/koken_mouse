#include "../iodefine.h"
#include "interface.h"
#include "Hardwares/sci.h"
#include "Hardwares/i2c.h"
#include "glob_var.h"
#include "Parameters/static_parameters.h"
#include "Parameters/adjust_parameters.h"
#include "Definations/system_definations.h"
#include "Definations/macro.h"

extern void wait_ms(int miribyou);

void LED(unsigned char led_num){
	//LED�̓v���A�b�v����Ă���̂ŁA0�œ_�� 1�ŏ���
    unsigned char led_pt = 0x00;
    if((led_num & 0x01) == 0){	//led_num�̎w��̃r�b�g��0�ł����LED��off
		led_pt |= 0x02;	
	}
	if((led_num & 0x02) == 0){
		led_pt |= 0x01;	
	}
	if((led_num & 0x04) == 0){
		led_pt |= 0x08;
	}
	if((led_num & 0x08) == 0){
		led_pt |= 0x04;
	}
	
	if((led_num & 0x0F) == 0x00){	
		led_pt = 0x0F;
	}
    IOex_LED(led_pt);
}

void buzzer_on(int duty_percent, int circuit_hz){
	int circuit,duty;

	//����A������ݒ�
	if (duty_percent <= 0)
		duty_percent = 1;
	if (duty_percent >= 100)
		duty_percent = 99;
	if (circuit_hz > FREQUENCY_MAX)
		circuit_hz = FREQUENCY_MAX;
	if (circuit_hz < FREQUENCY_MIN)
		circuit_hz = FREQUENCY_MIN;
	
    circuit = (int)(12*1000*1000 / circuit_hz);	//������ӂ�̃N���b�N�������߂� PCLK:12MHz
    duty = (int)(circuit * (float)duty_percent / 100.0);
	
    MTU.TSTR.BIT.CST2 = 0;	//MTU2�֎~
    MTU2.TCNT = 0;
    MTU2.TGRA = (unsigned short)duty-1;      //�f���[�e�B�[��ݒ�
    MTU2.TGRB = (unsigned short)circuit-1;   //���g���̐ݒ�
    MTU.TSTR.BIT.CST2 = 1; // MTU2����
}

void buzzer_off(void){

    MTU.TSTR.BIT.CST2 = 0; // MTU2��~
    MTU2.TCNT = 0;         //�J�E���^���N���A

}

void BEEP(int duty_percent, int circuit_hz, int count){
	
	volatile int i;
	for (i = 0; i < count; i++)
	{
		buzzer_on(duty_percent,circuit_hz);
		wait_ms(100);
		buzzer_off();
		wait_ms(100);
	}
	buzzer_off();
}

void mode_change( char* mode, char mode_min, char mode_max){
	int i;
	if(speed_r > 0.1){
		if(*mode == mode_max){
			*mode = mode_min;
		}else{
			*mode= *mode + 1;
		}
		BEEP(50,1500,1);
		for(i = 0; i < 100*1000*50; i++);
	}

	if(speed_r < -0.1){
		if(*mode == mode_min){
			*mode = mode_max;
		}else{
			*mode = *mode -1;
		}
		BEEP(50,500,1);
		for(i = 0; i < 100*1000*50; i++);
	}
	LED(*mode);
	wait_ms(50);	//�듮��h�~
}


void adjust_mode_select(void)
{
	char ad_mode = 1;
	unsigned char push_switch = IOex_SWITCH();

	while(1){

		switch(ad_mode){

			case 1:	
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	X	X	X	*
				*					*
				*****************************************/
				//�p�����[�^�[��UART�ŕ\��
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
					//�ǐ����L���ɂ���
					con_wall.enable = true;
					while(1){
						//A/D sensor
						SCI_printf("sen_r.value: %d\n\r",sen_r.value);
						SCI_printf("sen_l.value: %d\n\r",sen_l.value);
						SCI_printf("sen_fr.value: %d\n\r",sen_fr.value);
						SCI_printf("sen_fl.value: %d\n\r",sen_fl.value);
						SCI_printf("V_bat: %d\n\r",(int)(V_bat*1000));
						SCI_printf("sen_r.th_wall: %d\n\r",sen_r.th_wall);
						SCI_printf("sen_l.th_wall: %d\n\r",sen_l.th_wall);
						SCI_printf("sen_fr.th_wall: %d\n\r",sen_fr.th_wall);
						SCI_printf("sen_fl.th_wall: %d\n\r",sen_fl.th_wall);
						SCI_printf("con_wall.omega: %d\n\r",(int)(con_wall.omega*1000));
						SCI_printf("speed_r: %d\n\r", (int)(speed_r*100));
						SCI_printf("speed_l: %d\n\r", (int)(speed_l*100));
						//gyro
						SCI_printf("degree: %d\n\r",(int)degree*10);;			
						SCI_printf("gyro: %d\n\r", (int)(ang_vel*1000) );
						//encoder
						SCI_printf("locate_r: %d\n\r",locate_r);
						SCI_printf("locate_l: %d\n\r",locate_l);	
					
						//switch
						SCI_printf("switch2: %d\n\r",SWITCH_2(push_switch));
						SCI_printf("switch3: %d\n\r",SWITCH_3(push_switch));
						SCI_printf("switch4: %d\n\r",SWITCH_4(push_switch));
						wait_ms(100);
						//��ʃN���A�V�[�P���X
						SCI_printf("\x1b[2J");				//�N���A�X�N���[��[CLS]
						SCI_printf("\x1b[0;0H");			//�J�[�\����0,0�Ɉړ�
						
						if(SWITCH_2(push_switch) == 1){
							BEEP(50,440,1);
							break;	
						}
					}
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
				//���敪�O�i
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
					gyro_get_ref();
					BEEP(50,440,1);
					len_mouse = 0;
					straight(SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
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
				//90�x�E�ɐ���
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
					gyro_get_ref();
					BEEP(50,440,1);
					turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
					MOTOR_EN_OUT = 0;
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
			case 5:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	X	O	X	*
				*					*
				*****************************************/
			case 6:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	X	O	O	X	*
				*					*
				*****************************************/
			case 7:
				/****************************************
				*MODE LED STATE				*
				*					*
				*	D3	D4	D5	D6	*
				*	O	O	O	X	*
				*					*
				*****************************************/
				
				//�Z���T�[�̑O�Ɏ���������ăX�^�[�g
				if(sen_fr.value + sen_fl.value + sen_r.value + sen_l.value > SEN_DECISION * 4){
					BEEP(50,440,1);
				}
				break;

			//mode0~7�ȊO�̏ꍇ�B�������Ȃ��B
			default:
				break;
			
		}
		//���[�h�؂�ւ��p����
		mode_change(&ad_mode,1,7);
		
		//�v�b�V���X�C�b�`�̒l���X�V����
		push_switch = IOex_SWITCH();
		MOTOR_EN_OUT = 0;
		
		if(SWITCH_3(push_switch) == 1){
			BEEP(50,100,5);
			break;
		}
	}
}
