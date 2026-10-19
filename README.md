```markdown
# MPU6050 Hardware Interface (ROS 2)

Ce package fournit une interface matérielle (`ros2_control` **SensorInterface**) pour lire les données d'un capteur inertiel **MPU6050** via un Arduino.

Ce package fait partie d'une suite d'outils (Toolkit) pour la robotique.

## 🏗️ Architecture

Le système lit les données brutes de l'IMU via I2C sur l'Arduino, les envoie par USB Série, et l'interface C++ les convertit dans les unités du Système International (SI) pour ROS 2.

```text
+-------------------------------------------------------------+
|                       ORDINATEUR (ROS 2)                    |
|                                                             |
|  [imu_sensor_broadcaster] <── Publie sensor_msgs/msg/Imu    |
|         ▲                                                   |
|         │                                                   |
|  [mpu6050_hardware (C++ SensorInterface)]                   |
|         │                                                   |
|         │ 1. Parsing de la trame série                      |
|         │ 2. Conversion Euler (Roll,Pitch,Yaw) -> Quaternion |
|         ▼                                                   |
|  [libserial] ──────────────┐                                |
+----------------------------|--------------------------------+
                             | USB Série (115200 bauds)
                             | Trame: "R:0.0 P:0.0 Y:0.0 ..."
                             ▼
+-------------------------------------------------------------+
|                       ARDUINO (C++)                         |
|                                                             |
|  [MPU6050_light.h] <── [Bus I2C] <── [Capteur MPU6050]      |
+-------------------------------------------------------------+
```

## 📂 Structure du Package

```text
mpu6050_hardware/
├── CMakeLists.txt
├── package.xml
├── mpu6050_hardware.xml        # Manifeste du plugin
├── arduino/
│   └── mpu6050_firmware.ino    # Code Arduino (Lecture I2C)
├── config/
│   └── controllers.yaml        # Config de l'imu_sensor_broadcaster
├── include/mpu6050_hardware/
│   └── mpu6050_interface.hpp
├── launch/
│   └── mpu6050_test.launch.py
├── src/
│   └── mpu6050_interface.cpp   # Code C++ (conversion tf2)
└── urdf/
    ├── mpu6050_robot.urdf.xacro
    └── mpu6050_control.xacro
```

## ⚙️ Fonctionnalités Clés

- **Conversion Quaternion** : Utilise la bibliothèque `tf2` pour convertir les angles d'Euler (envoyés par l'Arduino) en Quaternions, le format standard de ROS 2 pour l'orientation.
- **Unités SI** : Les données sont converties en radians, rad/s, et m/s² directement depuis l'Arduino.
- **SensorInterface** : Utilise la classe `hardware_interface::SensorInterface` (et non `SystemInterface`), ce qui est le standard pour les capteurs passifs.

## 🚀 Utilisation

**1. Téléverser le code Arduino**
Ouvrez `arduino/mpu6050_firmware.ino` dans l'IDE Arduino et téléversez-le (nécessite la bibliothèque `MPU6050_light`).

**2. Compiler et lancer**
```bash
colcon build --packages-select mpu6050_hardware
source install/setup.bash
ros2 launch mpu6050_hardware mpu6050_test.launch.py
```

**3. Vérifier les données (dans un autre terminal)**
```bash
ros2 topic echo /imu_sensor_broadcaster/imu
```

## 👨‍💻 Auteur
Lacina Sidibe (PFE 2027)
