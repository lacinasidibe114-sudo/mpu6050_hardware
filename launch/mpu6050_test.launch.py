from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import Command
from launch_ros.parameter_descriptions import ParameterValue
from launch.actions import RegisterEventHandler
from launch.event_handlers import OnProcessExit
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg_path = get_package_share_directory('mpu6050_hardware')
    
    robot_description_content = Command([
        'xacro ', os.path.join(pkg_path, 'urdf', 'mpu6050_robot.urdf.xacro')
    ])
    robot_description = ParameterValue(robot_description_content, value_type=str)

    controllers_yaml = os.path.join(pkg_path, 'config', 'controllers.yaml')

    control_node = Node(
        package='controller_manager',
        executable='ros2_control_node',
        parameters=[
            {'robot_description': robot_description},
            controllers_yaml
        ],
        output='screen',
    )

    # Publie le TF
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': robot_description}],
        output='screen',
    )

    imu_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['imu_sensor_broadcaster', '--controller-manager', '/controller_manager'],
        output='screen',
    )

    return LaunchDescription([
        control_node,
        robot_state_publisher,
        imu_broadcaster_spawner
    ])