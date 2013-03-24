#include "board.h"

// MPU6050, Standard address 0x68
// MPU_INT on PB13 on rev4 hardware
#define MPU6050_ADDRESS         0x68

// Experimental DMP support
// #define MPU6050_DMP

#define DMP_MEM_START_ADDR 0x6E
#define DMP_MEM_R_W 0x6F

#define INV_MAX_NUM_ACCEL_SAMPLES      (8)
#define DMP_REF_QUATERNION             (0)
#define DMP_REF_GYROS                  (DMP_REF_QUATERNION + 4) // 4
#define DMP_REF_CONTROL                (DMP_REF_GYROS + 3)      // 7
#define DMP_REF_RAW                    (DMP_REF_CONTROL + 4)    // 11
#define DMP_REF_RAW_EXTERNAL           (DMP_REF_RAW + 8)        // 19
#define DMP_REF_ACCEL                  (DMP_REF_RAW_EXTERNAL + 6)       // 25
#define DMP_REF_QUANT_ACCEL            (DMP_REF_ACCEL + 3)      // 28
#define DMP_REF_QUATERNION_6AXIS       (DMP_REF_QUANT_ACCEL + INV_MAX_NUM_ACCEL_SAMPLES)        // 36
#define DMP_REF_EIS                    (DMP_REF_QUATERNION_6AXIS + 4)   // 40
#define DMP_REF_DMP_PACKET             (DMP_REF_EIS + 3)        // 43
#define DMP_REF_GARBAGE                (DMP_REF_DMP_PACKET + 1) // 44
#define DMP_REF_LAST                   (DMP_REF_GARBAGE + 1)    // 45

#define MPU_RA_XG_OFFS_TC       0x00    //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU_RA_YG_OFFS_TC       0x01    //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU_RA_ZG_OFFS_TC       0x02    //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU_RA_X_FINE_GAIN      0x03    //[7:0] X_FINE_GAIN
#define MPU_RA_Y_FINE_GAIN      0x04    //[7:0] Y_FINE_GAIN
#define MPU_RA_Z_FINE_GAIN      0x05    //[7:0] Z_FINE_GAIN
#define MPU_RA_XA_OFFS_H        0x06    //[15:0] XA_OFFS
#define MPU_RA_XA_OFFS_L_TC     0x07
#define MPU_RA_YA_OFFS_H        0x08    //[15:0] YA_OFFS
#define MPU_RA_YA_OFFS_L_TC     0x09
#define MPU_RA_ZA_OFFS_H        0x0A    //[15:0] ZA_OFFS
#define MPU_RA_ZA_OFFS_L_TC     0x0B
#define MPU_RA_PRODUCT_ID       0x0C    // Product ID Register
#define MPU_RA_XG_OFFS_USRH     0x13    //[15:0] XG_OFFS_USR
#define MPU_RA_XG_OFFS_USRL     0x14
#define MPU_RA_YG_OFFS_USRH     0x15    //[15:0] YG_OFFS_USR
#define MPU_RA_YG_OFFS_USRL     0x16
#define MPU_RA_ZG_OFFS_USRH     0x17    //[15:0] ZG_OFFS_USR
#define MPU_RA_ZG_OFFS_USRL     0x18
#define MPU_RA_SMPLRT_DIV       0x19
#define MPU_RA_CONFIG           0x1A
#define MPU_RA_GYRO_CONFIG      0x1B
#define MPU_RA_ACCEL_CONFIG     0x1C
#define MPU_RA_FF_THR           0x1D
#define MPU_RA_FF_DUR           0x1E
#define MPU_RA_MOT_THR          0x1F
#define MPU_RA_MOT_DUR          0x20
#define MPU_RA_ZRMOT_THR        0x21
#define MPU_RA_ZRMOT_DUR        0x22
#define MPU_RA_FIFO_EN          0x23
#define MPU_RA_I2C_MST_CTRL     0x24
#define MPU_RA_I2C_SLV0_ADDR    0x25
#define MPU_RA_I2C_SLV0_REG     0x26
#define MPU_RA_I2C_SLV0_CTRL    0x27
#define MPU_RA_I2C_SLV1_ADDR    0x28
#define MPU_RA_I2C_SLV1_REG     0x29
#define MPU_RA_I2C_SLV1_CTRL    0x2A
#define MPU_RA_I2C_SLV2_ADDR    0x2B
#define MPU_RA_I2C_SLV2_REG     0x2C
#define MPU_RA_I2C_SLV2_CTRL    0x2D
#define MPU_RA_I2C_SLV3_ADDR    0x2E
#define MPU_RA_I2C_SLV3_REG     0x2F
#define MPU_RA_I2C_SLV3_CTRL    0x30
#define MPU_RA_I2C_SLV4_ADDR    0x31
#define MPU_RA_I2C_SLV4_REG     0x32
#define MPU_RA_I2C_SLV4_DO      0x33
#define MPU_RA_I2C_SLV4_CTRL    0x34
#define MPU_RA_I2C_SLV4_DI      0x35
#define MPU_RA_I2C_MST_STATUS   0x36
#define MPU_RA_INT_PIN_CFG      0x37
#define MPU_RA_INT_ENABLE       0x38
#define MPU_RA_DMP_INT_STATUS   0x39
#define MPU_RA_INT_STATUS       0x3A
#define MPU_RA_ACCEL_XOUT_H     0x3B
#define MPU_RA_ACCEL_XOUT_L     0x3C
#define MPU_RA_ACCEL_YOUT_H     0x3D
#define MPU_RA_ACCEL_YOUT_L     0x3E
#define MPU_RA_ACCEL_ZOUT_H     0x3F
#define MPU_RA_ACCEL_ZOUT_L     0x40
#define MPU_RA_TEMP_OUT_H       0x41
#define MPU_RA_TEMP_OUT_L       0x42
#define MPU_RA_GYRO_XOUT_H      0x43
#define MPU_RA_GYRO_XOUT_L      0x44
#define MPU_RA_GYRO_YOUT_H      0x45
#define MPU_RA_GYRO_YOUT_L      0x46
#define MPU_RA_GYRO_ZOUT_H      0x47
#define MPU_RA_GYRO_ZOUT_L      0x48
#define MPU_RA_EXT_SENS_DATA_00 0x49
#define MPU_RA_MOT_DETECT_STATUS    0x61
#define MPU_RA_I2C_SLV0_DO      0x63
#define MPU_RA_I2C_SLV1_DO      0x64
#define MPU_RA_I2C_SLV2_DO      0x65
#define MPU_RA_I2C_SLV3_DO      0x66
#define MPU_RA_I2C_MST_DELAY_CTRL   0x67
#define MPU_RA_SIGNAL_PATH_RESET    0x68
#define MPU_RA_MOT_DETECT_CTRL      0x69
#define MPU_RA_USER_CTRL        0x6A
#define MPU_RA_PWR_MGMT_1       0x6B
#define MPU_RA_PWR_MGMT_2       0x6C
#define MPU_RA_BANK_SEL         0x6D
#define MPU_RA_MEM_START_ADDR   0x6E
#define MPU_RA_MEM_R_W          0x6F
#define MPU_RA_DMP_CFG_1        0x70
#define MPU_RA_DMP_CFG_2        0x71
#define MPU_RA_FIFO_COUNTH      0x72
#define MPU_RA_FIFO_COUNTL      0x73
#define MPU_RA_FIFO_R_W         0x74
#define MPU_RA_WHO_AM_I         0x75

