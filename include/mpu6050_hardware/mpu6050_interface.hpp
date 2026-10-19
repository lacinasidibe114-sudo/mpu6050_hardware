#ifndef MPU6050_HARDWARE__MPU6050_INTERFACE_HPP_
#define MPU6050_HARDWARE__MPU6050_INTERFACE_HPP_

#include <string>
#include <vector>
#include <libserial/SerialPort.h>
#include <array>

#include "hardware_interface/sensor_interface.hpp"
#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/state.hpp"

namespace mpu6050_hardware
{

class MPU6050Interface : public hardware_interface::SensorInterface
{
public:
  hardware_interface::CallbackReturn on_init(
    const hardware_interface::HardwareInfo & info) override;

  hardware_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;

  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  hardware_interface::return_type read(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

private:
  LibSerial::SerialPort serial_port_;
  std::string device_;

  // Tableaux pour stocker les données (x, y, z, w pour l'orientation)
  std::array<double, 4> hw_orientation_;          // x, y, z, w
  std::array<double, 3> hw_angular_velocity_;     // x, y, z
  std::array<double, 3> hw_linear_acceleration_;  // x, y, z
};

}  // namespace mpu6050_hardware

#endif