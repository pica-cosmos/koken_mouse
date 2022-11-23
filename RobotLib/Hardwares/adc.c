#include "adc.h"
#include <machine.h>
#include "../../iodefine.h"
#include "../Definations/system_definations.h"
#include "../Definations/original_typedef.h"

wall_sensor infrared_sensor_adc(void){
	
	unsigned long i = 0;
	wall_sensor sensor_value;

	// S12AD.ADANS0.WORD = (1<<0) | (1<<1) | (1<<2) | (1<<6);
	S12AD.ADANS0.WORD = 0x0047;	//A/D�ϊ���AN000,1,2,6�̂݋�����

	//�ԊO��LED��high�ɐݒ�
	LED_LEFT_OUT = 1;
	LED_LEFT_FORWARD_OUT = 1;
	LED_RIGHT_OUT = 1;
	LED_RIGHT_FORWARD_OUT = 1;

	for(i = 0; i < 100*1000/40; i++); //40��100usec �d���̕ϓ���h�~

	ADC_START = 1;	//A/D�ϊ����J�n
	while (ADC_START);	//A/D�ϊ����I���܂ő҂�

	//�ԊO��LED��low�ɐݒ�
	LED_LEFT_OUT = 0;
	LED_LEFT_FORWARD_OUT = 0;
	LED_RIGHT_OUT = 0;
	LED_RIGHT_FORWARD_OUT = 0;

	sensor_value.sen_fr = DATA_SENSOR_FR;
	sensor_value.sen_r = DATA_SENSOR_R;
	sensor_value.sen_fl = DATA_SENSOR_FL;
	sensor_value.sen_l = DATA_SENSOR_L;

	return sensor_value;
}

int voltage_adc(void){

	int volt;

	S12AD.ADANS0.WORD = (1<<9);	//A/D�ϊ���AN009�̂݋�����
	ADC_START = 1;
	while(ADC_START);

	volt = DATA_VOLTAGE;
	return volt;
}