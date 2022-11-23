#include "i2c.h"
#include "../../iodefine.h"
#include "../Definations/system_definations.h"
/*--------------
���W�X�^�̐���
ICCR1 : i2c�o�X�R���g���[�����W�X�^(p.1487)
--IICRST : 1��i2c�������Z�b�g
--ICE : i2c����(1),i2c�֎~(0)

ICCR2 : I2C�o�X�R���g���[�����W�X�^
--i2c�R���f�B�V�����������ݗp

ICSR2 : i2c�o�X�X�e�[�^�X���W�X�^(p.1507)
--�X�e�[�^�X���o�p
--�������ݎ� 0���������ނ��ƂŃN���A�ł���
--�ǂݎ�莞 1:�t���O���o

ICIER : i2c�o�X�C���^���v�g�C�l�[�u�����W�X�^(p.1502)
--�e��ݒ�����邱�ƂŁAi2c�C���^���v�g�������鎖���o����

ICDRR : I2C�o�X��M�f�[�^���W�X�^ 
--��M�f�[�^���i�[�����
ICDRT : I2C�o�X���M�f�[�^���W�X�^
--���M�f�[�^���i�[����

ICMR3 : �e�탂�[�h���W�X�^,�S��write (p.1496)
--ACKBR : ��M�A�N�m���b�W�r�b�g
--ACKBT : ���M�A�N�m���b�W�r�b�g
--ACKBP : ACKBT���C�g�v���e�N�g�r�b�g
--WAIT : 1��9�N���b�N�ڂ�1�N���b�N�ڂ̊Ԃ�Low�Ƀz�[���h,1byte���Ƃ̓�����\�ɂ���B
�f�[�^����M�������ꍇ�́AICDRR���W�X�^�𒀈�ǂޕK�v������

[�A�h���X��v���o�@�\] manual p.1538~
ICSER : �X���[�u�A�h���X�X�e�[�^�X���W�X�^(p.1500)
--SARyE�r�b�g��1�ɂ���ƁASARL/U���W�X�^�Őݒ肵���X���[�u�A�h���X���r�ł���
--ICRXI,ICTXI�����݂������ł���
SARL/U : �X���[�u�A�h���X�ݒ背�W�X�^(p.1511,2)
ICSR1 : I2C�o�X�X�e�[�^�X���W�X�^1(p.1504)
--SARL/U�Őݒ肵���X���[�u�A�h���X���o�t���O(AASy)

[���g���Ɠ]�����x�̐ݒ�(p.1515)]
ICMR1,bit0~2,CKS & ICBRL & ICBRH ��3�̗v�f��g�ݍ��킹�ē]�����x������
ICMR1 : bitcounter=9 PCLK/4 400kbps -> 28, 100kbps -> 38
ICBRL : �]�����x=1/{[ICBRH+1)+(ICBRL+1)]/IIC��}	400kbps -> F0, 100kbps -> FD
ICBRH : IIC��=PCLKx10^6�~������ / Tr=0�̏ꍇ��400kHz	400kbps -> E7, 100kbps -> F8

������̐ݒ�
PCLK~=50�Ȃ̂ŁA�\36-5,400kbps�̗����Q�l�ɂ���
----------------*/

