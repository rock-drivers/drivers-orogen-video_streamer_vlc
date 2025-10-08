/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "SingleCapturer.hpp"

#include "frame_helper/FrameHelper.h"

using namespace video_streamer_vlc;

SingleCapturer::SingleCapturer(std::string const& name)
    : SingleCapturerBase(name)
{
}

SingleCapturer::~SingleCapturer()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See SingleCapturer.hpp for more detailed
// documentation about them.

bool SingleCapturer::configureHook()
{
    if (! SingleCapturerBase::configureHook())
        return false;
    
    if (_input_buffer_ms.value() >=0 ) {
        capturer = std::make_shared<VlcCapture>(_uri.value(), _input_buffer_ms.value());
    } else {
        capturer = std::make_shared<VlcCapture>(_uri.value());
    }
    capturer->callbacks.push_back(this);

    return true;
}
bool SingleCapturer::startHook()
{
    if (! SingleCapturerBase::startHook())
        return false;
    output.reset(&frame);
    capturer->start();
    return true;
}
void SingleCapturer::updateHook()
{
    SingleCapturerBase::updateHook();

    cv::Mat image;
    if (capturer->read(image)) {

        cv::Mat newmat(image.size(), CV_8UC3);
        cv::cvtColor(image, newmat, cv::COLOR_RGBA2BGR);

	base::samples::frame::Frame *frame_ptr = output.write_access();
	if (frame_ptr)
	{
		frame_helper::FrameHelper::copyMatToFrame(newmat,*frame_ptr);
		frame_ptr->time = base::Time::now();
		output.reset(frame_ptr);
		_frame.write(output);
	}

    }
}
void SingleCapturer::errorHook()
{
    SingleCapturerBase::errorHook();
}
void SingleCapturer::stopHook()
{
    SingleCapturerBase::stopHook();
    capturer->stop();
}
void SingleCapturer::cleanupHook()
{
    SingleCapturerBase::cleanupHook();
    capturer.reset();
}
void SingleCapturer::imageCallback() {
    trigger();
}
