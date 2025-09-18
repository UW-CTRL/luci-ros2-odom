#include <chrono>
#include <memory>
#include <cmath>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2/LinearMath/Quaternion.h"
#include "luci_encoder_odometry/odometry.h"
#include "luci_messages/msg/luci_encoders.hpp"

// Define variables
float meter_conv = 0.0254; // inches to meters
float wheelDiameter = 13 * meter_conv;
float wheelCircumference = wheelDiameter * M_PI;      // meters
float wheelBase = 23 * meter_conv;               // meters
float gearRatio = 2.38;               // dimensionless
float rolloverThreshold = 100.0;       // degrees
bool rightMotorForwardIncreases = true;
bool leftMotorForwardIncreases = true;

nav_msgs::msg::Odometry createOdomMessage(const Position& pos, const Velocity& vel, const rclcpp::Time& timestamp)
{
    nav_msgs::msg::Odometry odom_msg;
    odom_msg.header.stamp = timestamp;
    odom_msg.header.frame_id = "odom";
    odom_msg.child_frame_id = "base_link";

    // Set position
    odom_msg.pose.pose.position.x = pos.x;
    odom_msg.pose.pose.position.y = pos.y;
    odom_msg.pose.pose.position.z = 0.0;

    // Convert theta to quaternion
    tf2::Quaternion q;
    q.setRPY(0, 0, pos.theta);
    odom_msg.pose.pose.orientation.x = q.x();
    odom_msg.pose.pose.orientation.y = q.y();
    odom_msg.pose.pose.orientation.z = q.z();
    odom_msg.pose.pose.orientation.w = q.w();

    // Set velocity
    odom_msg.twist.twist.linear.x = vel.linearX;
    odom_msg.twist.twist.linear.y = 0.0;
    odom_msg.twist.twist.angular.z = vel.angularZ;

    return odom_msg;
}


class EncoderToOdomNode : public rclcpp::Node
{
    public:
        EncoderToOdomNode()
        : Node("encoder_to_odom_node")
        {
            encoder_subscriber_ = this->create_subscription<luci_messages::msg::LuciEncoders>(
                "luci/encoders", 10, std::bind(&EncoderToOdomNode::encoder_callback, this, std::placeholders::_1));

            odom_publisher_ = this->create_publisher<nav_msgs::msg::Odometry>("odom", 10);
            
            // TF broadcaster
            tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        }
    private:
        void encoder_callback(const luci_messages::msg::LuciEncoders::SharedPtr msg)
        {
            auto now = this->get_clock()->now();
            odometry_processor_.updateTimestamp(msg->edge_timestamp);
            odometry_processor_.updateEncoderReading(Motor::LEFT, msg->left_angle);
            odometry_processor_.updateEncoderReading(Motor::RIGHT, msg->right_angle);

            odometry_processor_.processData();
            Position pos = odometry_processor_.getPosition();
            Velocity vel = odometry_processor_.getVelocity();
            auto odom_msg = createOdomMessage(pos, vel, now);
            odom_publisher_->publish(odom_msg);

            // Broadcast TF
            geometry_msgs::msg::TransformStamped odom_tf;
            odom_tf.header.stamp = now;
            odom_tf.header.frame_id = "odom";
            odom_tf.child_frame_id = "base_link";
            odom_tf.transform.translation.x = pos.x;
            odom_tf.transform.translation.y = pos.y;
            odom_tf.transform.translation.z = 0.0;
            tf2::Quaternion q;
            q.setRPY(0, 0, pos.theta);
            odom_tf.transform.rotation.x = q.x();
            odom_tf.transform.rotation.y = q.y();
            odom_tf.transform.rotation.z = q.z();
            odom_tf.transform.rotation.w = q.w();
            tf_broadcaster_->sendTransform(odom_tf);
    }
        

        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_publisher_;
        rclcpp::Subscription<luci_messages::msg::LuciEncoders>::SharedPtr encoder_subscriber_;
        std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
        
        OdometryProcessor odometry_processor_{wheelCircumference, wheelBase, gearRatio, rolloverThreshold, rightMotorForwardIncreases, leftMotorForwardIncreases};
};



int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<EncoderToOdomNode>());
    rclcpp::shutdown();
    return 0;
}