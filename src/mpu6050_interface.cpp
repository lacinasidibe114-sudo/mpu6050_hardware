#include "mpu6050_hardware/mpu6050_interface.hpp"
#include <cmath>
#include <cstdio>
#include <tf2/LinearMath/Quaternion.h> // Pour la conversion Euler -> Quaternion

#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "pluginlib/class_list_macros.hpp"

namespace mpu6050_hardware
{

hardware_interface::CallbackReturn MPU6050Interface::on_init(
  const hardware_interface::HardwareInfo & info)
{
  if (hardware_interface::SensorInterface::on_init(info) !=
      hardware_interface::CallbackReturn::SUCCESS)
  {
    return hardware_interface::CallbackReturn::ERROR;
  }

  device_ = info_.hardware_parameters["device"];

  // Initialisation des tableaux à zéro
  hw_orientation_ = {0.0, 0.0, 0.0, 1.0}; // Quaternion par défaut (w=1)
  hw_angular_velocity_ = {0.0, 0.0, 0.0};
  hw_linear_acceleration_ = {0.0, 0.0, 0.0};

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn MPU6050Interface::on_activate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  try {
    serial_port_.Open(device_);
    serial_port_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    serial_port_.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    serial_port_.SetParity(LibSerial::Parity::PARITY_NONE);
    serial_port_.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
    serial_port_.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  } catch (...) {
    RCLCPP_FATAL(rclcpp::get_logger("MPU6050Hardware"), "Impossible d'ouvrir %s", device_.c_str());
    return hardware_interface::CallbackReturn::ERROR;
  }

  RCLCPP_INFO(rclcpp::get_logger("MPU6050Hardware"), "Port %s ouvert.", device_.c_str());
  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn MPU6050Interface::on_deactivate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  if (serial_port_.IsOpen()) {
    serial_port_.Close();
  }
  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface> MPU6050Interface::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;
  
  // L'IMU est déclarée sous le nom "imu_sensor" dans le Xacro
  std::string sensor_name = info_.sensors[0].name;

  // Interfaces d'orientation
  state_interfaces.emplace_back(sensor_name, "orientation.x", &hw_orientation_[0]);
  state_interfaces.emplace_back(sensor_name, "orientation.y", &hw_orientation_[1]);
  state_interfaces.emplace_back(sensor_name, "orientation.z", &hw_orientation_[2]);
  state_interfaces.emplace_back(sensor_name, "orientation.w", &hw_orientation_[3]);

  // Interfaces de vitesse angulaire
  state_interfaces.emplace_back(sensor_name, "angular_velocity.x", &hw_angular_velocity_[0]);
  state_interfaces.emplace_back(sensor_name, "angular_velocity.y", &hw_angular_velocity_[1]);
  state_interfaces.emplace_back(sensor_name, "angular_velocity.z", &hw_angular_velocity_[2]);

  // Interfaces d'accélération linéaire
  state_interfaces.emplace_back(sensor_name, "linear_acceleration.x", &hw_linear_acceleration_[0]);
  state_interfaces.emplace_back(sensor_name, "linear_acceleration.y", &hw_linear_acceleration_[1]);
  state_interfaces.emplace_back(sensor_name, "linear_acceleration.z", &hw_linear_acceleration_[2]);

  return state_interfaces;
}

hardware_interface::return_type MPU6050Interface::read(
  const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/)
{
  if (!serial_port_.IsOpen()) return hardware_interface::return_type::ERROR;

  try {
    std::string data;
    serial_port_.ReadLine(data, '\n', 10);
    
    // Trame attendue : "R:-3.0522 P:-1.3803 Y:0.0068 AX:-0.004 AY:-0.011 AZ:9.826 GX:-0.0005 GY:-0.0005 GZ:0.0027"
    float r, p, y, ax, ay, az, gx, gy, gz;
    int parsed = std::sscanf(data.c_str(), 
      "R:%f P:%f Y:%f AX:%f AY:%f AZ:%f GX:%f GY:%f GZ:%f", 
      &r, &p, &y, &ax, &ay, &az, &gx, &gy, &gz);

    if (parsed == 9) {
      // 1. Conversion Euler (Roll, Pitch, Yaw) -> Quaternion
      tf2::Quaternion q;
      q.setRPY(r, p, y);
      hw_orientation_[0] = q.x();
      hw_orientation_[1] = q.y();
      hw_orientation_[2] = q.z();
      hw_orientation_[3] = q.w();

      // 2. Remplissage des autres données
      hw_linear_acceleration_ = {ax, ay, az};
      hw_angular_velocity_ = {gx, gy, gz};
    }
  } catch (...) {
    // Timeout normal
  }

  return hardware_interface::return_type::OK;
}

}  // namespace mpu6050_hardware

PLUGINLIB_EXPORT_CLASS(mpu6050_hardware::MPU6050Interface, hardware_interface::SensorInterface)