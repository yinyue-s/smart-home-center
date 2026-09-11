#include "mpu6050.h"

/**
  * @brief  MPU6050 ��ʼ��
  * @retval 0: �ɹ�, 1: ʧ�ܣ�WHO_AM_I ��ȡ����
  */
uint8_t MPU6050_Init(void)
{
    uint8_t check = 0;
    uint8_t data = 0;
    HAL_StatusTypeDef ret;

    // 1. ȷ���豸����
    ret = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 100);
    if(ret != HAL_OK || check != 0x68)
    {
        return 1;  // �豸δ��Ӧ
    }

    // 2. �������ģʽ�����ѣ�
    data = 0x00;  // CLK_SEL=0, 使用内部8MHz振荡器
    ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 100);
    if(ret != HAL_OK) return 2;
    
    // 等待传感器稳定（从睡眠模式唤醒需要时间）
    HAL_Delay(100);

    // 3. ���ò����ʷ�Ƶ��125Hz (SMPLRT_DIV = 7, 1kHz / (7+1) = 125Hz)
    data = 0x07;
    ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 100);
    if(ret != HAL_OK) return 3;

    // 4. �������ֵ�ͨ�˲�����DLPF������Լ44Hz
    data = 0x03;  // DLPF_CFG=3, 44Hz
    ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, 100);
    if(ret != HAL_OK) return 4;

    // 5. �������������̣���250��/s
    data = 0x00;  // FS_SEL=0, ±250°/s
    ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 100);
    if(ret != HAL_OK) return 5;

    // 6. ���ü��ٶȼ����̣���2g
    data = 0x00;  // AFS_SEL=0, ±2g
    ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 100);
    if(ret != HAL_OK) return 6;

    // 7. 禁用I2C主模式（确保MPU6050作为从设备）
    data = 0x00;
    ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x25, 1, &data, 1, 100);  // I2C_MST_CTRL
    if(ret != HAL_OK) return 7;

    // 等待传感器完成初始化
    HAL_Delay(50);

    return 0;
}

/**
  * @brief  ��ȡ MPU6050 ȫ�����ݣ����ٶ�+������+�¶ȣ�
  * @param  ax,ay,az : ���ٶȼ��������λ g��
  * @param  gx,gy,gz : �������������λ ��/s��
  * @note   ������Ҫĳ���ݣ��ɴ��� NULL ָ��
  */
void MPU6050_ReadData(float *ax, float *ay, float *az, float *gx, float *gy, float *gz)
{
    int16_t raw[6];
    uint8_t buf[14];

    // ��0x3B��ʼ��ȡ14���ֽڣ����ٶ�+�¶�+�����ǣ�
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x3B, 1, buf, 14, 100);

    // ƴ�Ӹߵ��ֽ�
    raw[0] = (buf[0] << 8) | buf[1];  // ax
    raw[1] = (buf[2] << 8) | buf[3];  // ay
    raw[2] = (buf[4] << 8) | buf[5];  // az
    raw[3] = (buf[8] << 8) | buf[9];  // gx
    raw[4] = (buf[10] << 8) | buf[11];// gy
    raw[5] = (buf[12] << 8) | buf[13];// gz

    // ��ָ�뱣������ֹ����
    if (ax != NULL) *ax = raw[0] / ACCEL_SCALE_FACTOR;
    if (ay != NULL) *ay = raw[1] / ACCEL_SCALE_FACTOR;
    if (az != NULL) *az = raw[2] / ACCEL_SCALE_FACTOR;
    if (gx != NULL) *gx = raw[3] / GYRO_SCALE_FACTOR;
    if (gy != NULL) *gy = raw[4] / GYRO_SCALE_FACTOR;
    if (gz != NULL) *gz = raw[5] / GYRO_SCALE_FACTOR;
}

/**
  * @brief  ����ȡ���ٶ����ݣ�Ч�ʸ��ߣ�
  * @param  ax,ay,az : ���ٶȼ��������λ g��
  */
void MPU6050_ReadAccel(float *ax, float *ay, float *az)
{
    int16_t raw[3];
    uint8_t buf[6];
    HAL_StatusTypeDef ret;

    // �ӼĴ��� 0x3B ��ʼ��ȡ 6 �ֽڣ����ٶ� X/Y/Z��
    ret = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H, 1, buf, 6, 100);
    
    if(ret != HAL_OK)
    {
        // I2C����ʧ败�������
        *ax = 0.0f;
        *ay = 0.0f;
        *az = 0.0f;
        return;
    }

    raw[0] = (buf[0] << 8) | buf[1];
    raw[1] = (buf[2] << 8) | buf[3];
    raw[2] = (buf[4] << 8) | buf[5];

    *ax = raw[0] / ACCEL_SCALE_FACTOR;
    *ay = raw[1] / ACCEL_SCALE_FACTOR;
    *az = raw[2] / ACCEL_SCALE_FACTOR;
}

/**
  * @brief  ���ü��ٶȼƼ��㸩���Ǻͺ���ǣ���̬�Ƕȣ�
  * @param  pitch : �����ǣ��������λ�ȣ�
  * @param  roll  : ����ǣ��������λ�ȣ�
  * @note   �����������������ǣ���̬ʹ��ʱ������Ӱ��ϴ�
  */
void MPU6050_GetAngle(float *pitch, float *roll)
{
    float ax, ay, az;
    MPU6050_ReadAccel(&ax, &ay, &az);

    // ���㹫ʽ����������ϵ��X��ǰ��Y���ң�Z���£�
    *pitch = atan2(ay, sqrt(ax*ax + az*az)) * 57.2958f;
    *roll  = atan2(-ax, sqrt(ay*ay + az*az)) * 57.2958f;
}