#define MPU6050_SMPLRT_DIV      0       //8000Hz
#define MPU6050_DLPF_CFG        0   // 256Hz
//#define MPU6050_DLPF_CFG   1        // 188Hz
//#define MPU6050_DLPF_CFG   2        // 98Hz
//#define MPU6050_DLPF_CFG   3        // 42Hz

#define MPU6000ES_REV_C4        0x14
#define MPU6000ES_REV_C5        0x15
#define MPU6000ES_REV_D6        0x16
#define MPU6000ES_REV_D7        0x17
#define MPU6000ES_REV_D8        0x18
#define MPU6000_REV_C4          0x54
#define MPU6000_REV_C5          0x55
#define MPU6000_REV_D6          0x56
#define MPU6000_REV_D7          0x57
#define MPU6000_REV_D8          0x58
#define MPU6000_REV_D9          0x59

static void mpu6050AccInit(void);
static void mpu6050AccRead(int16_t * accData);
static void mpu6050AccAlign(int16_t * accData);
static void mpu6050GyroInit(void);
static void mpu6050GyroRead(int16_t * gyroData);
static void mpu6050GyroAlign(int16_t * gyroData);

#ifdef MPU6050_DMP
static void mpu6050DmpInit(void);
float dmpdata[2];
int16_t dmpGyroData[3];
#endif

extern uint16_t acc_1G;
static uint8_t mpuAccelHalf = 0;

bool mpu6050Detect(sensor_t * acc, sensor_t * gyro, uint8_t *scale)
{
    bool ack;
    uint8_t sig, rev;
    uint8_t tmp[6];

    delay(35);                  // datasheet page 13 says 30ms. other stuff could have been running meanwhile. but we'll be safe

    ack = i2cRead(MPU6050_ADDRESS, MPU_RA_WHO_AM_I, 1, &sig);
    if (!ack)
        return false;

    // So like, MPU6xxx has a "WHO_AM_I" register, that is used to verify the identity of the device.
    // The contents of WHO_AM_I are the upper 6 bits of the MPU-60X0�s 7-bit I2C address.
    // The least significant bit of the MPU-60X0�s I2C address is determined by the value of the AD0 pin. (we know that already).
    // But here's the best part: The value of the AD0 pin is not reflected in this register.
    if (sig != (MPU6050_ADDRESS & 0x7e))
        return false;

    // determine product ID and accel revision
    i2cRead(MPU6050_ADDRESS, MPU_RA_XA_OFFS_H, 6, tmp);
    rev = ((tmp[5] & 0x01) << 2) | ((tmp[3] & 0x01) << 1) | (tmp[1] & 0x01);
    if (rev) {
        /* Congrats, these parts are better. */
        if (rev == 1) {
            mpuAccelHalf = 1;
        } else if (rev == 2) {
            mpuAccelHalf = 0;
        } else {
            failureMode(5);
        }
    } else {
        i2cRead(MPU6050_ADDRESS, MPU_RA_PRODUCT_ID, 1, &sig);
        rev = sig & 0x0F;
        if (!rev) {
            failureMode(5);
        } else if (rev == 4) {
            mpuAccelHalf = 1;
        } else {
            mpuAccelHalf = 0;
        }
    }

    acc->init = mpu6050AccInit;
    acc->read = mpu6050AccRead;
    acc->align = mpu6050AccAlign;
    gyro->init = mpu6050GyroInit;
    gyro->read = mpu6050GyroRead;
    gyro->align = mpu6050GyroAlign;

    // give halfacc (old revision) back to system
    if (scale)
        *scale = mpuAccelHalf;

#ifdef MPU6050_DMP
    mpu6050DmpInit();
#endif

    return true;
}

static void mpu6050AccInit(void)
{
    if (mpuAccelHalf)
        acc_1G = 255;
    else
        acc_1G = 512;
}

static void mpu6050AccRead(int16_t *accData)
{
    uint8_t buf[6];

#ifndef MPU6050_DMP
    i2cRead(MPU6050_ADDRESS, MPU_RA_ACCEL_XOUT_H, 6, buf);
    accData[0] = (int16_t)((buf[0] << 8) | buf[1]) / 8;
    accData[1] = (int16_t)((buf[2] << 8) | buf[3]) / 8;
    accData[2] = (int16_t)((buf[4] << 8) | buf[5]) / 8;
#else
    accData[0] = accData[1] = accData[2] = 0;
#endif
}

static void mpu6050AccAlign(int16_t *accData)
{
    int16_t temp[2];
    temp[0] = accData[0];
    temp[1] = accData[1];

    // official direction is RPY
    accData[0] = temp[1];
    accData[1] = -temp[0];
    accData[2] = accData[2];
}

static void mpu6050GyroInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // PB13 - MPU_INT output on rev4 hardware
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifndef MPU6050_DMP
    i2cWrite(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_1, 0x80);      //PWR_MGMT_1    -- DEVICE_RESET 1
    delay(5);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_SMPLRT_DIV, 0x00);      //SMPLRT_DIV    -- SMPLRT_DIV = 0  Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
    i2cWrite(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_1, 0x03);      //PWR_MGMT_1    -- SLEEP 0; CYCLE 0; TEMP_DIS 0; CLKSEL 3 (PLL with Z Gyro reference)
    i2cWrite(MPU6050_ADDRESS, MPU_RA_INT_PIN_CFG, 0 << 7 | 0 << 6 | 0 << 5 | 0 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 0 << 0);  // INT_PIN_CFG   -- INT_LEVEL_HIGH, INT_OPEN_DIS, LATCH_INT_DIS, INT_RD_CLEAR_DIS, FSYNC_INT_LEVEL_HIGH, FSYNC_INT_DIS, I2C_BYPASS_EN, CLOCK_DIS
    i2cWrite(MPU6050_ADDRESS, MPU_RA_CONFIG, MPU6050_DLPF_CFG);  //CONFIG        -- EXT_SYNC_SET 0 (disable input pin for data sync) ; default DLPF_CFG = 0 => ACC bandwidth = 260Hz  GYRO bandwidth = 256Hz)
    i2cWrite(MPU6050_ADDRESS, MPU_RA_GYRO_CONFIG, 0x18);      //GYRO_CONFIG   -- FS_SEL = 3: Full scale set to 2000 deg/sec

    // ACC Init stuff. Moved into gyro init because the reset above would screw up accel config. Oops.
    // Accel scale 8g (4096 LSB/g)
    i2cWrite(MPU6050_ADDRESS, MPU_RA_ACCEL_CONFIG, 2 << 3);
