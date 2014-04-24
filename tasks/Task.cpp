/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"

using namespace camstreamer;

Task::Task(std::string const& name)
    : TaskBase(name)
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine)
{
}

Task::~Task()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

bool Task::configureHook()
{
    if (! TaskBase::configureHook())
        return false;
    return true;
}
bool Task::startHook()
{
    if (! TaskBase::startHook())
        return false;

    streamer = new VlcStream(_sout, _fps, _camera_image_width, _camera_image_height);

    streamer->start();


    return true;
}
void Task::updateHook()
{
    TaskBase::updateHook();

  int image_read = _camera_image.read(current_image_);
  bool new_image = image_read == RTT::NewData;
  cv::Mat mat;

  if(image_read != RTT::NoData && new_image && current_image_->getStatus() == base::samples::frame::STATUS_VALID)
  {
    switch(current_image_->getFrameMode())
    {
      case base::samples::frame::MODE_RGB:
      case base::samples::frame::MODE_GRAYSCALE:
      {
        mat = frame_helper::FrameHelper::convertToCvMat(*current_image_);
        break;
      }
      case base::samples::frame::MODE_BAYER_RGGB:
      case base::samples::frame::MODE_BAYER_GRBG:
      case base::samples::frame::MODE_BAYER_BGGR:
      case base::samples::frame::MODE_BAYER_GBRG:
      {
        frame_helper::FrameHelper::convertBayerToRGB24(*current_image_, debayered_image);
        mat = frame_helper::FrameHelper::convertToCvMat(debayered_image);
//        std::runtime_error("CamerastreamTask: frame mode is not supported");
        break;
      }
      default:
      {
        std::runtime_error("CamerastreamTask: frame mode is not supported");
        break;
      }
    }

    // switch bgr to rgb
    cv::cvtColor(mat, mat, CV_BGR2RGB);

    // output stream
    streamer->write(mat);
  }

}
void Task::errorHook()
{
    TaskBase::errorHook();
}
void Task::stopHook()
{
    TaskBase::stopHook();
    streamer->stop();
}
void Task::cleanupHook()
{
    TaskBase::cleanupHook();
}
