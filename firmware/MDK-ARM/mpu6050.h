#ifndef __MPU6050_H
#define __MPU6050_H

#include "main.h"
#include "i2c.h"
#include "math.h"  

// ֱ�������ⲿ I2C ������� CubeMX ���ɣ�
extern I2C_HandleTypeDef hi2c1;

// �豸��ַ��AD0�ӵأ�7λ��ַ0x68������һλ�õ�8λд��ַ��
#define MPU6050_ADDR        (0x68 << 1)

// ���üĴ�����ַ
#define WHO_AM_I_REG        0x75
#define PWR_MGMT_1_REG      0x6B
#define SMPLRT_DIV_REG      0x19
#define CONFIG_REG          0x1A
#define GYRO_CONFIG_REG     0x1B
#define ACCEL_CONFIG_REG    0x1C
#define ACCEL_XOUT_H        0x3B   // ���ٶȼ���ʼ�Ĵ���

// ����ϵ������2g, ��250��/s��
#define ACCEL_SCALE_FACTOR  16384.0f
#define GYRO_SCALE_FACTOR   131.0f

// ��������
uint8_t MPU6050_Init(void);
void MPU6050_ReadData(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);
void MPU6050_ReadAccel(float *ax, float *ay, float *az);
void MPU6050_GetAngle(float *pitch, float *roll);

#endif