#endif
}

static void mpu6050GyroRead(int16_t * gyroData)
{
    uint8_t buf[6];
#ifndef MPU6050_DMP
    i2cRead(MPU6050_ADDRESS, MPU_RA_GYRO_XOUT_H, 6, buf);
    gyroData[0] = (int16_t)((buf[0] << 8) | buf[1]) / 4;
    gyroData[1] = (int16_t)((buf[2] << 8) | buf[3]) / 4;
    gyroData[2] = (int16_t)((buf[4] << 8) | buf[5]) / 4;
#else
    gyroData[0] = dmpGyroData[0] / 4 ;
    gyroData[1] = dmpGyroData[1] / 4;
    gyroData[2] = dmpGyroData[2] / 4;
#endif
}

static void mpu6050GyroAlign(int16_t * gyroData)
{
    // official direction is RPY
    gyroData[0] = gyroData[0];
    gyroData[1] = gyroData[1];
    gyroData[2] = -gyroData[2];
}

#ifdef MPU6050_DMP

//This 3D array contains the default DMP memory bank binary that gets loaded during initialization.
//In the Invensense UC3-A3 firmware this is uploaded in 128 byte tranmissions, but the Arduino Wire
//library only supports 32 byte transmissions, including the register address to which you're writing,
//so I broke it up into 16 byte transmission payloads which are sent in the dmp_init() function below.
//
//This was reconstructed from observed I2C traffic generated by the UC3-A3 demo code, and not extracted
//directly from that code. That is true of all transmissions in this sketch, and any documentation has
//been added after the fact by referencing the Invensense code.

