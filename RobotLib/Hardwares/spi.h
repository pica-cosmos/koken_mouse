#ifndef SPI_H_
#define SPI_H_

#define ENC_SS RSPI0.SPCMD0.BIT.SSLA 
#define ENC_RIGHT_SS 0x00
#define ENC_LEFT_SS 0x02

#define GYRO_ADDRESS 0x062100

void init_spi_gyro(void);
void preprocess_spi_gyro_2byte(int address);
void write_spdr_gyro(void);
void spii_int_gyro(void);
void read_spdr_gyro(void);
int ovre_check(void);
int gyro_read_check(void);
int gyro_write_check(void);
long read_gyro_data(void);

//For encoder
void init_spi_enc(void);
void preprocess_spi_enc(int address);
void spii_int_enc(void);
void read_spdr_enc(void);
void write_spdr_enc(void);
int enc_write_cnt_check(void);
int Get_enc_data(void);

#endif