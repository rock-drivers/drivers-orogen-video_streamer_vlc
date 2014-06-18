/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"

using namespace video_streamer_vlc;

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


bool Task::createInput(::video_streamer_vlc::PortConfig const & config)
{
    PortHelper ph;
    ph.config = config;
    if(config.config.raw_config.empty()){
        std::stringstream s;
        s << "#transcode{vcodec=" << config.config.vcodec << ", vb="<< config.config.bitrate << "}:std{access=http{mime=multipart/x-mixed-replace; boundary=--7b3cc56e5f51db803f790dad720ed50a},mux=" << config.config.mux << ",dst=" << config.config.dst << "}";
        ph.config.config.raw_config = s.str();
    }   
    ph.streamer = new VlcStream(ph.config.config.raw_config, config.config.fps, config.config.frame_width, config.config.frame_height);
    ph.port = new RTT::InputPort< ::RTT::extras::ReadOnlyPointer< ::base::samples::frame::Frame > >(config.port_name); 
    ports()->addEventPort(config.port_name, *ph.port);
    ph.streamer->start();
    my_ports.push_back(ph);
    return ph.streamer;
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
    return true;
}
void Task::updateHook()
{
    TaskBase::updateHook();
    cv::Mat mat;

    for(std::vector<PortHelper>::iterator it = my_ports.begin(); it != my_ports.end();it++)
    {
        while(it->port->read(current_image_) == RTT::NewData)
        {
            if(current_image_->getStatus() == base::samples::frame::STATUS_VALID)
            {
                if(current_image_->getWidth() != it->config.config.frame_width || current_image_->getHeight() != it->config.config.frame_height)
                {
                    std::cout << "Warn incoming image has different size than configured" << std::endl ;
                    std::cout << "Current Size is: " << current_image_->getWidth() << "x" << current_image_->getHeight() << std::endl;
                }
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
                        std::runtime_error("CamerastreamTask: frame mode is not supported");
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
                it->streamer->write(mat);
            }
        }
    }
}

void Task::errorHook()
{
    TaskBase::errorHook();
}
void Task::stopHook()
{
    TaskBase::stopHook();
}
void Task::cleanupHook()
{
    TaskBase::cleanupHook();
    for(std::vector<PortHelper>::iterator it = my_ports.begin(); it != my_ports.end();it++)
    {
        ports()->removePort(it->port->getName());
        it->streamer->stop();
        delete it->streamer;
        delete it->port;
    }
}