const unsigned char dmpMem[8][16][16] = {
    {
     {0xFB, 0x00, 0x00, 0x3E, 0x00, 0x0B, 0x00, 0x36, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00},
     {0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0xFA, 0x80, 0x00, 0x0B, 0x12, 0x82, 0x00, 0x01},
     {0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x28, 0x00, 0x00, 0xFF, 0xFF, 0x45, 0x81, 0xFF, 0xFF, 0xFA, 0x72, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x03, 0xE8, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x7F, 0xFF, 0xFF, 0xFE, 0x80, 0x01},
     {0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x3E, 0x03, 0x30, 0x40, 0x00, 0x00, 0x00, 0x02, 0xCA, 0xE3, 0x09, 0x3E, 0x80, 0x00, 0x00},
     {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00},
     {0x41, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x2A, 0x00, 0x00, 0x16, 0x55, 0x00, 0x00, 0x21, 0x82},
     {0xFD, 0x87, 0x26, 0x50, 0xFD, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x05, 0x80, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00},
     {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x6F, 0x00, 0x02, 0x65, 0x32, 0x00, 0x00, 0x5E, 0xC0},
     {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0xFB, 0x8C, 0x6F, 0x5D, 0xFD, 0x5D, 0x08, 0xD9, 0x00, 0x7C, 0x73, 0x3B, 0x00, 0x6C, 0x12, 0xCC},
     {0x32, 0x00, 0x13, 0x9D, 0x32, 0x00, 0xD0, 0xD6, 0x32, 0x00, 0x08, 0x00, 0x40, 0x00, 0x01, 0xF4},
     {0xFF, 0xE6, 0x80, 0x79, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD0, 0xD6, 0x00, 0x00, 0x27, 0x10}
     },
    {
     {0xFB, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0xFA, 0x36, 0xFF, 0xBC, 0x30, 0x8E, 0x00, 0x05, 0xFB, 0xF0, 0xFF, 0xD9, 0x5B, 0xC8},
     {0xFF, 0xD0, 0x9A, 0xBE, 0x00, 0x00, 0x10, 0xA9, 0xFF, 0xF4, 0x1E, 0xB2, 0x00, 0xCE, 0xBB, 0xF7},
     {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x02, 0x02, 0x00, 0x00, 0x0C},
     {0xFF, 0xC2, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0xCF, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x14},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x03, 0x3F, 0x68, 0xB6, 0x79, 0x35, 0x28, 0xBC, 0xC6, 0x7E, 0xD1, 0x6C},
     {0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2, 0x6A, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x30},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x25, 0x4D, 0x00, 0x2F, 0x70, 0x6D, 0x00, 0x00, 0x05, 0xAE, 0x00, 0x0C, 0x02, 0xD0}
     },
    {
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x01, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0xFF, 0xEF, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00},
     {0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
     },
    {
     {0xD8, 0xDC, 0xBA, 0xA2, 0xF1, 0xDE, 0xB2, 0xB8, 0xB4, 0xA8, 0x81, 0x91, 0xF7, 0x4A, 0x90, 0x7F},
     {0x91, 0x6A, 0xF3, 0xF9, 0xDB, 0xA8, 0xF9, 0xB0, 0xBA, 0xA0, 0x80, 0xF2, 0xCE, 0x81, 0xF3, 0xC2},
     {0xF1, 0xC1, 0xF2, 0xC3, 0xF3, 0xCC, 0xA2, 0xB2, 0x80, 0xF1, 0xC6, 0xD8, 0x80, 0xBA, 0xA7, 0xDF},
     {0xDF, 0xDF, 0xF2, 0xA7, 0xC3, 0xCB, 0xC5, 0xB6, 0xF0, 0x87, 0xA2, 0x94, 0x24, 0x48, 0x70, 0x3C},
     {0x95, 0x40, 0x68, 0x34, 0x58, 0x9B, 0x78, 0xA2, 0xF1, 0x83, 0x92, 0x2D, 0x55, 0x7D, 0xD8, 0xB1},
     {0xB4, 0xB8, 0xA1, 0xD0, 0x91, 0x80, 0xF2, 0x70, 0xF3, 0x70, 0xF2, 0x7C, 0x80, 0xA8, 0xF1, 0x01},
     {0xB0, 0x98, 0x87, 0xD9, 0x43, 0xD8, 0x86, 0xC9, 0x88, 0xBA, 0xA1, 0xF2, 0x0E, 0xB8, 0x97, 0x80},
     {0xF1, 0xA9, 0xDF, 0xDF, 0xDF, 0xAA, 0xDF, 0xDF, 0xDF, 0xF2, 0xAA, 0xC5, 0xCD, 0xC7, 0xA9, 0x0C},
     {0xC9, 0x2C, 0x97, 0x97, 0x97, 0x97, 0xF1, 0xA9, 0x89, 0x26, 0x46, 0x66, 0xB0, 0xB4, 0xBA, 0x80},
     {0xAC, 0xDE, 0xF2, 0xCA, 0xF1, 0xB2, 0x8C, 0x02, 0xA9, 0xB6, 0x98, 0x00, 0x89, 0x0E, 0x16, 0x1E},
     {0xB8, 0xA9, 0xB4, 0x99, 0x2C, 0x54, 0x7C, 0xB0, 0x8A, 0xA8, 0x96, 0x36, 0x56, 0x76, 0xF1, 0xB9},
     {0xAF, 0xB4, 0xB0, 0x83, 0xC0, 0xB8, 0xA8, 0x97, 0x11, 0xB1, 0x8F, 0x98, 0xB9, 0xAF, 0xF0, 0x24},
     {0x08, 0x44, 0x10, 0x64, 0x18, 0xF1, 0xA3, 0x29, 0x55, 0x7D, 0xAF, 0x83, 0xB5, 0x93, 0xAF, 0xF0},
     {0x00, 0x28, 0x50, 0xF1, 0xA3, 0x86, 0x9F, 0x61, 0xA6, 0xDA, 0xDE, 0xDF, 0xD9, 0xFA, 0xA3, 0x86},
     {0x96, 0xDB, 0x31, 0xA6, 0xD9, 0xF8, 0xDF, 0xBA, 0xA6, 0x8F, 0xC2, 0xC5, 0xC7, 0xB2, 0x8C, 0xC1},
     {0xB8, 0xA2, 0xDF, 0xDF, 0xDF, 0xA3, 0xDF, 0xDF, 0xDF, 0xD8, 0xD8, 0xF1, 0xB8, 0xA8, 0xB2, 0x86}
     },
    {
     {0xB4, 0x98, 0x0D, 0x35, 0x5D, 0xB8, 0xAA, 0x98, 0xB0, 0x87, 0x2D, 0x35, 0x3D, 0xB2, 0xB6, 0xBA},
     {0xAF, 0x8C, 0x96, 0x19, 0x8F, 0x9F, 0xA7, 0x0E, 0x16, 0x1E, 0xB4, 0x9A, 0xB8, 0xAA, 0x87, 0x2C},
     {0x54, 0x7C, 0xB9, 0xA3, 0xDE, 0xDF, 0xDF, 0xA3, 0xB1, 0x80, 0xF2, 0xC4, 0xCD, 0xC9, 0xF1, 0xB8},
     {0xA9, 0xB4, 0x99, 0x83, 0x0D, 0x35, 0x5D, 0x89, 0xB9, 0xA3, 0x2D, 0x55, 0x7D, 0xB5, 0x93, 0xA3},
     {0x0E, 0x16, 0x1E, 0xA9, 0x2C, 0x54, 0x7C, 0xB8, 0xB4, 0xB0, 0xF1, 0x97, 0x83, 0xA8, 0x11, 0x84},
     {0xA5, 0x09, 0x98, 0xA3, 0x83, 0xF0, 0xDA, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xD8, 0xF1, 0xA5},
     {0x29, 0x55, 0x7D, 0xA5, 0x85, 0x95, 0x02, 0x1A, 0x2E, 0x3A, 0x56, 0x5A, 0x40, 0x48, 0xF9, 0xF3},
     {0xA3, 0xD9, 0xF8, 0xF0, 0x98, 0x83, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0x97, 0x82, 0xA8, 0xF1},
     {0x11, 0xF0, 0x98, 0xA2, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xDA, 0xF3, 0xDE, 0xD8, 0x83, 0xA5},
     {0x94, 0x01, 0xD9, 0xA3, 0x02, 0xF1, 0xA2, 0xC3, 0xC5, 0xC7, 0xD8, 0xF1, 0x84, 0x92, 0xA2, 0x4D},
     {0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9},
     {0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0x93, 0xA3, 0x4D},
     {0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9},
     {0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0xA8, 0x8A, 0x9A},
     {0xF0, 0x28, 0x50, 0x78, 0x9E, 0xF3, 0x88, 0x18, 0xF1, 0x9F, 0x1D, 0x98, 0xA8, 0xD9, 0x08, 0xD8},
     {0xC8, 0x9F, 0x12, 0x9E, 0xF3, 0x15, 0xA8, 0xDA, 0x12, 0x10, 0xD8, 0xF1, 0xAF, 0xC8, 0x97, 0x87}
     },
    {
     {0x34, 0xB5, 0xB9, 0x94, 0xA4, 0x21, 0xF3, 0xD9, 0x22, 0xD8, 0xF2, 0x2D, 0xF3, 0xD9, 0x2A, 0xD8},
     {0xF2, 0x35, 0xF3, 0xD9, 0x32, 0xD8, 0x81, 0xA4, 0x60, 0x60, 0x61, 0xD9, 0x61, 0xD8, 0x6C, 0x68},
     {0x69, 0xD9, 0x69, 0xD8, 0x74, 0x70, 0x71, 0xD9, 0x71, 0xD8, 0xB1, 0xA3, 0x84, 0x19, 0x3D, 0x5D},
     {0xA3, 0x83, 0x1A, 0x3E, 0x5E, 0x93, 0x10, 0x30, 0x81, 0x10, 0x11, 0xB8, 0xB0, 0xAF, 0x8F, 0x94},
     {0xF2, 0xDA, 0x3E, 0xD8, 0xB4, 0x9A, 0xA8, 0x87, 0x29, 0xDA, 0xF8, 0xD8, 0x87, 0x9A, 0x35, 0xDA},
     {0xF8, 0xD8, 0x87, 0x9A, 0x3D, 0xDA, 0xF8, 0xD8, 0xB1, 0xB9, 0xA4, 0x98, 0x85, 0x02, 0x2E, 0x56},
     {0xA5, 0x81, 0x00, 0x0C, 0x14, 0xA3, 0x97, 0xB0, 0x8A, 0xF1, 0x2D, 0xD9, 0x28, 0xD8, 0x4D, 0xD9},
     {0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x84, 0x0D, 0xDA, 0x0E, 0xD8, 0xA3, 0x29, 0x83, 0xDA},
     {0x2C, 0x0E, 0xD8, 0xA3, 0x84, 0x49, 0x83, 0xDA, 0x2C, 0x4C, 0x0E, 0xD8, 0xB8, 0xB0, 0xA8, 0x8A},
     {0x9A, 0xF5, 0x20, 0xAA, 0xDA, 0xDF, 0xD8, 0xA8, 0x40, 0xAA, 0xD0, 0xDA, 0xDE, 0xD8, 0xA8, 0x60},
     {0xAA, 0xDA, 0xD0, 0xDF, 0xD8, 0xF1, 0x97, 0x86, 0xA8, 0x31, 0x9B, 0x06, 0x99, 0x07, 0xAB, 0x97},
     {0x28, 0x88, 0x9B, 0xF0, 0x0C, 0x20, 0x14, 0x40, 0xB8, 0xB0, 0xB4, 0xA8, 0x8C, 0x9C, 0xF0, 0x04},
     {0x28, 0x51, 0x79, 0x1D, 0x30, 0x14, 0x38, 0xB2, 0x82, 0xAB, 0xD0, 0x98, 0x2C, 0x50, 0x50, 0x78},
     {0x78, 0x9B, 0xF1, 0x1A, 0xB0, 0xF0, 0x8A, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x8B, 0x29, 0x51, 0x79},
     {0x8A, 0x24, 0x70, 0x59, 0x8B, 0x20, 0x58, 0x71, 0x8A, 0x44, 0x69, 0x38, 0x8B, 0x39, 0x40, 0x68},
     {0x8A, 0x64, 0x48, 0x31, 0x8B, 0x30, 0x49, 0x60, 0xA5, 0x88, 0x20, 0x09, 0x71, 0x58, 0x44, 0x68}
     },
    {
     {0x11, 0x39, 0x64, 0x49, 0x30, 0x19, 0xF1, 0xAC, 0x00, 0x2C, 0x54, 0x7C, 0xF0, 0x8C, 0xA8, 0x04},
     {0x28, 0x50, 0x78, 0xF1, 0x88, 0x97, 0x26, 0xA8, 0x59, 0x98, 0xAC, 0x8C, 0x02, 0x26, 0x46, 0x66},
     {0xF0, 0x89, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x24, 0x70, 0x59, 0x44, 0x69, 0x38, 0x64, 0x48, 0x31},
     {0xA9, 0x88, 0x09, 0x20, 0x59, 0x70, 0xAB, 0x11, 0x38, 0x40, 0x69, 0xA8, 0x19, 0x31, 0x48, 0x60},
     {0x8C, 0xA8, 0x3C, 0x41, 0x5C, 0x20, 0x7C, 0x00, 0xF1, 0x87, 0x98, 0x19, 0x86, 0xA8, 0x6E, 0x76},
     {0x7E, 0xA9, 0x99, 0x88, 0x2D, 0x55, 0x7D, 0x9E, 0xB9, 0xA3, 0x8A, 0x22, 0x8A, 0x6E, 0x8A, 0x56},
     {0x8A, 0x5E, 0x9F, 0xB1, 0x83, 0x06, 0x26, 0x46, 0x66, 0x0E, 0x2E, 0x4E, 0x6E, 0x9D, 0xB8, 0xAD},
     {0x00, 0x2C, 0x54, 0x7C, 0xF2, 0xB1, 0x8C, 0xB4, 0x99, 0xB9, 0xA3, 0x2D, 0x55, 0x7D, 0x81, 0x91},
     {0xAC, 0x38, 0xAD, 0x3A, 0xB5, 0x83, 0x91, 0xAC, 0x2D, 0xD9, 0x28, 0xD8, 0x4D, 0xD9, 0x48, 0xD8},
     {0x6D, 0xD9, 0x68, 0xD8, 0x8C, 0x9D, 0xAE, 0x29, 0xD9, 0x04, 0xAE, 0xD8, 0x51, 0xD9, 0x04, 0xAE},
     {0xD8, 0x79, 0xD9, 0x04, 0xD8, 0x81, 0xF3, 0x9D, 0xAD, 0x00, 0x8D, 0xAE, 0x19, 0x81, 0xAD, 0xD9},
     {0x01, 0xD8, 0xF2, 0xAE, 0xDA, 0x26, 0xD8, 0x8E, 0x91, 0x29, 0x83, 0xA7, 0xD9, 0xAD, 0xAD, 0xAD},
     {0xAD, 0xF3, 0x2A, 0xD8, 0xD8, 0xF1, 0xB0, 0xAC, 0x89, 0x91, 0x3E, 0x5E, 0x76, 0xF3, 0xAC, 0x2E},
     {0x2E, 0xF1, 0xB1, 0x8C, 0x5A, 0x9C, 0xAC, 0x2C, 0x28, 0x28, 0x28, 0x9C, 0xAC, 0x30, 0x18, 0xA8},
     {0x98, 0x81, 0x28, 0x34, 0x3C, 0x97, 0x24, 0xA7, 0x28, 0x34, 0x3C, 0x9C, 0x24, 0xF2, 0xB0, 0x89},
     {0xAC, 0x91, 0x2C, 0x4C, 0x6C, 0x8A, 0x9B, 0x2D, 0xD9, 0xD8, 0xD8, 0x51, 0xD9, 0xD8, 0xD8, 0x79}
     },
    {
     {0xD9, 0xD8, 0xD8, 0xF1, 0x9E, 0x88, 0xA3, 0x31, 0xDA, 0xD8, 0xD8, 0x91, 0x2D, 0xD9, 0x28, 0xD8},
     {0x4D, 0xD9, 0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x83, 0x93, 0x35, 0x3D, 0x80, 0x25, 0xDA},
     {0xD8, 0xD8, 0x85, 0x69, 0xDA, 0xD8, 0xD8, 0xB4, 0x93, 0x81, 0xA3, 0x28, 0x34, 0x3C, 0xF3, 0xAB},
     {0x8B, 0xF8, 0xA3, 0x91, 0xB6, 0x09, 0xB4, 0xD9, 0xAB, 0xDE, 0xFA, 0xB0, 0x87, 0x9C, 0xB9, 0xA3},
     {0xDD, 0xF1, 0xA3, 0xA3, 0xA3, 0xA3, 0x95, 0xF1, 0xA3, 0xA3, 0xA3, 0x9D, 0xF1, 0xA3, 0xA3, 0xA3},
     {0xA3, 0xF2, 0xA3, 0xB4, 0x90, 0x80, 0xF2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3},
     {0xA3, 0xB2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xB0, 0x87, 0xB5, 0x99, 0xF1, 0xA3, 0xA3, 0xA3},
     {0x98, 0xF1, 0xA3, 0xA3, 0xA3, 0xA3, 0x97, 0xA3, 0xA3, 0xA3, 0xA3, 0xF3, 0x9B, 0xA3, 0xA3, 0xDC},
     {0xB9, 0xA7, 0xF1, 0x26, 0x26, 0x26, 0xD8, 0xD8, 0xFF}
     }
};


