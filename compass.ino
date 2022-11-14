///////////COMPASS////////////////

#include "MPU9250.h"
#include "eeprom_utils.h"


MPU9250 mpu;
static uint32_t prev_compass_ms = 0;
float Z = 0;

float yaw_rad = 0;
float heading_d = 0;

const float deg2rad = 0.0174533;


void compass_setup(){
Wire.begin();
    delay(2000);
      mpu.verbose(true);
    
    MPU9250Setting setting;
    setting.accel_fs_sel = ACCEL_FS_SEL::A2G;
    setting.gyro_fs_sel = GYRO_FS_SEL::G250DPS;
    setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
    setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ;
    setting.gyro_fchoice = 0x03;
    setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_41HZ;
    setting.accel_fchoice = 0x01;
    setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_45HZ;

    if (!mpu.setup(0x68,setting)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }

   mpu.setFilterIterations(10);
   mpu.setMagneticDeclination(2.1);

#if defined(ESP_PLATFORM) || defined(ESP8266)
    EEPROM.begin(0x80);
#endif

    delay(1000);



   //load factory

   if(true){
    mpu.verbose(true);
     calibrate_gyro();
     //calibrate_mag();
     saveCalibration();
   }

   
   
   // load from eeprom
   loadCalibration();
   set_default_calibration();
   printCalibration();
   mpu.selftest();
   

/*
  //wait for the mpu to update. at most there is a new mag reading each 10ms, so we need a delay
  int compassInitCounter = 0;
  while(compassInitCounter<200){
    delay(10);
    if(mpu.update()){
      compassInitCounter++;
    }
   }

   heading_float = mpu.getYaw();
   if(heading_float<0) heading_float += 360;
   Serial.println("initial heading "+String(heading_float,2));
   */
   mpu.verbose(false);
}

void compass_loop(){   
  if (mpu.update()) {
    get_heading_d();
      getCompass();
      
      //print_roll_pitch_yaw();
      //print_compass();
      //print_roll_pitch_yaw();
      //printQuaternion();
      //print_gyro();
    }
}

void getCompass(){
  //filter the compass

  float dt = float(millis() - prev_compass_ms) / 1000.0;
  prev_compass_ms = millis();


  //get the cosine ans sine of yaw for filtering purposes
   float mpu_yaw_rad = mpu.getYaw() * 0.0174532;
  
   float cos_yaw = cos(yaw_rad)*0.9 + cos(mpu_yaw_rad) * 0.1;
   float sin_yaw = sin(yaw_rad)*0.9 + sin(mpu_yaw_rad) * 0.1;

  yaw_rad = atan2(sin_yaw, cos_yaw);

  float heading_mag = yaw_rad*57.295779;
 
  if(heading_mag<0)heading_mag += 360;
  float diffMag = headingDiff(heading_mag,heading_float);



  //now fuse the sensors. the gyro and magnetometer.
  heading_float = heading_add(heading_float,heading_d*dt*0.99 + diffMag * 0.01);
  //heading_float = heading_mag;
  if( heading_float < 0 ) heading_float += 360; 
  
}

void get_heading_d(){
  float rad_X = mpu.getEulerX()*deg2rad;
  float rad_Y = mpu.getEulerY()*deg2rad;

  
 heading_d = heading_d*0.9 + 0.1 * (-mpu.getGyroZ()*cos(rad_X)*cos(rad_Y) - sin(rad_X) * mpu.getGyroY() + sin(rad_Y)*mpu.getGyroX());
 
}

void print_roll_pitch_yaw() {

    Serial.print("Yaw:");
    Serial.print(mpu.getYaw(), 2);
    Serial.print(",roll:");
    Serial.print(mpu.getRoll(), 2);
    Serial.print(",pitch:");
    Serial.print(mpu.getPitch(), 2);
    Serial.println();
}

void print_compass(){
    Serial.print("heading:");
    Serial.print(heading_float, 2);
    Serial.print(",mpu_yaw:");
    Serial.print(mpu.getYaw(), 2);
    //Serial.print(",heading_d:");
    //Serial.print(heading_d, 2);
    Serial.println();
}


void printQuaternion(){
  Serial.print("X:");
  Serial.print(mpu.getQuaternionX());
  Serial.print(",Y:");
  Serial.print(mpu.getQuaternionY());
  Serial.print(",Z:");
  Serial.print(mpu.getQuaternionZ());
  Serial.print(",W:");
  Serial.print(mpu.getQuaternionW());
  Serial.println();
}

void print_gyro(){    
  float rad_X = mpu.getEulerX()*deg2rad;
  float rad_Y = mpu.getEulerY()*deg2rad;
  float Z = -mpu.getGyroZ()*cos(rad_X)*cos(rad_Y) - sin(rad_X) * mpu.getGyroY() + sin(rad_Y)*mpu.getGyroX();
  Serial.print (",GX:");
  Serial.print (mpu.getGyroX());
  Serial.print (",GY:");
  Serial.print (mpu.getGyroY());
  Serial.print (",GZ:");
  Serial.print (-mpu.getGyroZ());
  Serial.print (",heading_d:");
  Serial.print(heading_d);
  Serial.print (",Ghor:");
  Serial.print (Z);
  
  //Serial.print (",heading_d:");
  //Serial.print (heading_d);
  Serial.println();
}

void calibrate_gyro(){
  // calibrate anytime you want to
    Serial.println("Accel Gyro calibration will start in 5sec.");
    Serial.println("Please leave the device still on the flat plane.");
    mpu.verbose(true);
    delay(1000);
    mpu.calibrateAccelGyro();
  Serial.println("Accel Gyro calibration Done.");
    

}

void calibrate_mag(){
  Serial.println("Mag calibration will start in 1sec.");
    Serial.println("Please Wave device in a figure eight until done.");
    delay(1000);
    mpu.calibrateMag();
  Serial.println("Mag calibration.");
    print_calibration();
    mpu.verbose(false);

    // save to eeprom
}

void print_calibration() {
    Serial.println("< calibration parameters >");
    Serial.println("accel bias [g]: ");
    Serial.print(mpu.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.println();
    Serial.println("gyro bias [deg/s]: ");
    Serial.print(mpu.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.println();
    Serial.println("mag bias [mG]: ");
    Serial.print(mpu.getMagBiasX());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasY());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasZ());
    Serial.println();
    Serial.println("mag scale []: ");
    Serial.print(mpu.getMagScaleX());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleY());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleZ());
    Serial.println();
}

//this is a good working calibration for me currently. This is user dependent
void set_default_calibration(){
  mpu.setAccBias(-4.72, 32.94, 11.24);
  mpu.setGyroBias(-3.10,0.51,-0.45);
  mpu.setMagBias(14.24,262.60,-224.81);
  mpu.setMagScale(0.87,0.91,1.33);
}
