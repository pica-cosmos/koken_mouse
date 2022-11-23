/*��TXI,TEI�����݂ɂ��Ă��������ڂ�����������*/
#include "sci.h"
#include <machine.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../iodefine.h"
#include "../Definations/system_definations.h"

#pragma inline(SCI_send_check)

static char sci1_buff[BUFF_SIZE];	//TX�p�̕�������i�[���邽�߂̔z��B�z��̃T�C�Y��BUFF_SIZE�𒴂���ƃo�b�t�@�I�[�o�[�̃��X�N������
static int sci1_length;		//sci1_buff�̒������i�[
static int sci1_point;		//�v�f���J�E���^

unsigned char get_buff;
static unsigned char get_buff_m[BUFF_SIZE];
static char get_buff_2[BUFF_SIZE];

static int receve_data_num, string_num;

// #define SCI_ERROR
#ifdef SCI_ERROR
void sci_error(void){

}
#endif

void init_sci(long baud){

	system_protect_allow();	//�v���e�N�g����
    MSTP(SCI1) = 0;
	system_protect_prohibit();

	SCI1.SCR.BYTE = 0x00;
	while (0x00 != (SCI1.SCR.BYTE & 0xF0));	//���荞�ݗv�����֎~�����܂ő҂�
	PORT2.PODR.BIT.B6 = 1;			//TXD��Dirction�̐؂�ւ���̒l��high

	//PORT26��TX , PORT30��RX�ɐݒ�
	PORT2.PDR.BIT.B6 = 1;			//�o�͂ɐݒ�
	PORT3.PDR.BIT.B0 = 0;			//���͂ɐݒ�
	PORT2.PMR.BIT.B6 = 0;			//�ėp�|�[�g�ɐݒ�
	PORT3.PMR.BIT.B0 = 0;			//�ėp�|�[�g�ɐݒ�

	PmnPFS_allow();
	MPC.P26PFS.BIT.PSEL = 0x0A;		//TXD1
	MPC.P30PFS.BIT.PSEL = 0x0A;		//RXD1
	PmnPFS_prohibit();

	PORT2.PMR.BIT.B6 = 1;	//TXD1 �@�\�|�[�g�ɐݒ�
	PORT3.PMR.BIT.B0 = 1;	//RXD1 �@�\�|�[�g�ɐݒ�

	//�V���A���R�~���j�P�[�V�����C���^�[�t�F�C�X���[�h SCMR.SMIF = 1
	SCI1.SCR.BIT.CKE = 0;
	SCI1.SMR.BYTE = 0x00;			//1stopbit parity�Ȃ� 8bit ��������
	SCI1.SCMR.BYTE = 0xF2;			//S=32clock
	SCI1.SEMR.BYTE = 0x10;			//8�T�C�N��
	// SCI1.BRR = (int)(((long)48000000 / (32*1/2*baud)) - 1); //@48MHz �ʐM���x���v�Z����
	SCI1.BRR = 25;
	SCI1.SSR.BYTE &= 0xC7;			// ORER,PER,FER���N���A����

	//����M�����݂�ݒ�A�����ݒ�ł́AIEN�Ŋ����݂��֎~����B�D�揇�ʂ͍Œ�
	SCI1.SCR.BYTE =	0x00;			//����M�ł̊��荞�ݓ�����֎~(�����݂��I���ɂȂ�킯�ł͂Ȃ�)
	IEN(SCI1, TXI1) = 1;
	IEN(SCI1, TEI1) = 1;
	IPR(SCI1, TXI1) = 1;			//�D�揇�ʂ��Œ�ɂ���
	IPR(SCI1, TEI1) = 1;
	IR(SCI1, TXI1) = 0;
	IR(SCI1, TEI1) = 0;

}

int SCI_printf(const char *format, ...){

	va_list arg;

	if(SCI1.SSR.BIT.TEND == 0) return 0;

	va_start(arg, format);
	sci1_length = vsprintf(sci1_buff, format, arg);
	va_end(arg);

	if(sci1_length < 1) return 0;
	sci1_point = 0;

	SCI1.SCR.BIT.TE = 1;	//�V���A�����M���싖��
	SCI1.SCR.BIT.TIE = 1;	//TXI ���荞�ݗv��������
	
	SCI1.TDR = sci1_buff[sci1_point++];
	return 1;
}


