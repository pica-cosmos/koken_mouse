#ifndef SCI_H_
#define SCI_H_

/*-----------------------------
[Tera Term �̐ݒ�]
speed:115200bps(�C��)       ���T���v���v���O�����ł�38400
���s�R�[�h R:auto, M:LF
����-��M/���M : SJIS
���P�[�� : Japanese
[�ڑ��ݒ�]
mode:�V���A�� / port:COM14
------------------------------*/

#define BUFF_SIZE 512

#define ESC_KEY     0x1b 
#define ENTER_KEY   0x0a

//SCI�̏�����
void init_sci(long baud);

//��SCI_printf�ł͊����݂��g���Ă��邪�A���̊����݂��\�����ʃg���u���𐶂މ\�������邽�߁Aprintf�̌�ɕK��send_check������
int SCI_printf(const char *format, ...);
void SCI_gets( volatile unsigned char *p);
//��ʃN���A�p�֐�
void SCI_clear(void);

//���M��������
void int_sci1_txi(void);
//���M����������
void int_sci_tei(void);
void int_scr1_rxi(void);
int int_scr1_eri(void);

//SCI�����M���ł���΂P,���M���Ŗ�����΂O��Ԃ�
int SCI_send_check(void);
void sci_wait_send_check(void);
#endif