//DMP update transmissions (Bank, Start Address, Update Length, Update Data...)

const uint8_t dmp_updates[29][9] = {
    {0x03, 0x7B, 0x03, 0x4C, 0xCD, 0x6C},       //FCFG_1 inv_set_gyro_calibration
    {0x03, 0xAB, 0x03, 0x36, 0x56, 0x76},       //FCFG_3 inv_set_gyro_calibration
    {0x00, 0x68, 0x04, 0x02, 0xCB, 0x47, 0xA2}, //D_0_104 inv_set_gyro_calibration
    {0x02, 0x18, 0x04, 0x00, 0x05, 0x8B, 0xC1}, //D_0_24 inv_set_gyro_calibration
    {0x01, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x00}, //D_1_152 inv_set_accel_calibration
    {0x03, 0x7F, 0x06, 0x0C, 0xC9, 0x2C, 0x97, 0x97, 0x97},     //FCFG_2 inv_set_accel_calibration
    {0x03, 0x89, 0x03, 0x26, 0x46, 0x66},       //FCFG_7 inv_set_accel_calibration
    {0x00, 0x6C, 0x02, 0x20, 0x00},     //D_0_108 inv_set_accel_calibration
    {0x02, 0x40, 0x04, 0x00, 0x00, 0x00, 0x00}, //CPASS_MTX_00 inv_set_compass_calibration
    {0x02, 0x44, 0x04, 0x00, 0x00, 0x00, 0x00}, //CPASS_MTX_01
    {0x02, 0x48, 0x04, 0x00, 0x00, 0x00, 0x00}, //CPASS_MTX_02
    {0x02, 0x4C, 0x04, 0x00, 0x00, 0x00, 0x00}, //CPASS_MTX_10
    {0x02, 0x50, 0x04, 0x00, 0x00, 0x00, 0x00}, //CPASS_MTX_11
    {0x02, 0x54, 0x04, 0x00, 0x00, 0x00, 0x00}, //CPASS_MTX_12
    {0x02, 0x58, 0x04, 0x00, 0x00, 0x00, 0x00}, //CPASS_MTX_20
    {0x02, 0x5C, 0x04, 0x00, 0x00, 0x00, 0x00}, //CPASS_MTX_21
    {0x02, 0xBC, 0x04, 0x00, 0x00, 0x00, 0x00}, //CPASS_MTX_22
    {0x01, 0xEC, 0x04, 0x00, 0x00, 0x40, 0x00}, //D_1_236 inv_apply_endian_accel
    {0x03, 0x7F, 0x06, 0x0C, 0xC9, 0x2C, 0x97, 0x97, 0x97},     //FCFG_2 inv_set_mpu_sensors
    {0x04, 0x02, 0x03, 0x0D, 0x35, 0x5D},       //CFG_MOTION_BIAS inv_turn_on_bias_from_no_motion
    {0x04, 0x09, 0x04, 0x87, 0x2D, 0x35, 0x3D}, //FCFG_5 inv_set_bias_update 
    {0x00, 0xA3, 0x01, 0x00},   //D_0_163 inv_set_dead_zone
    //SET INT_ENABLE at i=22
    {0x07, 0x86, 0x01, 0xFE},   //CFG_6 inv_set_fifo_interupt
    {0x07, 0x41, 0x05, 0xF1, 0x20, 0x28, 0x30, 0x38},   //CFG_8 inv_send_quaternion
    {0x07, 0x7E, 0x01, 0x30},   //CFG_16 inv_set_footer
    {0x07, 0x46, 0x01, 0x9A},   //CFG_GYRO_SOURCE inv_send_gyro
    {0x07, 0x47, 0x04, 0xF1, 0x28, 0x30, 0x38}, //CFG_9 inv_send_gyro -> inv_construct3_fifo
    {0x07, 0x6C, 0x04, 0xF1, 0x28, 0x30, 0x38}, //CFG_12 inv_send_accel -> inv_construct3_fifo
    {0x02, 0x16, 0x02, 0x00, 0x00},     //D_0_22 inv_set_fifo_rate
};

