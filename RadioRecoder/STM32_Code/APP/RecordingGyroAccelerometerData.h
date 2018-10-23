#ifndef __RECODINGGYROACCELEROMETERDATA_H
#define __RECODINGGYROACCELEROMETERDATA_H			
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "sys.h"  

void gad_rx_callback(void);
u8 gad_recorder(u8 key,u32 time,u32 fs);
void gad_second_level_cache(void);
#endif 

