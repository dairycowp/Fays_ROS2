#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <memory>
#include <atomic>
#include <iostream>
#include "fays_atrak/fays_atrak_types.h"
#include "fays_atrak/fays_atrak_vimod.h"

class FaysViNode : public rclcpp::Node
{
public:
    FaysViNode(const std::string& config_path)
    : Node("fays_vi_node"), mbIsRunning_(true)
    {
        left_image_pub_ = this->create_publisher<sensor_msgs::msg::Image>("cam0/image_raw", 10);
        right_image_pub_ = this->create_publisher<sensor_msgs::msg::Image>("cam1/image_raw", 10);
        imu_pub_ = this->create_publisher<sensor_msgs::msg::Imu>("imu/data", 50);

        FAYS_ATRAK_AIM_CreateHandleWithConfig(&mptrHandle_, config_path.c_str());
        mptrImgThr_ = std::make_unique<std::thread>(&FaysViNode::ImgOnlineCapture, this);
        mptrImuThr_ = std::make_unique<std::thread>(&FaysViNode::ImuOnlineCapture, this);
    }

    ~FaysViNode()
    {
        mbIsRunning_ = false;
        if (mptrImgThr_ && mptrImgThr_->joinable()) mptrImgThr_->join();
        if (mptrImuThr_ && mptrImuThr_->joinable()) mptrImuThr_->join();
        FAYS_ATRAK_AIM_DestroyHandle(mptrHandle_);
    }

private:
    void ImgOnlineCapture()
    {
        FAYS_ATRAK_AIM_SetStereoFPS(mptrHandle_);
        FAYS_ATRAK_AIM_SetStereoExposure(mptrHandle_, 500.);
        FAYS_ATRAK_AIM_SetStereoGain(mptrHandle_, 1.2);

        while (mbIsRunning_)
        {
            if (EXIT_SUCCESS == FAYS_ATRAK_AIM_GetStereoFrames(mptrHandle_, &mImgData_))
            {
                if (mImgData_.height > 0 && mImgData_.width > 0 && mImgData_.data != nullptr) {
                    int half_height = mImgData_.height / 2;
                    cv::Mat stereo_img(mImgData_.height, mImgData_.width, CV_8UC1, mImgData_.data);
                    // 上半部分为左图
                    cv::Mat right_img = stereo_img(cv::Rect(0, 0, mImgData_.width, half_height)).clone();
                    // 下半部分为右图
                    cv::Mat left_img = stereo_img(cv::Rect(0, half_height, mImgData_.width, half_height)).clone();

                    // 分别用 left_img 和 right_img 构造消息
                    auto left_msg = cv_bridge::CvImage(std_msgs::msg::Header(), "mono8", left_img).toImageMsg();
                    left_msg->header.stamp = this->now();
                    auto right_msg = cv_bridge::CvImage(std_msgs::msg::Header(), "mono8", right_img).toImageMsg();
                    right_msg->header.stamp = this->now();

                    left_image_pub_->publish(*left_msg);
                    right_image_pub_->publish(*right_msg);
                }
            }
        }
    }

    void ImuOnlineCapture()
    {
        using namespace std::chrono;
        auto last_pub = steady_clock::now();

        while (mbIsRunning_)
        {
            if (FAYS_ATRAK_AIM_GetImuData(mptrHandle_, &mImuData_) == EXIT_SUCCESS)
            {
                auto now = steady_clock::now();
                // 200Hz = 5ms
                if (duration_cast<microseconds>(now - last_pub).count() >= 5000)
                {
                    sensor_msgs::msg::Imu imu_msg;
                    imu_msg.header.stamp = this->now();
                    imu_msg.linear_acceleration.x = mImuData_.acc[0];
                    imu_msg.linear_acceleration.y = mImuData_.acc[1];
                    imu_msg.linear_acceleration.z = mImuData_.acc[2];
                    imu_msg.angular_velocity.x = mImuData_.gyro[0];
                    imu_msg.angular_velocity.y = mImuData_.gyro[1];
                    imu_msg.angular_velocity.z = mImuData_.gyro[2];

                    // Set covariance using config values
                    constexpr double acc_noise_density = 1.86e-03;
                    constexpr double gyr_noise_density = 1.87e-04;

                    imu_msg.linear_acceleration_covariance[0] = acc_noise_density * acc_noise_density;
                    imu_msg.linear_acceleration_covariance[4] = acc_noise_density * acc_noise_density;
                    imu_msg.linear_acceleration_covariance[8] = acc_noise_density * acc_noise_density;

                    imu_msg.angular_velocity_covariance[0] = gyr_noise_density * gyr_noise_density;
                    imu_msg.angular_velocity_covariance[4] = gyr_noise_density * gyr_noise_density;
                    imu_msg.angular_velocity_covariance[8] = gyr_noise_density * gyr_noise_density;

                    imu_pub_->publish(imu_msg);
                    last_pub = now;
                }
            }
        }
    }

    void* mptrHandle_{nullptr};
    std::unique_ptr<std::thread> mptrImgThr_;
    std::unique_ptr<std::thread> mptrImuThr_;
    AtrakIMU mImuData_;
    AtrakImage mImgData_;
    std::atomic<bool> mbIsRunning_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr left_image_pub_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr right_image_pub_;
    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    if (argc < 2) {
        std::cerr << "Usage: ros2 run <pkg> <node> <config.yaml>" << std::endl;
        return 1;
    }
    auto node = std::make_shared<FaysViNode>(argv[1]);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