static long dmp_lastRead = 0;
static uint8_t dmp_processed_packet[8];
static uint8_t dmp_received_packet[50];
static uint8_t dmp_temp = 0;
uint8_t dmp_fifoCountL = 0;
static uint8_t dmp_fifoCountL2 = 0;
static uint8_t dmp_packetCount = 0x00;
static bool dmp_longPacket = false;
static bool dmp_firstPacket = true;

static void mpu6050DmpMemInit(void);
static void mpu6050DmpBankSelect(uint8_t bank);
static bool mpu6050DmpFifoReady(void);
static void mpu6050DmpGetPacket(void);
static void mpu6050DmpProcessQuat(void);
void mpu6050DmpResetFifo(void);

static void mpu6050DmpInit(void)
{
    uint8_t temp = 0;

    i2cWrite(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_1, 0xC0); // device reset
    i2cWrite(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_2, 0x00);
    delay(10);

    i2cWrite(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_1, 0x00);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_BANK_SEL, 0x70);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_MEM_START_ADDR, 0x06);
    i2cRead(MPU6050_ADDRESS, MPU_RA_MEM_R_W, 1, &temp);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_BANK_SEL, 0x00);

    /*
       dmp_temp = i2c_readReg(MPU60X0_I2CADDR, MPU_RA_XG_OFFS_TC);
       dmp_temp = i2c_readReg(MPU60X0_I2CADDR, MPU_RA_YG_OFFS_TC);
       dmp_temp = i2c_readReg(MPU60X0_I2CADDR, MPU_RA_ZG_OFFS_TC);
       dmp_temp = i2c_readReg(MPU60X0_I2CADDR, MPU_RA_USER_CTRL);
     */

    i2cWrite(MPU6050_ADDRESS, MPU_RA_INT_PIN_CFG, 0x32);        // I2C bypass enabled, latch int enabled, int read clear
    i2cRead(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_1, 1, &temp);
    delay(5);

    mpu6050DmpMemInit();
}

