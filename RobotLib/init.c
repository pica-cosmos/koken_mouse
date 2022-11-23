#include "init.h"
#include <machine.h>
#include "../iodefine.h"
#include "Definations/system_definations.h"
#include "Parameters/static_parameters.h"
#include "Parameters/adjust_parameters.h"
#include "interrupt.h"
#include "interface.h"
#include "Hardwares/i2c.h"
#include "Hardwares/sci.h"
#include "Hardwares/spi.h"
#include "glob_var.h"

extern void wait_ms(int miribyou);


void init_all(void){

	volatile long i;
	clrpsw_i();	//�����݂��֎~	

	init_clk();		//�e���{�@�\���ɏ���������
	init_I2C();
	init_sci(115200);

	init_motor();
	init_wallsensor();
	init_buzzer();
	init_IOex();
	init_spi_enc();
	init_spi_gyro();
	init_maze();
	init_parameters();
	LED(0);
	
	//Gyro�����ݒ�
	preprocess_spi_gyro_2byte(0x0681);		//�W���C�����Z�b�g
	for(i = 0; i < 100*1000*10; i++);
	preprocess_spi_gyro_2byte(0x0601);		//Low Power Mode OFF
	for(i = 0; i < 100*1000*10; i++);

	//�W���C���̐ݒ�
	preprocess_spi_gyro_2byte(0x7F20);		//User Bank2�ɕύX
	for(i = 0; i < 100*1000*10; i++);
	preprocess_spi_gyro_2byte(0x0107);		//Range ���ő�2000dps�֕ύX
	for(i = 0; i < 100*1000*10; i++);
	preprocess_spi_gyro_2byte(0x7F00);		//User Bank0�ɕύX
	for(i = 0; i < 100*1000*10; i++);

	preprocess_spi_gyro_2byte(0x0621);		//�W���C���X�^�[�g
	//�ϐ�������
	timer = 0;

	init_CMT();
	setpsw_i();		//�����݋���
	timer_start();
}

//�N���b�N�ݒ�
void init_clk(void){

    SYSTEM.PRCR.WORD = 0xa50b; //�N���b�N�\�[�X�I���̕ی�̉���

    SYSTEM.PLLCR.WORD = 0x0F00; /* PLL ���{�~16 ����1���� (12.000MHz * 16 = 192MHz)*/
                                // SYSTEM.PLLWTCR.BYTE = 0x0F;	    // 4194304cycle(Default) PLL�����肵�����g���ɂȂ�܂�wait �������Ő��䂳���̂ŕK�v�Ȃ�
    SYSTEM.PLLCR2.BYTE = 0x00;  // PLLEN : enable / PLL enable

    // ICK   : 192/2 = 96MHz 		// �V�X�e���N���b�N CPU DMAC DTC ROM RAM
    // PCLKA : 192/2 = 96MHz 		// ���Ӄ��W���[���N���b�NA ETHERC�AEDMAC�ADEU
    // PCLKB : 192/4 = 48MHz 		// ���Ӄ��W���[���N���b�NB ��L�ȊO PCLKB=PCLK

    // SYSTEM.SCKCR.BIT.FCK=0x02;		//FCLK MAX 50MHz  192/4
    // SYSTEM.SCKCR.BIT.ICK=0x01;		//ICLK MAX 100MHz 192/2
    // SYSTEM.SCKCR.BIT.PSTOP1=0x01;		//BCLK �o�͒�~
    // SYSTEM.SCKCR.BIT.PSTOP0=0x01;		//SDCLK �o�͒�~
    // SYSTEM.SCKCR.BIT.BCK=0x02;		//BCLK MAX 100MHz ICLK�ȉ��ɂ���K�v������192/4
    // SYSTEM.SCKCR.BIT.PCKA=0x01;		//PCLKA MAX 100MHz 192/2
    // SYSTEM.SCKCR.BIT.PCKB=0x02;		//PCLKB MAX 50MHz 192/4
    // ��L�̐ݒ�ł͐�����clock�ݒ肪�ł��Ȃ����߉��L�̂悤�Ɉꊇ�Őݒ肷�邱��

    SYSTEM.SCKCR.LONG = 0x21C21211; // FCK1/4 ICK1/2 BCLK��~ SDCLK��~ BCK1/4 PCLKA1/2 PCLKB1/4

    // SYSTEM.SCKCR2.BIT.UCK=0x03;		//UCLK MAX 48MHz 192/4
    // SYSTEM.SCKCR2.BIT.IEBCK=0x02;		//IECLK MAX 50MHz 192/4

    SYSTEM.SCKCR2.WORD = 0x0032; /* UCLK1/4 IEBCK1/4 */
    SYSTEM.BCKCR.BYTE = 0x01;    /* BCLK = 1/2 */

    SYSTEM.SCKCR3.WORD = 0x0400; // PLL��H�I��
}

