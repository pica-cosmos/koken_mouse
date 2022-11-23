#ifndef MACRO_H_
#define MACRO_H_

#include "system_definations.h"

#define CONV_SEN2WALL(w) ((w) ? WALL : NOWALL)			//�Z���T��񂩂�Ǐ��֕ϊ�
#define VATT_CALC() (2.0*3.3*(float)(DATA_VOLTAGE/4095.0))
/*-------------------------------------------------------
�e�X�C�b�`�ɂ��āA������Ă��邩�ǂ����̗L���𒲂ׂ�}�N��
1:�������Ă��� 0:������Ă��Ȃ�
(�ڂ�����i2c.h���Q��)
--------------------------------------------------------*/

#define SWITCH_2(sw2) ((sw2 & (1<<6)) >> 6) //�X�C�b�`�Q�̏�Ԃ𒲂ׂ�
#define SWITCH_3(sw3) ((sw3 & (1<<5)) >> 5) //�X�C�b�`�R�̏�Ԃ𒲂ׂ�
#define SWITCH_4(sw4) ((sw4 & (1<<4)) >> 4) //�X�C�b�`�S�̏�Ԃ𒲂ׂ�

#endif