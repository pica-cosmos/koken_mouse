#ifndef INTERRUPT_H_
#define INTERRUPT_H_

/*----------------------------------
<CMT�ɂ��Ă̐���>
CMT0�͐��䊄�荞�ݗp�^�C�}
CMT1��AD�ϊ��ł̃Z���T�ǂݍ��ݗp�^�C�}
CMT2�̓G���R�[�_�֘A �l�̎擾/���x�X�V/�����ϕ��ȂǗp�^�C�}
CMT3�͒x�����[�`���p�^�C�}

CMT1�̕�����128 : 0.375MHZ
0~65535�܂ł̒l(16bit)�ŃJ�E���g�����
CMCOR��max�̂Ƃ��F174.76ms / cycle
------------------------------------*/

// //CMT0���J�n / ��~
// void CMT0_START(float interrupt_intv_ms, void (*handler)(void));
// void CMT0_STOP(void);

void timer_start(void);
void timer_stop(void);

void int_cmt0(void);
void int_cmt1(void);
void int_cmt2(void);
#endif