void init_CMT(void){

    //CMT�̃v���e�N�g������
    system_protect_allow();
		MSTP(CMT0) = 0;
		MSTP(CMT1) = 0;
		MSTP(CMT2) - 0;
		MSTP(CMT3) = 0;
    system_protect_prohibit();

    //�J�E���g��~
    CMT.CMSTR0.BIT.STR0 = 0;
	CMT.CMSTR0.BIT.STR1 = 0;
	CMT.CMSTR1.BIT.STR2 = 0;
	CMT.CMSTR1.BIT.STR3 = 0;

    //CMT0�͐��䊄�荞�ݗp�^�C�}�Ƃ��Ďg�p
	CMT0.CMCR.BIT.CKS=1;	// PCLK/32 1.5MHz
	CMT0.CMCR.BIT.CMIE=1;	//���荞�݂�����
	CMT0.CMCNT=0;			//�J�E���^�[�̃N���A
	CMT0.CMCOR=1500-1;		//1kHz

	IEN(CMT0,CMI0) = 1;		//���荞�ݗv�������� 
	IPR(CMT0,CMI0) = 15;	//���荞�ݗD��x 15���ō�
	IR(CMT0,CMI0)=0;		//���荞�݃X�e�[�^�t���O���N���A
	
	//CMT1�̓Z���T�[����p�^�C�}�Ƃ��Ďg�p
	CMT1.CMCR.BIT.CKS=1;	// PCLK/32 1.5MHz
	CMT1.CMCR.BIT.CMIE=1;	//���荞�݂�����
	CMT1.CMCNT=0;			//�J�E���^�[�̃N���A
	CMT1.CMCOR=(1500/4)-1;	//4kHz

	IEN(CMT1,CMI1) = 1;		//���荞�ݗv�������� 
	IPR(CMT1,CMI1) = 14;	//���荞�ݗD��x�����_�ɐݒ�
	IR(CMT1,CMI1)=0;		//���荞�݃X�e�[�^�t���O���N���A

	//CMT2�̓Z���T�[����p�^�C�}�Ƃ��Ďg�p
	CMT2.CMCR.BIT.CKS=1;	// PCLK/32 1.5MHz
	CMT2.CMCR.BIT.CMIE=1;	//���荞�݂�����
	CMT2.CMCNT=0;			//�J�E���^�[�̃N���A
	CMT2.CMCOR=(1500/2)-1;	//2kHz

	IEN(CMT2,CMI2) = 1;		//���荞�ݗv�������� 
	IPR(CMT2,CMI2) = 13;	//���荞�ݗD��x�����_�ɐݒ�
	IR(CMT2,CMI2)=0;		//���荞�݃X�e�[�^�t���O���N���A
	
    //CMT3�͒x�����[�`���p�^�C�}�Ƃ��Ďg�p
    CMT3.CMCR.BIT.CKS = 2;      // PCLK 1/128 0.375MHZ
    CMT3.CMCR.BIT.CMIE = 1;     //�R���y�A�}�b�`���荞�݋���
    IEN(CMT3, CMI3) = 1;		//���荞�ݗv��������
    IPR(CMT3, CMI3) = 2;        //���荞�ݗD��x���Q�ɐݒ�
    IR(CMT3, CMI3) = 0;			//���荞�݃X�e�[�^�t���O���N���A
}

void init_buzzer(void){

    PORTB.PODR.BIT.B5 = 0;
    PORTB.PDR.BIT.B5 = 1;       // output
    PORTB.PMR.BIT.B5 = 0;

    PmnPFS_allow();
    MPC.PB5PFS.BIT.PSEL = 1;
    PmnPFS_prohibit();
    PORTB.PMR.BIT.B5 = 1;       // PWM�p�@�\�s���ɐݒ�

    system_protect_allow();
    MSTP(MTU) = 0;
    system_protect_prohibit();
	
    MTU.TSTR.BIT.CST2 = 0; // MTU2�J�E���g��~
    MTU2.TCR.BIT.CCLR = 2; // TGRB�̃R���y�A�}�b�`��TCNT�N���A
    MTU2.TCR.BIT.TPSC = 1; // TCLK/4 12MHZ
    MTU2.TMDR.BIT.MD = 2;  // mode : PWM1
    MTU2.TIOR.BIT.IOA = 2; //����low �R���y�A�}�b�`high
    MTU2.TIOR.BIT.IOB = 1; //����low �R���y�A�}�b�`low
    MTU2.TCNT = 0;         //�J�E���^���N���A
}