unsigned char IOex_SWITCH(void)
{
	unsigned char who;
	I2C_START();
	I2C_PUT((0x18) << 1); //�A�h���X write
	I2C_PUT(0x00);		  //PWR_MGMT_2 INPUT
	I2C_RESTART();
	I2C_PUT((0x18 << 1) | 0x01); //�A�h���X read

	while (RIIC2.ICSR2.BIT.RDRF == 0)	//��M�f�[�^������܂őҋ@
		;
	if (RIIC2.ICSR2.BIT.NACKF)	//NACK�����o���ꂽ��X�g�b�v�R���f�B�V�������o�t���O�����Z�b�g
	{
		RIIC2.ICSR2.BIT.STOP = 0;
	}

	RIIC2.ICMR3.BIT.WAIT = 1;	//ICDRR���ǂ݂������܂�CLK��low�Ƀz�[���h�i1byte���Ƃ̓ǂݏo��������\�ɂ���j
	RIIC2.ICMR3.BIT.ACKBT = 1;	//�}�X�^�[����NACK�𑗐M
	RIIC2.ICDRR;	//dummy

	while (RIIC2.ICSR2.BIT.RDRF == 0)
		;
	who = RIIC2.ICDRR;	//�f�[�^�̓ǂݏo��

	while (RIIC2.ICSR2.BIT.RDRF == 0)
		;
	RIIC2.ICSR2.BIT.STOP = 0;	//�t���O���N���A
	RIIC2.ICCR2.BIT.SP = 1;		//�X�g�b�v�R���f�B�V�����𔭍s
	RIIC2.ICDRR; // dummy
	RIIC2.ICMR3.BIT.WAIT = 0;

	while (RIIC2.ICSR2.BIT.STOP == 0)
		;
	RIIC2.ICSR2.BIT.NACKF = 0;	//�t���O��������
	RIIC2.ICSR2.BIT.STOP = 0;

	return who;
}

void IOex_LED(unsigned char led_num)
{
	//�|�[�����O�ɂ��ݒ�
	I2C_START();			 // Startbit����
	I2C_PUT((0x18 << 1));	 //�A�h���X write
	I2C_PUT(0x01);			 // PWR_MGMT_2 OUTOUT
	I2C_PUT(0x00 | led_num); // sleep����
	I2C_STOP();
}

void IOex_init(void)
{
	I2C_START();		  // Startbit����
	I2C_PUT((0x18 << 1)); //�A�h���X write
	I2C_PUT(0x03);		  // PWR_MGMT_2 init
	I2C_PUT(0xF0);		  // sleep����
	I2C_STOP();
}

void I2C_init(void)
{

	// I2C�̃C�j�V�����C�Y
	PORT1.PCR.BIT.B6 = 0; //���̓v���A�b�v����
	PORT1.PDR.BIT.B6 = 0; // Input
	PORT1.PCR.BIT.B7 = 0; //���̓v���A�b�v����
	PORT1.PDR.BIT.B7 = 0; // Input

	// // I2C�s���̃��[�h��������
	// PORT1.PMR.BIT.B6 = 0; // SCL �ėp�|�[�g
	// PORT1.PMR.BIT.B7 = 0; // SDA �ėp�|�[�g
	// MPC.PWPR.BIT.B0WI = 0;
	// MPC.PWPR.BIT.PFSWE = 1;
	// MPC.P16PFS.BIT.PSEL = 0; //�ėp�|�[�g
	// MPC.P17PFS.BIT.PSEL = 0; //�ėp�|�[�g
	// MPC.PWPR.BIT.PFSWE = 0x80;

	
	// I2C�p�̃s���Ƃ��Đݒ�
	PmnPFS_allow();
	MPC.P16PFS.BIT.PSEL = 0x0F; // SCL2-DS
	MPC.P17PFS.BIT.PSEL = 0x0F; // SDA2-DS
	PmnPFS_prohibit();

	//�ėp�|�[�g����@�\�|�[�g�֐؂�ւ�
	PORT1.PMR.BIT.B6 = 1;
	PORT1.PMR.BIT.B7 = 1;

	//�V�X�e���v���e�N�g������ i2c��L����
	system_protect_allow();
	MSTP(RIIC2) = 0;
	system_protect_prohibit();

	// RIIC2�S���W�X�^�����Z�b�g IICRST=1 & ICE=0
	RIIC2.ICCR1.BIT.ICE = 0;	// SCLn SDAn�[�q��쓮���
	RIIC2.ICCR1.BIT.IICRST = 1; // IICRST�A�ݒ�p�S���W�X�^�����Z�b�g
	
	//�p���[�I�����̖\���΍�Ŋ��荞�݋֎~�ɐݒ�
	IEN(RIIC2, EEI2) = 0; //�����݃x�N�^���֎~
	IEN(RIIC2, RXI2) = 0;
	IEN(RIIC2, TXI2) = 0;
	IEN(RIIC2, TEI2) = 0;
	IPR(RIIC2, EEI2) = 10; //���荞�ݏ���
	IPR(RIIC2, RXI2) = 10;
	IPR(RIIC2, TXI2) = 10;
	IPR(RIIC2, TEI2) = 10;
	IR(RIIC2, EEI2) = 0;
	IR(RIIC2, RXI2) = 0; //�X�e�[�^�X�t���O��clear
	IR(RIIC2, TXI2) = 0;
	IR(RIIC2, TEI2) = 0;
	
	RIIC2.ICSER.BYTE = 0x00;	// �X���[�u�A�h���X��v���o�@�\�𖳌���(SARL,SARU)

	//���g���Ɠ]�����x�̐ݒ� �]�����x:400kbps
	RIIC2.ICMR1.BYTE = 0x28; 
	RIIC2.ICBRL.BYTE = 0xF0; 
	RIIC2.ICBRH.BYTE = 0xE7; 

	//���̑��l�X�Ȑݒ� �^�C���A�E�g�@�\�͎g��Ȃ��i��X...�j
	RIIC2.ICMR2.BYTE = 0x00;
	RIIC2.ICMR3.BYTE = 0x00;
	RIIC2.ICMR3.BIT.ACKWP = 1; // ACKBT(�A�N�m���b�W)�̏������݋���
	RIIC2.ICFER.BYTE = 0x72;

	//i2c�̎��s�ݒ�
	RIIC2.ICIER.BYTE = 0x00; 	//�S�����݂��֎~
	//	RIIC2.ICIER.BYTE =0xfc;//���M�f�[�^�G���v�e�B�A���M�I�����荞�� ��M�f�[�^�t�����荞�݁ANACK��M���荞�݁A�X�g�b�v�R���f�B�V�������o���荞�݁A�X�^�[�g�R���f�B�V�������o���荞��
	RIIC2.ICCR1.BIT.IICRST = 0;	//i2c���Z�b�g��Ԃ�����
	RIIC2.ICCR1.BIT.ICE = 0; // ICE ����I2C�]������֎~
}

