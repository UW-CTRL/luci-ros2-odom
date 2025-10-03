# luci-ros2-odom

This package is a ROS 2 (Robot Operating System) package designed to handle odometry for the Luci robot. It provides tools and nodes to calculate and publish odometry data, enabling localization and navigation functionalities.

## Features
- Odometry calculation based on sensor data.
- Publishes odometry messages to ROS 2 topics.
- Integration with other ROS 2 navigation stacks.

## Prerequisites
- ROS 2 installed (tested with Humble or later).
- A workspace set up (`ros2_ws`).

## Installation
1. Clone the repository into your ROS 2 workspace:
    ```bash
    cd ~/ros2_ws/src
    git clone https://github.com/your-repo/luci-ros2-odom.git
    ```
2. Build the workspace:
    ```bash
    cd ~/ros2_ws
    colcon build
    ```
3. Source the workspace:
    ```bash
    source ~/ros2_ws/install/setup.bash
    ```

## Usage
1. Launch the odometry node:
    ```bash
    ros2 launch luci-ros2-odom odometry.launch.py
    ```
2. Verify the odometry topic:
    ```bash
    ros2 topic echo /odom
    ```


## Contributing
Contributions are welcome! Please fork the repository and submit a pull request.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.