//���[�^�n���W�X�^�̐ݒ�
void init_motor(void){

	MOTOR_EN_OUT = 0;	//�\��motor ��SLEEP�ɂ���B���[�^�[���n�C�C���s�[�_���X�Ō듮�삷��̂�h��

	//���[�^�[�n�s�����o�͂�
	PORTC.PDR.BIT.B6 = 1;//motor SLEEP (STBY)
	PORTC.PDR.BIT.B5 = 1;//Rmotor PH
	PORTB.PDR.BIT.B3 = 1;//Rmotor EN
	PORTC.PDR.BIT.B4 = 1;//Lmotor PH
	PORTB.PDR.BIT.B1 = 1;//Lmotor EN
	
	//�@�\�s���ݒ�	
	PmnPFS_allow();			//PFSWE�ւ̏������݋���ݒ�
	MPC.PB1PFS.BIT.PSEL=1;	//PWM R MTIOC0C	PB1�����荞�݃s���Ƃ��Đݒ�
	MPC.PB3PFS.BIT.PSEL=1;	//PWM L MTIOC0A	PB3�����荞�݃s���Ƃ��Đݒ�
	PmnPFS_prohibit();		//PFSWE�ւ̏������݋֎~��ݒ�
	
	//PB1,3���@�\�|�[�g�ɐݒ�
	PORTB.PMR.BIT.B3=1;
	PORTB.PMR.BIT.B1=1;

	//MTU�̃C�j�V�����C�Y
	SYSTEM.PRCR.WORD = 0xA502;	//�v���e�N�g����
	MSTP(MTU) = 0;				// MTU (MTU0?MTU5)�̃X�g�b�v��ԉ���
	SYSTEM.PRCR.WORD = 0xA500;	//�v���e�N�g�ݒ�
	
	//�s����@�\�ݒ莞�ɂ̓^�C�}�X�g�b�v
	MTU.TSTR.BIT.CST0=0;	//MTU0�̃J�E���g��~
	
	//���E���[�^�pMTU0 PWM2 ���萔��=L/R=17uH/(1.07+0.5+0.3)=110kHz
	MTU0.TCR.BIT.TPSC=0;	//�v���X�P�[���\ PCLK/1 48MHz
	MTU0.TCR.BIT.CCLR=6;	//PWM TGRD�̃R���y�A�}�b�`��TCNT(�J�E���^)�N���A

	MTU0.TMDR.BIT.MD=3;		//PWM2��I���B������PWM�������\

	//���荞�ݎ���PWM�̓����ݒ�BIOA/IOC : PWM / IOD : ����
	MTU0.TIORH.BIT.IOA=5;	//�����o��0�R���y�A�}�b�`PWM�̗�����0���o��	PB3
	MTU0.TIORL.BIT.IOC=5;	//�����o��0�R���y�A�}�b�`PWM�̗�����0���o�� PB1
	MTU0.TIORL.BIT.IOD=2;	//�����o��0�R���y�A�}�b�`PWM�̎��������1���o��

	MTU0.TGRA = 0;			//������ �l��4�ȉ����Ɠ��삵�Ȃ�
	MTU0.TGRC = 0;			//������
	MTU0.TGRD = 240 - 1;	//���� 200kHz �^�C�}�J�E���^�͂O����n�܂�̂� -1 ������

	/* 	�����ݐM�������ƁATGIA0���Z�b�g�B�����ݐ悪���荞�ݗv�����󂯓�����TGIA0���N���A
		TGIA0���g�������ꍇ�́A���̃R�����g�A�E�g�����
	*/

	MTU0.TIER.BIT.TGIEA = 1;	//���荞�ݗv���iTGIA�j�����B�����݃x�N�^�� "Excep_MTU0_TGIA0"
	IEN(MTU0,TGIA0) = 1;		//IER���W�X�^�𑀍삷�鎖�ŁA�����݃R���g���[���[���̊����ݗv����L���ɂ���
	IPR(MTU0,TGIA0) = 12;		//�����݂̗D�揇�ʂ��w��B�D�揇�ʂ���������
	IR(MTU0,TGIA0) = 0;			//IR���W�X�^���N���A���āA�����ݗv���̎󂯓���̐������

	MTU0.TIER.BIT.TGIEC = 1;	//���荞�ݗv��(TGIC)������
	IEN(MTU0,TGIC0) = 1;
	IPR(MTU0,TGIC0) = 12;
	IR(MTU0,TGIC0) = 0;
}