void I2C_START(void)
{
	RIIC2.ICCR1.BIT.ICE = 1; // ICE ����I2C�]�����싖��
	while (RIIC2.ICCR2.BIT.BBSY == 1) // 1:�o�X�r�W�[ 0:�o�X�t���[ �p�X���t���[�ɂȂ�܂ő҂�
		;
	RIIC2.ICSR2.BIT.START = 0; // START�R���f�B�V�������o�t���O���N���A
	RIIC2.ICCR2.BIT.ST = 1;	   //�X�^�[�g�R���f�B�V�������s
}

void I2C_RESTART(void)
{
	RIIC2.ICCR1.BIT.ICE = 1; // ICE ����I2C�]�����싖��
	while (RIIC2.ICSR2.BIT.TDRE == 0)
		; // ICDRR���W�X�^�ɑ��M�f�[�^�������Ȃ�܂őҋ@
	while (RIIC2.ICSR2.BIT.TEND == 0)
		; //�f�[�^�̑��M���I������܂őҋ@
	
	RIIC2.ICSR2.BIT.START = 0; // START�R���f�B�V�������o�t���O���N���A
	RIIC2.ICCR2.BIT.RS = 1;	   //���X�^�[�g�R���f�B�V�����̔��s
	while (RIIC2.ICCR2.BIT.RS)
		; //���X�^�[�g�R���f�B�V���������s�����܂őҋ@
}

void I2C_STOP(void)
{
	while (RIIC2.ICSR2.BIT.TDRE == 0)
		; // ICDRR���W�X�^�ɑ��M�f�[�^�������Ȃ�܂őҋ@
	while (RIIC2.ICSR2.BIT.TEND == 0)
		; //�f�[�^�̑��M���I������܂őҋ@

	RIIC2.ICSR2.BIT.STOP = 0;	//���o�t���O���N���A
	RIIC2.ICCR2.BIT.SP = 1; //�X�g�b�v�R���f�B�V�����̔��s

	while (RIIC2.ICSR2.BIT.STOP == 0)
		;	//�X�g�b�v�R���f�B�V���������s�����܂őҋ@

	RIIC2.ICSR2.BIT.NACKF = 0; //�R���f�B�V�������o�p���W�X�^���N���A
	RIIC2.ICSR2.BIT.STOP = 0;
	RIIC2.ICCR1.BIT.ICE = 0; // ICE ����I2C�]������֎~
}

