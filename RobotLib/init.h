#ifndef INIT_H_
#define INIT_H_

void init_clk(void);            //�N���b�N�̏�����
void init_CMT(void);            //CMT�̏�����
void init_buzzer(void);         //�u�U�[�̏����� MTU2���g�p
void init_motor(void);          //���[�^�[�̏�����
void init_wallsensor(void);     //�ǃZ���T/�o�b�e���[�`�F�b�N�̏�����
void init_maze(void);           //���H���̏�����
void gyro_get_ref(void);        //�W���C���̃��t�@�����X�擾

/*****************************************************************************************
���Z���T�[�n�̃p�����[�^������
���t�@�����X�Ƃ��ǂ�臒l�Ƃ�
*****************************************************************************************/
void init_parameters(void);

void init_all(void);

#endif