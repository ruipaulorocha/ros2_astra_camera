/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2013-2022 Orbbec 3D Technology, Inc                      */
/*                                                                        */
/* PROPRIETARY RIGHTS of Orbbec 3D Technology are involved in the         */
/* subject matter of this material. All manufacturing, reproduction, use, */
/* and sales rights pertaining to this subject matter are governed by the */
/* license agreement. The recipient of this software implicitly accepts   */
/* the terms of the license.                                              */
/*                                                                        */
/**************************************************************************/

#pragma once
#include <glog/logging.h>
#include <optional>
#include <atomic>

#include <rclcpp/rclcpp.hpp>
#include <libuvc/libuvc.h>
#include <image_transport/image_transport.hpp>
#include <camera_info_manager/camera_info_manager.hpp>

#include <sensor_msgs/msg/camera_info.hpp>

#include "types.h"

namespace astra_camera {
struct UVCCameraConfig {
  int vendor_id = 0;
  int product_id = 0;
  int width = 0;
  int height = 0;
  int fps = 0;
  int index = 0;
  std::string serial_number;
  std::string format;
  int retry_count = 0;
  UVCCameraConfig() = default;
  UVCCameraConfig(const UVCCameraConfig&) = default;
  UVCCameraConfig(UVCCameraConfig&&) = default;
  UVCCameraConfig& operator=(const UVCCameraConfig&) = default;
  UVCCameraConfig& operator=(UVCCameraConfig&&) = default;
};

// uvc camera config printer
std::ostream& operator<<(std::ostream& os, const UVCCameraConfig& config);

class UVCCameraDriver {
 public:
  explicit UVCCameraDriver(rclcpp::Node* node, UVCCameraConfig config);

  ~UVCCameraDriver();

  void updateConfig(const UVCCameraConfig& config);

  void setVideoMode();

  void startStreaming();

  void stopStreaming();

  int getResolutionX() const;

  int getResolutionY() const;

 private:
  void setupCameraControlService();

  sensor_msgs::msg::CameraInfo getCameraInfo();

  enum uvc_frame_format UVCFrameFormatString(const std::string& format);

  static void frameCallbackWrapper(uvc_frame_t* frame, void* ptr);

  void frameCallback(uvc_frame_t* frame);

  std::string getNoSlashNamespace();

  void autoControlsCallback(enum uvc_status_class status_class, int event, int selector,
                            enum uvc_status_attribute status_attribute, void* data,
                            size_t data_len);
  static void autoControlsCallbackWrapper(enum uvc_status_class status_class, int event,
                                          int selector, enum uvc_status_attribute status_attribute,
                                          void* data, size_t data_len, void* ptr);

  void openCamera();

  bool getUVCExposureCb(const std::shared_ptr<GetInt32::Request>& request,
                        std::shared_ptr<GetInt32::Response>& response);

  bool setUVCExposureCb(const std::shared_ptr<SetInt32::Request>& request,
                        std::shared_ptr<SetInt32::Response>& response);

  bool getUVCGainCb(const std::shared_ptr<GetInt32::Request>& request,
                    std::shared_ptr<GetInt32::Response>& response);

  bool setUVCGainCb(const std::shared_ptr<SetInt32::Request>& request,
                    std::shared_ptr<SetInt32::Response>& response);

  bool getUVCWhiteBalanceCb(const std::shared_ptr<GetInt32::Request>& request,
                            std::shared_ptr<GetInt32::Response>& response);

  bool setUVCWhiteBalanceCb(const std::shared_ptr<SetInt32::Request>& request,
                            std::shared_ptr<SetInt32::Response>& response);

  bool setUVCAutoExposureCb(const std::shared_ptr<SetBool::Request>& request,
                            std::shared_ptr<SetBool::Response>& response);

  bool setUVCAutoWhiteBalanceCb(const std::shared_ptr<SetBool::Request>& request,
                                std::shared_ptr<SetBool::Response>& response);

  bool getUVCMirrorCb(const std::shared_ptr<GetInt32::Request>& request,
                      std::shared_ptr<GetInt32::Response>& response);

  bool setUVCMirrorCb(const std::shared_ptr<SetBool::Request>& request,
                      std::shared_ptr<SetBool::Response>& response);

  bool toggleUVCCamera(const std::shared_ptr<SetBool::Request>& request,
                       std::shared_ptr<SetBool::Response>& response);

 private:
  rclcpp::Node* node_;
  rclcpp::Logger logger_;
  UVCCameraConfig config_;
  std::string frame_id_;
  uvc_context_t* ctx_ = nullptr;
  uvc_device_t* device_ = nullptr;
  uvc_device_handle_t* device_handle_ = nullptr;
  uvc_frame_t* frame_buffer_ = nullptr;
  uvc_stream_ctrl_t ctrl_{};
  int uvc_flip_{0};
  std::atomic_bool is_streaming_started{false};

  rclcpp::Service<GetInt32>::SharedPtr get_uvc_exposure_srv_;
  rclcpp::Service<SetInt32>::SharedPtr set_uvc_exposure_srv_;
  rclcpp::Service<GetInt32>::SharedPtr get_uvc_gain_srv_;
  rclcpp::Service<SetInt32>::SharedPtr set_uvc_gain_srv_;
  rclcpp::Service<GetInt32>::SharedPtr get_uvc_white_balance_srv_;
  rclcpp::Service<SetInt32>::SharedPtr set_uvc_white_balance_srv_;
  rclcpp::Service<SetBool>::SharedPtr set_uvc_auto_exposure_srv_;
  rclcpp::Service<SetBool>::SharedPtr set_uvc_auto_white_balance_srv_;
  rclcpp::Service<GetInt32>::SharedPtr get_uvc_mirror_srv_;
  rclcpp::Service<SetBool>::SharedPtr set_uvc_mirror_srv_;
  rclcpp::Service<SetBool>::SharedPtr toggle_uvc_camera_srv_;

  image_transport::Publisher image_publisher_;
  rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr camera_info_publisher_;
  std::optional<sensor_msgs::msg::CameraInfo> camera_info_;
  rclcpp::Client<GetCameraInfo>::SharedPtr get_camera_info_cli_;
};

}  // namespace astra_camera