void I2C_PUT(unsigned char aaa)
{
	if (RIIC2.ICSR2.BIT.NACKF == 1)	// NACK���o�Ȃ�stop�R���f�B�V�����𖢌��o�ɂ���
	{
		RIIC2.ICSR2.BIT.STOP = 0;
	}
	while (RIIC2.ICSR2.BIT.TDRE == 0)	// ICDRR���W�X�^�ɑ��M�f�[�^�������Ȃ�܂őҋ@
		;
	RIIC2.ICDRT = aaa;
}

#ifdef IIC_INTERRUPT
// ===================================���荞��===================================
// �ʐM�G���[/�C�x���g�����ɂ�銄�荞��
// AL    AL=1 & ALIE=1
// NACKF NACKF=1 & NAKIE=1
// TMOF  TMOF=1 & TMOIE=1
// START START=1 & STIE=1
// STOP  STOP=1 & SPIE=1
void int_i2c_ee(void)
{
	/* Check stop condition detection */
	if ((RIIC2.ICSR2.BIT.STOP != 0) && (RIIC2.ICIER.BIT.SPIE != 0))
	{
		/* Clear each status flag */
		RIIC2.ICSR2.BIT.NACKF = 0;
		RIIC2.ICSR2.BIT.STOP = 0;
		/* Enable/Disable each interrupt */
		RIIC2.ICIER.BYTE = 0xBB; // TEIE��STIE�̊��荞�݂��֎~

		/* Initialize ram for RIIC */
		iic_buff.trm_cnt = 0; /* Clear the internal transmission counter for IIC */
		iic_buff.rcv_cnt = 0; /* Clear the internal reception counter for IIC */
	}

	/* Check NACK reception */
	if ((RIIC2.ICSR2.BIT.NACKF != 0) && (RIIC2.ICIER.BIT.NAKIE != 0))
	{
		RIIC2.ICIER.BIT.NAKIE = 0;
		/* Generate Stop Condition */
		RIIC2.ICSR2.BIT.STOP = 0;
		RIIC2.ICCR2.BIT.SP = 1;
	}

	/* Check start condition detection restart*/
	if ((RIIC2.ICSR2.BIT.START != 0) && (RIIC2.ICIER.BIT.STIE != 0))
	{
		/* Disable Start Condition Detection Interrupt */
		RIIC2.ICSR2.BIT.START = 0;
		RIIC2.ICIER.BIT.STIE = 0;
		RIIC2.ICIER.BIT.TIE = 1;
		/* Transfer slave device address */
		if ((iic_buff.trm_cnt == 0) && (iic_buff.rcv_cnt == 0))
		{										  //�ŏ��̃X�^�[�g�R���f�B�V����
			RIIC2.ICDRT = (iic_buff.SlvAdr << 1); /* When master transfer, b0 must be '1' */
		}
		else
		{
			RIIC2.ICDRT = (iic_buff.SlvAdr << 1) | 0x01; /* When master transfer, b0 must be '1' */
		}
	}
}

