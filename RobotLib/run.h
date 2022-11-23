#ifndef RUN_H_
#define RUN_H_

/*---------------------------------------------------------------
���i�p�̊֐��B�t�H�A�����B�������x�����^��
len : �ړ�����[mm], acc : �����x[m/ss], max_sp : �ō����x[m/s], end_sp : �I�[���x[m/s]
---------------------------------------------------------------*/
void straight(float len, float acc, float max_sp, float end_sp);

/*--------------------------------------------------------------
��]�p�̊֐��B�t�H�A�����B�ԑ̖̂ڕW���x(speed)�͈��B���p�����x��]�^��
deg : �ړ��p�x[degree], ang_acc : �p�����x[rad/ss], max_om : �ō��p���x[rad/s], dir : ��]����(0:right, 1:left)
---------------------------------------------------------------*/
void turn(int deg, float ang_acc, float max_om, short dir);

#endif
