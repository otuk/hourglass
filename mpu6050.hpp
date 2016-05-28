#ifndef _MPU6050_HPP
#define _MPU6050_HPP

extern float ypr[3];// [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
void init_mpu();
void read_mpu();

#endif
