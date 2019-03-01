/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */
#include <iostream>

#include "SingleStreamer.hpp"

using namespace video_streamer_vlc;

SingleStreamer::SingleStreamer(std::string const& name)
    : SingleStreamerBase(name)
{
}

SingleStreamer::SingleStreamer(std::string const& name, RTT::ExecutionEngine* engine)
    : SingleStreamerBase(name, engine)
{
}

SingleStreamer::~SingleStreamer()
{
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Streamer.hpp for more detailed
// documentation about them.

bool SingleStreamer::configureHook()
{
    if (! SingleStreamerBase::configureHook())
        return false;
    
    config = _config.get();
    if(config.raw_config.empty()){
        std::stringstream s;
        s << "#transcode{vcodec=" << config.vcodec 
            << ",vb="<< config.bitrate 
            << ",acodec=vorb,ab=128,channels=2,samplerate=44100,scodec=none}"
            << ":http{"
            << "mux=" << config.mux << ",dst=" << config.dst << "}";
        config.raw_config = s.str();
        std::cout << config.raw_config << std::endl;
    }   
    
    streamer = std::make_unique<VlcStream>(config.raw_config, config.fps, 
        config.frame_width, config.frame_height);

    return true;
}
bool SingleStreamer::startHook()
{
    if (! SingleStreamerBase::startHook())
        return false;
    streamer->start();
    return true;
}


void SingleStreamer::updateHook()
{
    SingleStreamerBase::updateHook();
    cv::Mat mat;

    while(_input_frames.read(current_image_) == RTT::NewData)
    {
        if(current_image_->getStatus() == base::samples::frame::STATUS_VALID)
        {
            if(current_image_->getWidth() != config.frame_width || 
                current_image_->getHeight() != config.frame_height)
            {
                std::cout << "Warn incoming image has different size than configured" 
                    << std::endl ;
                std::cout << "Current Size is: " << current_image_->getWidth() << "x" 
                    << current_image_->getHeight() << std::endl;
                std::cout << "Configured size is: " << config.frame_height 
                    << "x" << config.frame_width << std::endl;
            }
            switch(current_image_->getFrameMode())
            {
                case base::samples::frame::MODE_RGB:
                case base::samples::frame::MODE_GRAYSCALE:
                {
                    mat = frame_helper::FrameHelper::convertToCvMat(*current_image_);
                    break;
                }
                case base::samples::frame::MODE_JPEG:
                case base::samples::frame::MODE_BAYER_RGGB:
                case base::samples::frame::MODE_BAYER_GRBG:
                case base::samples::frame::MODE_BAYER_BGGR:
                case base::samples::frame::MODE_BAYER_GBRG:
                {
                    rgb_image.init( *current_image_, false );
                    rgb_image.setFrameMode( base::samples::frame::MODE_RGB );
                    frame_helper.convertColor(*current_image_, rgb_image);
                    mat = frame_helper::FrameHelper::convertToCvMat(rgb_image);
                    break;
                }
                default:
                {
                    throw std::runtime_error("CamerastreamStreamer: frame mode is not supported");
                    break;
                }
            }

            // switch bgr to rgb
            cv::cvtColor(mat, mat, CV_BGR2RGB);
            // output stream
            streamer->write(mat, current_image_->time.toMicroseconds() );
        }
    }
}

void SingleStreamer::errorHook()
{
    SingleStreamerBase::errorHook();
}
void SingleStreamer::stopHook()
{
    streamer->stop();
    SingleStreamerBase::stopHook();
}
void SingleStreamer::cleanupHook()
{
    streamer.reset();
    SingleStreamerBase::cleanupHook();
}
