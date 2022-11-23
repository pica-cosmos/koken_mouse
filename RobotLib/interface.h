#ifndef INTERFACE_H_
#define INTERFACE_H_

#define FREQUENCY_MAX	10000
#define FREQUENCY_MIN	200
#define MTU_CNT_MAX 	65536

//��buzzer��duty������������Ɖ��ʂ��傫���Ȃ�

/*-------------------------------
IOex��LED�����点��֐� 16�i����led_num�ɑ��
0b 	0  0  0  0
	D6 D5 D4 D3 
led_num�ɒl������ƁA���̏���LED���_������̂ŁA�l��ǂ�
���点�����r�b�g��1�ɂ���
--------------------------------*/
void LED(unsigned char led_num);

/*---------------------------------
buzzer:PB5 / MTU2,MTIOC2A
PB5 1:low / 0:high
�u�U�[�̃I��/�I�t �����P�Ƀf���[�e�B�[��, �����Q�Ɏ��g����ݒ�
----------------------------------*/
void buzzer_on(int duty_percent, int circuit_hz);
void buzzer_off(void);

/*------------------------------------------------------------------------
�����P�Ƀf���[�e�B�[��A�����Q�Ɏ��g���A�����R�Ƀu�U�[��炷�񐔂��w��
100ms�Ԋu�Ńu�U�[��炷
------------------------------------------------------------------------*/
void BEEP(int duty_percent, int circuit_hz, int count);

/*----------------------------------------------
���[�h�ύX�̃v���O�����B���݂̃��[�h�������ɑ��
mode�̍ō��l��mode_max, mode�̍Œ�l��mode_min�ɑ������
�E�̃^�C���̃X�s�[�h����:mode++
�E�̃^�C���̃X�s�[�h����:mode--
-----------------------------------------------*/
void mode_change( char* mode, char mode_min, char mode_max);

/*----------------------------------------------
�������[�h�́A���[�h�ύX�p�v���O���� mode : 1~7
mode 1 : �p�����[�^�[��UART�ŕ\��
mode 2 : ����O�i
mode 3 : 90�x�E�ɐ���
mode 4~7 : �󂫃��[�h

SW2�������ƁAmode1���甲���o��
SW3�������ƁA�������[�h���甲���o��
-----------------------------------------------*/
void adjust_mode_select(void);

#endif