void SCI_gets( volatile unsigned char *p){

	volatile long n;
	
	SCI1.SCR.BIT.RE = 1;
	SCI1.SCR.BIT.RIE = 1;
	
    for( n=0 ;  (get_buff_2[n]!='\0')&&(n<=sizeof(get_buff_2)); n++){	//get_buff_2�̒l��NULL�ł͂Ȃ��ꍇ
        *(p+n) = get_buff_2[n];     //�z��̃G�C���A�Xp�ɒl����
    }
}

void SCI_clear(void){

	//��ʃN���A�V�[�P���X
	SCI_printf("\x1b[2J");
	while(SCI_send_check());
	SCI_printf("\x1b[0;0H");
}

void int_sci1_txi(void){
	
	if (sci1_point == sci1_length)
	{
		SCI1.SCR.BIT.TIE = 0;
		SCI1.SCR.BIT.TEIE = 1;
	}else{
		while (SCI1.SSR.BIT.TEND == 0)	//data�̑��M����wait
			;
		SCI1.TDR = sci1_buff[sci1_point++];
	}
	
}

void int_sci_tei(void){
	
	SCI1.SCR.BIT.TE = 0;	//TX�֎~
	SCI1.SCR.BIT.TIE = 0;	//TXI�����݋֎~
	SCI1.SCR.BIT.TEIE = 0;	//TEI�����݋֎~
}

//intprg.c�ŌĂяo��
void int_scr1_rxi(void){

    int n;
    get_buff_m[receve_data_num++] = SCI1.RDR;

    if(receve_data_num >= sizeof(get_buff_m)-1 )
		receve_data_num=0;  // -1������̂́A������̍Ō�ɋ󕶎�(NULL)�����邽�߁B�z��̓Y�����z��̗v�f���ɂȂ����ꍇ
    
	switch(SCI1.RDR){
        case ESC_KEY:

            receve_data_num = 0;

            for( n=0; n<=sizeof(get_buff_m); n++ ){		//get_buff_m��������
                get_buff_m[n]='\0';
            }for( n=0; n<=sizeof(get_buff_2); n++ ){	//get_buff_2��������
                get_buff_2[n]='\0';
            }
            break;
        case ENTER_KEY:

            string_num = receve_data_num;
            receve_data_num = 0;

            for( n=0; n<=sizeof(get_buff_2); n++ ){	//get_buff_2��������
                get_buff_2[n]='\0';
            }
            for( n=0; n<string_num-1 ; n++ ){	//get_buff_m��get_buff_2�ɃR�s�[
                get_buff_2[n]=get_buff_m[n];
            }
            get_buff_2[string_num]='\0';	//get_buff_2�̖�����NULL��ǉ�
            for(n=0 ;n<sizeof(get_buff_m) ;n++){	//get_buff_m��������
                get_buff_m[n]='\0';
            }
            break;
		default :
			break;
    }
}

int int_scr1_eri(void){

    unsigned char er=1;
    if(SCI1.SSR.BIT.PER == 1){
        SCI1.SSR.BIT.PER=0;///�p���e�B�G���[�t���O
        er= 2;
    }
    if(SCI1.SSR.BIT.FER == 1){
        SCI1.SSR.BIT.FER=0;///�t���[���G���[�t���O
        er= 3;
    }
    if(SCI1.SSR.BIT.ORER == 1){
        SCI1.SSR.BIT.ORER=0;//�I�[�o�����G���[�t���O
        er = 4;
    }
    return er;
}

int SCI_send_check(void){
	//�g�����X�~�b�g�G���h�t���O
	if (SCI1.SSR.BIT.TEND == 0)
	{
		return 1;	//�f�[�^���M��
	}else{
		return 0;	//�f�[�^���M����
	}
}