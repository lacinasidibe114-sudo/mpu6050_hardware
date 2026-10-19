#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

// Fréquence d'envoi (50 Hz = toutes les 20 ms)
const unsigned long interval = 20; 
unsigned long last_time = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  byte status = mpu.begin();
  if (status != 0) {
    Serial.println("Erreur : MPU6050 non connecté. Vérifie le câblage I2C (SDA/SCL).");
    while (1) {} // Bloque le programme si l'IMU n'est pas là
  }
  
  // Calibration (garde le robot immobile pendant 2 secondes au démarrage)
  delay(1000);
  mpu.calcOffsets(); 
  Serial.println("MPU6050 prêt !");
}

void loop() {
  // Rafraîchit les données du capteur (à faire le plus souvent possible)
  mpu.update();
  
  unsigned long current_time = millis();
  
  // Envoi de la trame toutes les 20ms
  if (current_time - last_time >= interval) {
    
    // 1. Angles (en degrés -> conversion en radians)
    float roll  = mpu.getAngleX() * DEG_TO_RAD;
    float pitch = mpu.getAngleY() * DEG_TO_RAD;
    float yaw   = mpu.getAngleZ() * DEG_TO_RAD;
    
    // 2. Vitesses angulaires (en degrés/s -> conversion en rad/s)
    float gyro_x = mpu.getGyroX() * DEG_TO_RAD;
    float gyro_y = mpu.getGyroY() * DEG_TO_RAD;
    float gyro_z = mpu.getGyroZ() * DEG_TO_RAD;
    
    // 3. Accélérations (en g -> conversion en m/s²)
    // 1 g = 9.80665 m/s²
    float acc_x = mpu.getAccX() * 9.80665;
    float acc_y = mpu.getAccY() * 9.80665;
    float acc_z = mpu.getAccZ() * 9.80665;
    
    // Envoi de la trame : "R:0.12 P:-0.05 Y:1.50 AX:0.1 AY:0.2 AZ:9.8 GX:0.0 GY:0.0 GZ:0.0"
    Serial.print("R:"); Serial.print(roll, 4);
    Serial.print(" P:"); Serial.print(pitch, 4);
    Serial.print(" Y:"); Serial.print(yaw, 4);
    
    Serial.print(" AX:"); Serial.print(acc_x, 3);
    Serial.print(" AY:"); Serial.print(acc_y, 3);
    Serial.print(" AZ:"); Serial.print(acc_z, 3);
    
    Serial.print(" GX:"); Serial.print(gyro_x, 4);
    Serial.print(" GY:"); Serial.print(gyro_y, 4);
    Serial.print(" GZ:"); Serial.println(gyro_z, 4);
    
    last_time = current_time;
  }
}