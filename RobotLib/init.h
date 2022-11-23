#ifndef INIT_H_
#define INIT_H_

//�N���b�N��CMT�̏�����
void clk_init(void);
void CMT_init(void);

//�u�U�[�̏����� MTU2���g�p
void buzzer_init(void);

void motor_init(void);

void init_adc(void);
void infrared_sensor_init(void);    //�ԊO���Z���T�[�̏�����

//���H���̏�����
void init_maze(void);

// �W���C���̃��t�@�����X�擾
void gyro_get_ref(void);

/*****************************************************************************************
���Z���T�[�n�̃p�����[�^������
���t�@�����X�Ƃ��ǂ�臒l�Ƃ�
*****************************************************************************************/
void init_parameters(void);

void init_all(void);

#endif