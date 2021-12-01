/* Generated from orogen/lib/orogen/templates/tasks/Streamer.cpp */

#include "Streamer.hpp"

using namespace video_streamer_vlc;

Streamer::Streamer(std::string const& name)
    : StreamerBase(name)
{
}

Streamer::Streamer(std::string const& name, RTT::ExecutionEngine* engine)
    : StreamerBase(name, engine)
{
}

Streamer::~Streamer()
{
}

bool Streamer::createInput(::video_streamer_vlc::PortConfig const & config)
{
    PortHelper ph;
    ph.config = config;
    if(config.config.raw_config.empty()){
        std::stringstream s;
        s << "#transcode{vcodec=" << config.config.vcodec 
            << ", vb="<< config.config.bitrate 
            << "}:std{"
            << "access=" << config.config.access 
            << ", mux=" << config.config.mux << ",dst=" << config.config.dst << "}";
        ph.config.config.raw_config = s.str();
    }   
    ph.streamer = new VlcStream(ph.config.config.raw_config, config.config.fps, 
        config.config.frame_width, config.config.frame_height);
    ph.port  = new RTT::InputPort< ::RTT::extras::ReadOnlyPointer< 
        ::base::samples::frame::Frame > >(config.port_name); 
    ports()->addEventPort(config.port_name, *ph.port);
    ph.streamer->start();
    my_ports.push_back(ph);
    return ph.streamer;
}


/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Streamer.hpp for more detailed
// documentation about them.

bool Streamer::configureHook()
{
    if (! StreamerBase::configureHook())
        return false;
    return true;
}
bool Streamer::startHook()
{
    if (! StreamerBase::startHook())
        return false;
    return true;
}
void Streamer::updateHook()
{
    StreamerBase::updateHook();
    cv::Mat mat;

    for(std::vector<PortHelper>::iterator it = my_ports.begin(); it != my_ports.end();it++)
    {
        while(it->port->read(current_image_) == RTT::NewData)
        {
            if(current_image_->getStatus() == base::samples::frame::STATUS_VALID)
            {
                if(current_image_->getWidth() != it->config.config.frame_width || 
                    current_image_->getHeight() != it->config.config.frame_height)
                {
                    std::cout << "Warn incoming image has different size than configured" 
                        << std::endl ;
                    std::cout << "Current Size is: " << current_image_->getWidth() << "x" 
                        << current_image_->getHeight() << std::endl;
                    std::cout << "Configured size is: " << it->config.config.frame_height 
                        << "x" << it->config.config.frame_width << std::endl;
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
                #if (CV_VERSION_MAJOR >= 4)
                    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
                #else
                    cv::cvtColor(mat, mat, CV_BGR2RGB);
                #endif
                
                // output stream
                it->streamer->write(mat, current_image_->time.toMicroseconds() );
            }
        }
    }
}

void Streamer::errorHook()
{
    StreamerBase::errorHook();
}
void Streamer::stopHook()
{
    StreamerBase::stopHook();
}
void Streamer::cleanupHook()
{
    StreamerBase::cleanupHook();
    for(std::vector<PortHelper>::iterator it = my_ports.begin(); it != my_ports.end();it++)
    {
        ports()->removePort(it->port->getName());
        it->streamer->stop();
        delete it->streamer;
        delete it->port;
    }
}