void mpu6050DmpLoop(void)
{
    uint8_t temp;
    uint8_t buf[2];

    if (mpu6050DmpFifoReady()) {
        LED1_ON;
        mpu6050DmpGetPacket();

        i2cRead(MPU6050_ADDRESS, MPU_RA_INT_STATUS, 1, &temp);
        if (dmp_firstPacket) {
            delay(1);
            mpu6050DmpBankSelect(0x00);
            
            mpu6050DmpBankSelect(0x00); // bank
            i2cWrite(MPU6050_ADDRESS, MPU_RA_MEM_START_ADDR, 0x60);
            i2cWriteBuffer(MPU6050_ADDRESS, MPU_RA_MEM_R_W, 4, "\x04\x00\x00\x00"); // data

            mpu6050DmpBankSelect(0x01);
            
            i2cWrite(MPU6050_ADDRESS, MPU_RA_MEM_START_ADDR, 0x62);
            i2cRead(MPU6050_ADDRESS, DMP_MEM_R_W, 2, buf);
            dmp_firstPacket = false;
            mpu6050DmpFifoReady();
        }

        if (dmp_fifoCountL == 42) {
            mpu6050DmpProcessQuat();
        }
        LED1_OFF;
    }
}

#define dmp_quatTake32(a, b) (((a)[4*(b)+0]<<8) | ((a)[4*(b)+1]<<0))
extern int16_t angle[2];

static void mpu6050DmpProcessQuat(void)
{
    float quat0, quat1, quat2, quat3;
    int32_t quatl0, quatl1, quatl2, quatl3;

    quatl0 = dmp_quatTake32(dmp_received_packet, 0);
    quatl1 = dmp_quatTake32(dmp_received_packet, 1);
    quatl2 = dmp_quatTake32(dmp_received_packet, 2);
    quatl3 = dmp_quatTake32(dmp_received_packet, 3);

    if (quatl0 > 32767) 
        quatl0 -= 65536;
    if (quatl1 > 32767) 
        quatl1 -= 65536;
    if (quatl2 > 32767) 
        quatl2 -= 65536;
    if (quatl3 > 32767) 
        quatl3 -= 65536;

    quat0 = ((float) quatl0) / 16384.0f;
    quat1 = ((float) quatl1) / 16384.0f;
    quat2 = ((float) quatl2) / 16384.0f;
    quat3 = ((float) quatl3) / 16384.0f;
    
    dmpdata[0] = atan2f(2 * ((quat0 * quat1) + (quat2 * quat3)), 1.0 - (2 * ((quat1 * quat1) + (quat2 * quat2)))) * (180.0f / M_PI);
    dmpdata[1] = asinf(2 * ((quat0 * quat2) - (quat3 * quat1))) * (180.0f / M_PI);
    angle[0] = dmpdata[0] * 10;
    angle[1] = dmpdata[1] * 10;

    dmpGyroData[0] = ((dmp_received_packet[4 * 4 + 0] << 8) | (dmp_received_packet[4 * 4 + 1] << 0));
    dmpGyroData[1] = ((dmp_received_packet[4 * 5 + 0] << 8) | (dmp_received_packet[4 * 5 + 1] << 0));
    dmpGyroData[2] = ((dmp_received_packet[4 * 6 + 0] << 8) | (dmp_received_packet[4 * 6 + 1] << 0));
}

void mpu6050DmpResetFifo(void)
{
    uint8_t ctrl;
    
    i2cRead(MPU6050_ADDRESS, MPU_RA_USER_CTRL, 1, &ctrl);
    ctrl |= 0x04;
    i2cWrite(MPU6050_ADDRESS, MPU_RA_USER_CTRL, ctrl);
}

static void mpu6050DmpGetPacket(void)
{
    if (dmp_fifoCountL > 32) {
        dmp_fifoCountL2 = dmp_fifoCountL - 32;
        dmp_longPacket = true;
    }
    
    if (dmp_longPacket) {
        i2cRead(MPU6050_ADDRESS, MPU_RA_FIFO_R_W, 32, dmp_received_packet);
        i2cRead(MPU6050_ADDRESS, MPU_RA_FIFO_R_W, dmp_fifoCountL, dmp_received_packet + 32);
        dmp_longPacket = false;
    } else {
        i2cRead(MPU6050_ADDRESS, MPU_RA_FIFO_R_W, dmp_fifoCountL, dmp_received_packet);
    }
}

uint16_t dmpFifoLevel = 0;

static bool mpu6050DmpFifoReady(void)
{
    uint8_t buf[2];

    i2cRead(MPU6050_ADDRESS, MPU_RA_FIFO_COUNTH, 2, buf);
    
    dmp_fifoCountL = buf[1];
    dmpFifoLevel = buf[0] << 8 | buf[1];
    
    if (dmp_fifoCountL == 42 || dmp_fifoCountL == 44)
        return true;
    else {
        // lame hack to empty out fifo, as dmpResetFifo doesn't actually seem to do it...
        if (dmpFifoLevel > 100) {
            // clear out fifo
            uint8_t crap[16];
            do {
                i2cRead(MPU6050_ADDRESS, MPU_RA_FIFO_R_W, dmpFifoLevel > 16 ? 16 : dmpFifoLevel, crap);
                i2cRead(MPU6050_ADDRESS, MPU_RA_FIFO_COUNTH, 2, buf);
                dmpFifoLevel = buf[0] << 8 | buf[1];
            } while (dmpFifoLevel);
        }
    }

    return false;
}

static void mpu6050DmpBankSelect(uint8_t bank)
{
    i2cWrite(MPU6050_ADDRESS, MPU_RA_BANK_SEL, bank);
}

