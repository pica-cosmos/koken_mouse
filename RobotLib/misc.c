#include "../iodefine.h"
#include "Parameters/static_parameters.h"

#define CMT3_CYCLE_PER_MS   375         //1ms�����艽��J�E���g����邩 48(MHz)/PCLK(1/128)/1000(ms)
#define CMT3_CNT_MAX        65536       //�J�E���g�ł���ő�̐�

volatile int cmt3_compa_times = 0;              // CMT1�R���y�A�}�b�`���̃t���O�p

//�x�����[�`�� �����Ƀ~���b�����w�肷��
//CMT3 PCLK:1/128
void wait_ms(int miribyou){

    int interrupt_cycles = miribyou * CMT3_CYCLE_PER_MS;
    volatile int max_interrupt_times = interrupt_cycles / CMT3_CNT_MAX;
    int remained_interrupt_cycles = (interrupt_cycles - (CMT3_CNT_MAX * max_interrupt_times));
    
    //�w�肳�ꂽ�񐔕������A�J�E���^���ő�l�܂ŃJ�E���g���āA���Ԃ��҂�
    cmt3_compa_times = 0;

    CMT.CMSTR1.BIT.STR3 = 0;    //�J�E���g��~
    CMT3.CMCNT = 0;             //�J�E���^���N���A
    CMT3.CMCOR = CMT3_CNT_MAX -1;
    CMT3.CMCR.BIT.CMIE = 1;     //�R���y�A�}�b�`���荞�݋���
    CMT.CMSTR1.BIT.STR3 = 1;    //�����݋���

    while(cmt3_compa_times < max_interrupt_times)
        ;

    //��x�����A�c�����T�C�N���̐������J�E���g���āA���Ԃ��҂�
    cmt3_compa_times = 0;

    CMT.CMSTR1.BIT.STR3 = 0;    //�J�E���g��~
    CMT3.CMCNT = 0;             //�J�E���^���N���A
    CMT3.CMCOR = remained_interrupt_cycles -1;
    CMT3.CMCR.BIT.CMIE = 1;     //�R���y�A�}�b�`���荞�݋���
    CMT.CMSTR1.BIT.STR3 = 1;    //�����݋���

    while(cmt3_compa_times == 0)
        ;

    //�����ݒ�~
    CMT.CMSTR1.BIT.STR3 = 0;    //�J�E���g��~
    CMT3.CMCR.BIT.CMIE = 0;  //�����݋֎~
    CMT3.CMCNT = 0;          //�J�E���^���N���A
}