void int_i2c_rx(void)
{
	volatile unsigned char tmp;

	/* Increase internal reception counter on RAM. It is a receive data counter. */
	iic_buff.rcv_cnt++;

	if (iic_buff.rcv_cnt == iic_buff.RWCnt)
	{
		RIIC2.ICMR3.BIT.WAIT = 1;
		RIIC2.ICMR3.BIT.ACKBT = 1;
	}

	if (iic_buff.rcv_cnt == 1)
	{
		/* dummy read */
		tmp = RIIC2.ICDRR;
	}
	else if (iic_buff.rcv_cnt == (iic_buff.RWCnt + 1))
	{ //�f�[�^��M�����
		if (iic_buff.SlvAdr == 0x08)
		{
			if (iic_buff.RA == 0x04)
			{
				iic_data.Touch_Data = RIIC2.ICDRR;
			}
			else if (iic_buff.RA == 0x02)
			{
				iic_data.TCNT.BYTE.TCNT_H = RIIC2.ICDRR;
			}
			else if (iic_buff.RA == 0x03)
			{
				iic_data.TCNT.BYTE.TCNT_L = RIIC2.ICDRR;
			}
		}
		RIIC2.ICSR2.BIT.STOP = 0;
		RIIC2.ICCR2.BIT.SP = 1;
		RIIC2.ICMR3.BIT.WAIT = 0;
	}
	else
	{
		if (iic_buff.SlvAdr == 0x08)
		{
			iic_data.TCNT.BYTE.TCNT_H = RIIC2.ICDRR;
		}
	}

	//
	//	}else if(iic_buff.RWCnt == 2){
	//		RIIC2.ICMR3.BIT.WAIT = 1;
	//		RIIC2.ICMR3.BIT.ACKBT = 1;
	//		*iic_buff.pRWData++ = RIIC2.ICDRR;
	//		iic_buff.RWCnt--;
	//	}else if(iic_buff.RWCnt == 1){
	//		RIIC2.ICSR2.BIT.STOP = 0;
	//		RIIC2.ICCR2.BIT.SP = 1;
	//		*iic_buff.pRWData++ = RIIC2.ICDRR;	/* Read final data */
	//		iic_buff.RWCnt--;
	//		RIIC2.ICMR3.BIT.WAIT = 0;
	//		tmp2=(I2C_BUF[1]<<8) & 0xff00;
	//		tmp3=I2C_BUF[0] & 0x00ff;
	//		if(GYRO_ACCE_F==0){
	//			GYRO16=( tmp2 | tmp3) >> 4;
	//		}else{
	//			ACCE16=( tmp2 | tmp3) >> 4;
	//		}
	//
	//	}else{
	//		*iic_buff.pRWData++ = RIIC0.ICDRR;
	//		iic_buff.RWCnt--;
	//	}
}

//���M�f�[�^ �G���v�e�B
// TDRE=1 & TIE=1
void int_i2c_tx(void)
{
	/* Increase internal transmission counter */
	iic_buff.trm_cnt++;
	if (iic_buff.trm_cnt == 1)
	{
		/* Transfer slave device register address */
		RIIC2.ICDRT = iic_buff.RA;
		if (iic_buff.RW == 1)
		{							  // read �̂��߃��X�^�[�g�̏���
			RIIC2.ICIER.BIT.TEIE = 1; /* Enable Transmit End Interrupt */
		}
	}
	else if (iic_buff.trm_cnt == 2)
	{
		if (iic_buff.RW == 0)
		{
			RIIC2.ICDRT = iic_buff.write_buf[0];
		}
		//		if(GYRO_ACCE_F==0){
		//			RIIC2.ICDRT = 0xac;//out_z_L ??read �ŏ�ʃA�h���X��1�̏ꍇ�A�A�h���X��auto incr
		//		}else{
		//			RIIC2.ICDRT = 0xaa;//out_y_L ??read
		//		}
	}
	else if (iic_buff.trm_cnt == (iic_buff.RWCnt + 2))
	{ //�������݃f�[�^�I���
		if (iic_buff.RW == 0)
		{
			RIIC2.ICIER.BIT.TEIE = 1; /* Enable Transmit End Interrupt */
		}
	}
}

void int_i2c_te(void)
{
	if (iic_buff.RW == 0)
	{ // write�̂�
		RIIC2.ICIER.BIT.SPIE = 1;
		RIIC2.ICSR2.BIT.STOP = 0; // STOP�R���f�B�V��������
		RIIC2.ICCR2.BIT.SP = 1;
	}
	else
	{							   // read
		RIIC2.ICSR2.BIT.START = 0; /* Clear Start Condition Detection flag */
		RIIC2.ICIER.BIT.STIE = 1;  /* Enable Start Condition Detection Interrupt */
		RIIC2.ICIER.BIT.TEIE = 0;  /* Disable Transmit End Interrupt */
		/* Generate restart condition */
		RIIC2.ICCR2.BIT.RS = 1;
	}
}

#endif