static void mpu6050DmpBankInit(void)
{
    uint8_t i, j;
    uint8_t incoming[9];

    for (i = 0; i < 7; i++) {
        mpu6050DmpBankSelect(i);
        for (j = 0; j < 16; j++) {
            uint8_t start_addy = j * 0x10;

            i2cWrite(MPU6050_ADDRESS, DMP_MEM_START_ADDR, start_addy);
            i2cWriteBuffer(MPU6050_ADDRESS, DMP_MEM_R_W, 16, (uint8_t *) & dmpMem[i][j][0]);
        }
    }

    mpu6050DmpBankSelect(7);

    for (j = 0; j < 8; j++) {

        uint8_t start_addy = j * 0x10;

        i2cWrite(MPU6050_ADDRESS, DMP_MEM_START_ADDR, start_addy);
        i2cWriteBuffer(MPU6050_ADDRESS, DMP_MEM_R_W, 16, (uint8_t *) & dmpMem[7][j][0]);
    }

    i2cWrite(MPU6050_ADDRESS, DMP_MEM_START_ADDR, 0x80);
    i2cWriteBuffer(MPU6050_ADDRESS, DMP_MEM_R_W, 9, (uint8_t *) & dmpMem[7][8][0]);

    i2cRead(MPU6050_ADDRESS, DMP_MEM_R_W, 8, incoming);
}


static void mpu6050DmpMemInit(void)
{
    uint8_t i;
    uint8_t temp;

    mpu6050DmpBankInit();

    // Bank, Start Address, Update Length, Update Data...
    for (i = 0; i < 22; i++) {
        mpu6050DmpBankSelect(dmp_updates[i][0]); // bank
        i2cWrite(MPU6050_ADDRESS, DMP_MEM_START_ADDR, dmp_updates[i][1]); // address
        i2cWriteBuffer(MPU6050_ADDRESS, DMP_MEM_R_W, dmp_updates[i][2], (uint8_t *)&dmp_updates[i][3]); // data
    }

    i2cWrite(MPU6050_ADDRESS, MPU_RA_INT_ENABLE, 0x32);

    for (i = 22; i < 29; i++) {
        mpu6050DmpBankSelect(dmp_updates[i][0]); // bank
        i2cWrite(MPU6050_ADDRESS, DMP_MEM_START_ADDR, dmp_updates[i][1]); // address
        i2cWriteBuffer(MPU6050_ADDRESS, DMP_MEM_R_W, dmp_updates[i][2], (uint8_t *)&dmp_updates[i][3]); // data
    }

    /*
       dmp_temp = i2c_readReg(MPU60X0_I2CADDR, MPU_RA_PWR_MGMT_1);
       dmp_temp = i2c_readReg(MPU60X0_I2CADDR, MPU_RA_PWR_MGMT_2);
     */

    i2cWrite(MPU6050_ADDRESS, MPU_RA_INT_ENABLE, 0x02);     // ??
    i2cWrite(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_1, 0x03);     // CLKSEL =  PLL w Z ref
    i2cWrite(MPU6050_ADDRESS, MPU_RA_SMPLRT_DIV, 0x04);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_GYRO_CONFIG, 0x18);    // full scale 2000 deg/s
    i2cWrite(MPU6050_ADDRESS, MPU_RA_CONFIG, 0x0B); // ext_sync_set=temp_out_L, accel DLPF 44Hz, gyro DLPF 42Hz
    i2cWrite(MPU6050_ADDRESS, MPU_RA_DMP_CFG_1, 0x03);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_DMP_CFG_2, 0x00);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_XG_OFFS_TC, 0x00);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_YG_OFFS_TC, 0x00);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_ZG_OFFS_TC, 0x00);

    // clear offsets
    i2cWriteBuffer(MPU6050_ADDRESS, MPU_RA_XG_OFFS_USRH, 6, "\x00\x00\x00\x00\x00\x00"); // data

    mpu6050DmpBankSelect(0x01); // bank
    i2cWrite(MPU6050_ADDRESS, MPU_RA_MEM_START_ADDR, 0xB2);
    i2cWriteBuffer(MPU6050_ADDRESS, MPU_RA_MEM_R_W, 2, "\xFF\xFF"); // data

    mpu6050DmpBankSelect(0x01); // bank
    i2cWrite(MPU6050_ADDRESS, MPU_RA_MEM_START_ADDR, 0x90);
    i2cWriteBuffer(MPU6050_ADDRESS, MPU_RA_MEM_R_W, 4, "\x09\x23\xA1\x35"); // data

    i2cRead(MPU6050_ADDRESS, MPU_RA_USER_CTRL, 1, &temp);

    i2cWrite(MPU6050_ADDRESS, MPU_RA_USER_CTRL, 0x04);      // fifo reset

    // Insert FIFO count read?
    mpu6050DmpFifoReady();

    i2cWrite(MPU6050_ADDRESS, MPU_RA_USER_CTRL, 0x00); // ?? I think this enables a lot of stuff but disables fifo
    i2cWrite(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_1, 0x03); // CLKSEL =  PLL w Z ref
    delay(2);

    i2cRead(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_2, 1, &temp);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_2, 0x00); 
    i2cRead(MPU6050_ADDRESS, MPU_RA_ACCEL_CONFIG, 1, &temp);

    i2cWrite(MPU6050_ADDRESS, MPU_RA_ACCEL_CONFIG, 0x00); // full scale range +/- 2g
    delay(2);
    i2cRead(MPU6050_ADDRESS, MPU_RA_PWR_MGMT_1, 1, &temp);

    i2cWrite(MPU6050_ADDRESS, MPU_RA_MOT_THR, 0x02);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_ZRMOT_THR, 0x9C);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_MOT_DUR, 0x50);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_ZRMOT_DUR, 0x00);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_USER_CTRL, 0x04);      // fifo reset
    i2cWrite(MPU6050_ADDRESS, MPU_RA_USER_CTRL, 0x00);
    i2cWrite(MPU6050_ADDRESS, MPU_RA_USER_CTRL, 0xC8);      // fifo enable

    mpu6050DmpBankSelect(0x01); // bank
    i2cWrite(MPU6050_ADDRESS, MPU_RA_MEM_START_ADDR, 0x6A);
    i2cWriteBuffer(MPU6050_ADDRESS, MPU_RA_MEM_R_W, 2, "\x06\x00"); // data

    mpu6050DmpBankSelect(0x01); // bank
    i2cWrite(MPU6050_ADDRESS, MPU_RA_MEM_START_ADDR, 0x60);
    i2cWriteBuffer(MPU6050_ADDRESS, MPU_RA_MEM_R_W, 8, "\x00\x00\x00\x00\x00\x00\x00\x00"); // data

    mpu6050DmpBankSelect(0x00); // bank
    i2cWrite(MPU6050_ADDRESS, MPU_RA_MEM_START_ADDR, 0x60);
    i2cWriteBuffer(MPU6050_ADDRESS, MPU_RA_MEM_R_W, 4, "\x40\x00\x00\x00"); // data
}

#else                          /* MPU6050_DMP */
void mpu6050DmpLoop(void)
{

}

void mpu6050DmpResetFifo(void)
{

}
#endif                          /* MPU6050_DMP */
