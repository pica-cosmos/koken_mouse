#ifndef I2C_H_
#define I2C_H_

// #define IIC_INTERRUPT
// #define IIC_STRUCT

/***********************************************
<I2C�̐���>

Start     Address R/W ACK  DATA ACK DATA ACK STOP
Condition 1-7     8    9   1-8  9  1-8   9   Condition

Write Sequence
Master S AD+W     RA      DATA     DATA     P
Slave         ACK    ACK       ACK      ACK

Read Sequence
Master S AD+W     RA     S AD+R          ACK       NACK P
Slave         ACK    ACK        ACK DATA      DATA

S:  StartCondition
AD: Address
ACK:Acknowledge
RA: Register Address
P:  StopCondition
NACK:not ackowlege

R/W
0:Write
1:Read


<IO�G�L�X�p���_(PCA9557PW)�̐���>
slave address : 0x18

INPUT/OUTPUT (control code)
0x00 : INPUT IO�G�L�X�p���_�͓��͗p
0x01 : OUTPUT : IO�G�L�X�p���_�͏o�͗p

LED [ 1:���� / 0:�_�� (OUTPUT) ]
D3 : BIT1
D4 : BIT0
D5 : BIT3
D6 : BIT2

SWITCH [ 0:������Ă��Ȃ� / 1:������Ă��� (INPUT) ]
SW2 : BIT6
SW3 : BIT5
SW4 : BIT4
************************************************/

/*I2C����M�p�̊֐� �����݂͋֎~*/
/*----------------------------- */
void I2C_init(void);
void I2C_START(void);
void I2C_RESTART(void);
void I2C_STOP(void);
void I2C_PUT(unsigned char aaa);


/*IO�G�L�X�p���_�p�̊֐�*/
/*---------------------*/
//IO�G�L�X�p���_�̏�����
void IOex_init(void);

//�X�C�b�`�̒l��16�i���Ŏ�M���A�l��Ԃ��imanual p:1525)
unsigned char IOex_SWITCH(void);

//LED�����点��֐� �����ɂ�16�i���Œl���w��
void IOex_LED(unsigned char led_num);

/*�����ݗp�̊֐������A���̃v���O�����ł͎g�p���Ă��Ȃ�*/
/*-----------------------------------------------*/
#ifdef IIC_INTERRUPT
void int_i2c_ee(void);
void int_i2c_rx(void);
void int_i2c_tx(void);
void int_i2c_te(void);
#endif

//���Ɏg����\���̂Ȃ̂��͕s��
#ifdef IIC_STRUCT
struct str_IIC_data
{
	union
	{
		unsigned short WORD;
		struct
		{
			unsigned char GYRO_Z_H;
			unsigned char GYRO_Z_L;
		} BYTE;
	} GYRO_Z;
	union
	{
		unsigned short WORD;
		struct
		{
			unsigned char ACC_X_H;
			unsigned char ACC_X_L;
		} BYTE;
	} ACC_X;
	union
	{
		unsigned short WORD;
		struct
		{
			unsigned char ACC_Y_H;
			unsigned char ACC_Y_L;
		} BYTE;
	} ACC_Y;
	union
	{
		unsigned short WORD;
		struct
		{
			unsigned char TCNT_H;
			unsigned char TCNT_L;
		} BYTE;
	} TCNT;
	unsigned char Touch_Data;
	unsigned char LED_Data;
	union
	{
		unsigned short WORD;
		struct
		{
			unsigned char TCNT_TH_H;
			unsigned char TCNT_TH_L;
		} BYTE;
	} TCNT_TH;
} iic_data;

struct str_IIC
{
	unsigned char SlvAdr;		 /* Slave Address, Don't set bit0. It's a Read/Write bit */
	unsigned char RW;			 /* read:1 write:0 */
	unsigned char RA;			 /*Register Address*/
								 //	unsigned char	PreCnt;		/* Number of Predata */
								 //	unsigned char	read_buf[10];	/* read data buffer */
	unsigned char RWCnt;		 /* Number of Data */
	unsigned char write_buf[10]; /* write data buffer */
	unsigned char trm_cnt;		 /*transmission counter*/
	unsigned char rcv_cnt;		 /*reception counter*/
} iic_buff;
#endif

#endif