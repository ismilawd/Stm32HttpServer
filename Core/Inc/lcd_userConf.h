
uint8_t i2cDeviceAddr = 0x4E; //* LCD i2c module default address,
I2C_HandleTypeDef hi2cx;
extern I2C_HandleTypeDef hi2c1;
static void hi2cx_define(void)
{
	hi2cx = hi2c1;	//* Change "hi2c1" like hi2c2, hi2c3... according to which i2c type you use, default is hi2c1.
}

#define LCD_16x2

#define BFR_MAX 100
