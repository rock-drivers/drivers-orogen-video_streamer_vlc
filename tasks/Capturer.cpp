/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Capturer.hpp"
#include "frame_helper/FrameHelper.h"

using namespace video_streamer_vlc;

Capturer::Capturer(std::string const& name)
    : CapturerBase(name)
{
}

Capturer::Capturer(std::string const& name, RTT::ExecutionEngine* engine)
    : CapturerBase(name, engine)
{
}

Capturer::~Capturer()
{
}

bool Capturer::createStream(::std::string const & portname, ::std::string const & uri)
{
    PortHelperCapture ph;
    ph.uri = uri;
    ph.capturer = new VlcCapture(ph.uri);
    ph.port = new RTT::OutputPort< ::RTT::extras::ReadOnlyPointer< ::base::samples::frame::Frame > >(portname); 
    ports()->addPort(portname, *ph.port);
    ph.capturer->callbacks.push_back(this);
    ph.capturer->start();
    my_ports.push_back(ph);
    return ph.capturer;
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Capturer.hpp for more detailed
// documentation about them.

bool Capturer::configureHook()
{
    if (! CapturerBase::configureHook())
        return false;
    return true;
}
bool Capturer::startHook()
{
    if (! CapturerBase::startHook())
        return false;
    return true;
}
void Capturer::updateHook()
{
    CapturerBase::updateHook();
    cv::Mat image;
    for(unsigned int i=0;i<my_ports.size();i++){
        while(my_ports[i].capturer->read(image)){
            frame_helper::FrameHelper::copyMatToFrame(image,my_ports[i].frame);
            my_ports[i].frame.time = base::Time::now();
            my_ports[i].output.reset(&my_ports[i].frame);
            my_ports[i].port->write(my_ports[i].output); 
        }
    }
}
void Capturer::errorHook()
{
    CapturerBase::errorHook();
}
void Capturer::stopHook()
{
    CapturerBase::stopHook();
}
void Capturer::cleanupHook()
{
    CapturerBase::cleanupHook();
}

void Capturer::imageCallback(){
    trigger();
}