void init_wallsensor(void){

	//ADC�̐ݒ�
	system_protect_allow();		//A/D�R���o�[�^�ɓd�͂�����
	MSTP(S12AD) = 0;
	system_protect_prohibit();

	S12AD.ADCSR.BIT.CKS = 3;	//A/D�ϊ��̃N���b�N��PCLK��1����(48M[Hz])�ɐݒ�
	S12AD.ADCSR.BIT.ADCS = 0;	//�V���O���X�L�������[�h�ɐݒ�
	
    //�ԊOLED�̃s���ݒ�
	PORTA.PDR.BIT.B3 = 1;	//PA3���o�͗p�ɐݒ�
	PORT1.PDR.BIT.B5 = 1;	//P15���o�͗p�ɐݒ�
	PORT1.PDR.BIT.B4 = 1;	//P14���o�͗p�ɐݒ�
	PORT3.PDR.BIT.B1 = 1;	//P31���o�͗p�ɐݒ�

    //A/D�|�[�g��PMR�ݒ�
	PORT4.PMR.BIT.B6=1;	//P46�����Ӌ@�\�|�[�g�ɐݒ�
	PORT4.PMR.BIT.B2=1;	//P46�����Ӌ@�\�|�[�g�ɐݒ�
	PORT4.PMR.BIT.B1=1;	//P46�����Ӌ@�\�|�[�g�ɐݒ�
	PORT4.PMR.BIT.B0=1;	//P46�����Ӌ@�\�|�[�g�ɐݒ�
	PORTE.PMR.BIT.B1=1;	//PE1�����Ӌ@�\�|�[�g�ɐݒ� 
	
    //A/D�|�[�g��PFS�ݒ� �A�i���O�[�q�Ƃ��Ďg�p����
	PmnPFS_allow();		//�v���e�N�g����
	MPC.P46PFS.BIT.ASEL=1;	//A/D SEN_FR	AN006���g�p
	MPC.P42PFS.BIT.ASEL=1;	//A/D SEN_R 	AN002���g�p
	MPC.P41PFS.BIT.ASEL=1;	//A/D SEN_FR	AN001���g�p
	MPC.P40PFS.BIT.ASEL=1;	//A/D SEN_R 	AN000���g�p
	MPC.PE1PFS.BIT.ASEL=1;	//A/D V_BAT		AN009���g�p
	PmnPFS_prohibit();	//�v���e�N�g�ݒ�

}

void init_maze(void)
{
	int i,j;
	
	for(i = 0; i < MAZESIZE_X; i++)
	{
		for(j = 0; j < MAZESIZE_Y; j++)
		{
			wall[i][j].north = wall[i][j].east = wall[i][j].south = wall[i][j].west = UNKNOWN;	//���H�̑S�̂��킩��Ȃ�����ݒ肷��
		}
	}
	
	for(i = 0; i < MAZESIZE_X; i++)
	{
		wall[i][0].south = WALL;		//�l���̕ǂ�ǉ�����(��)
		wall[i][MAZESIZE_Y-1].north = WALL;	//�l���̕ǂ�ǉ�����(�k)
	}
	
	for(j = 0; j < MAZESIZE_Y; j++)
	{
		wall[0][j].west = WALL;			//�l���̕ǂ�ǉ�����(��)
		wall[MAZESIZE_X-1][j].east = WALL;	//�l���̕ǂ�ǉ�����(��)
	}
	
	wall[0][0].east = wall[1][0].west = WALL;	//�X�^�[�g�n�_�̉E�̕ǂ�ǉ�����
	
}

void gyro_get_ref(void){
	long i = 0;
	float gyro_ref_temp = 0;
	gyro_ref = 0;
	//�W���C���̃��t�@�����X�擾
	for(i = 0; i < 2500; i++){
		gyro_ref_temp += (float)gyro_x_new;
		wait_ms(1);
	}
	gyro_ref = (gyro_ref_temp/2500.0);
	degree = 0;
	wait_ms(100);
}

void init_parameters(void)
{
			
	sen_r.ref = REF_SEN_R;				//�E�Z���T�̃��t�@�����X�l��������
	sen_l.ref = REF_SEN_L;				//���Z���T�̃��t�@�����X�l��������
	
	sen_r.th_wall = TH_SEN_R;			//�E�Z���T�̕ǗL�����f��臒l��������
	sen_l.th_wall = TH_SEN_L;			//���Z���T�̕ǗL�����f��臒l��������
	
	sen_fr.th_wall = TH_SEN_FR;			//�E�O�Z���T�̕ǗL�����f��臒l��������
	sen_fl.th_wall = TH_SEN_FL;			//���O�Z���T�̕ǗL�����f��臒l��������
	
	sen_r.th_control = CONTH_SEN_R;			//�E�Z���T�̕ǐ��䂩���邩�ۂ���臒l��������
	sen_l.th_control = CONTH_SEN_L;			//���Z���T�̕ǐ��䂩���邩�ۂ���臒l��������
	
	con_wall.kp = CON_WALL_KP/10000.0;			//�ǔ�ᐧ��̔��萔��������
}