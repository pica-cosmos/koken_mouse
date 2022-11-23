#include "../iodefine.h"
#include "Definations/original_typedef.h"
#include "Hardwares/adc.h"
#include "interface.h"

extern void wait_ms(int miribyou);

void enc_test(void){
    volatile int i = 0;
    volatile int data = 0;
	volatile int sci_tend_flag = 0;
	unsigned char mode = 1;
	init_all();
	
	for ( i = 0; i < 100*100*100; i++);
    while(1){
		wait_ms(100);
		RSPI0.SPCMD0.BIT.SSLA = 	0x00;		//SSL信号アサート設定(SSL2を使う)
		preprocess_spi_enc(0x1400);			//Read Angle
		data = Get_enc_data();			//エンコーダ値取得

		SCI_printf("R_Encdata_10bit,%d\n\r", ((int)(data & 0xFFFF)) & 0x2FFF );	//エンコーダ値表示
		sci_wait_send_check();

		RSPI0.SPCMD0.BIT.SSLA = 	0x02;		//SSL信号アサート設定(SSL0を使う)
		preprocess_spi_enc(0x1300);			//Read Angle
		data = Get_enc_data();			//エンコーダ値取得

		SCI_printf("L_Encdata_10bit,%d\n\r",((int)(data & 0xFFFF)) & 0x2FFF );	//エンコーダ値表示
		sci_wait_send_check();

		wait_ms(100);
		//画面クリアシーケンス
		SCI_clear();
		sci_wait_send_check();

	}
}

void gyro_test(void){
	volatile long i,n;
	volatile long data = 0;
	volatile int sci_tend_flag = 0;
	unsigned char mode = 1;

	init_all();
	for ( i = 0; i < 100*100*100; i++);
	
    preprocess_spi_gyro_2byte(0x062100);
    data = read_gyro_data();
    for(i = 0; i < 100*1000*10; i++);
    
    while(1){
        SCI_printf("data_H,%d\n\r",(data & 0x00FF00) >> 8);
		sci_wait_send_check();
        SCI_printf("data_L,%d\n\r\n\r",(data & 0x0000FF));
		sci_wait_send_check();
        SCI_printf("Ang_Velocity[deg/s],%d\n\r",((int)((2000.0*((float)data))/32767.0)));
		sci_wait_send_check();

        wait_ms(100);

        //画面クリアシーケンス
        SCI_clear();
		sci_wait_send_check();
		preprocess_spi_gyro_2byte(0xB70000);
        data = (short)(read_gyro_data()&0x0000FFFF);
		wait_ms(100);
    }
}

void adc_test(void){
	volatile long i;
	wall_sensor sensor_val;
	volatile int volt;

	init_clk();
	init_wallsensor();
	init_sci(115200);
	init_CMT();
	for(i=0;i<100*100*100;i++);

	for(i=1;i<=100;i++)
	{
		sensor_val = infrared_sensor_adc();
		volt = voltage_adc();
		SCI_printf("NUMBER [%d]\n",i);
		sci_wait_send_check();
		SCI_printf("DATA_SENSOR_FR : %d\n",sensor_val.sen_fr);
		sci_wait_send_check();
		SCI_printf("DATA_SENSOR_R : %d\n",sensor_val.sen_r);
		sci_wait_send_check();
		SCI_printf("DATA_SENSOR_FL : %d\n",sensor_val.sen_fl);
		sci_wait_send_check();
		SCI_printf("DATA_SENSOR_L : %d\n",sensor_val.sen_l);
		sci_wait_send_check();
		SCI_printf("VOLTAGE : %f V\n",2.0*3.3*(float)(volt/4095.0));
		sci_wait_send_check();
		wait_ms(100);
	}
	wait_ms(300);
	SCI_printf("FINISH ADC TEST\n");
	wait_ms(1000);
	SCI_clear();
}

void SCI_test(void){
	volatile long n;
	volatile int i;

	init_clk();
	init_sci(115200);
	init_CMT();
	for(n=0;n<100*100*100;n++);
	wait_ms(300);
	for (i = 0; i < 100; i++)
	{
		SCI_printf("TEST_NUMBER : %d\n",i+1);
		wait_ms(100);
	}
	wait_ms(300);
	SCI_printf("FINISH TEST\n");
	wait_ms(1000);
	SCI_clear();
}

void IOex_test(void){
	int i;
	unsigned char sw;
	init_clk();		//各種基本機能を先に初期化する
	init_sci(115200);
	init_I2C();
	init_IOex();
	init_CMT();
	
	for(i=0;i<100*100*100;i++);

	SCI_printf("START LEDTEST\n");
	for(i=0;i<2;i++){
		LED(0x00);
		wait_ms(500);
		LED(0x01);
		wait_ms(500);
		LED(0x02);
		wait_ms(500);
		LED(0x04);
		wait_ms(500);
		LED(0x08);
		wait_ms(500);
		LED(0x0F);
		wait_ms(500);
		SCI_printf("ROTATE %d\n",i);
	}
	sci_wait_send_check();
	SCI_printf("FINISH LEDTEST\n");
	sci_wait_send_check();
	SCI_printf("\nSTART SWTEST\n");
	sci_wait_send_check();
	SCI_printf("push switch2\n");
	while (1)
	{
		sw = IOex_SWITCH();
		if (SWITCH_2(sw) == 1)
		{
			sci_wait_send_check();
			SCI_printf("SUCCESSED_sw3\n");
			break;
		}
		wait_ms(100);
	}
	sci_wait_send_check();
	SCI_printf("push switch3\n");
	while (1)
	{
		sw = IOex_SWITCH();
		if (SWITCH_3(sw) == 1)
		{
			sci_wait_send_check();
			SCI_printf("SUCCESSED_sw3\n");
			break;
		}
		wait_ms(100);
	}
	sci_wait_send_check();
	SCI_printf("push switch4\n");
	while (1)
	{
		sw = IOex_SWITCH();
		if (SWITCH_4(sw) == 1)
		{
			sci_wait_send_check();
			SCI_printf("SUCCESSED_sw4\n");
			break;
		}
		wait_ms(100);
	}
	sci_wait_send_check();
	SCI_printf("FINISH SWTEST\n");
}

void buzzer_test(void){
	volatile int i,j;
	init_clk();
	init_sci(115200);
	init_buzzer();
	init_CMT();
	
	SCI_printf("VOLUME TEST\n");
	BEEP(0,1000,10);
	wait_ms(500);
	BEEP(50,1000,10);
	wait_ms(500);
	BEEP(90,1000,10);
	wait_ms(500);
	SCI_printf("FREQUENCY TEST\n");
	BEEP(50,50,10);
	wait_ms(500);
	BEEP(50,1000,10);
	wait_ms(500);
	BEEP(50,2000,10);
	wait_ms(500);
	BEEP(50,15000,10);
	SCI_printf("FINISH TEST\n");
}