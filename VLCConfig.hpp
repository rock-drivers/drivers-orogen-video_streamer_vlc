/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2014
 * Desc:
 *
*/
#ifndef VLCCONFIG_H
#define VLCCONFIG_H 

#include <string>

namespace video_streamer_vlc 
{
    struct Config{
        
        /*
         * image width in pixel, default 640
         */
	unsigned int frame_width;
        
        /*
         * image height in pixel, default 480
         */
	unsigned int frame_height;
        
        /*
         * Frames per second, default 30
         */
        unsigned int fps;
        
        /*
         * bitrate of encoded video stream, default 500
         */
        unsigned int bitrate;
        
        /*
         * Raw configuraion, if set all other config values are overridden.
         * The string then is constructed from: 
         * # #transcode{vcodec=MJPG, vb=500}:std{access=http{mime=multipart/x-mixed-replace; boundary=--7b3cc56e5f51db803f790dad720ed50a},mux=mpjpeg,dst=127.0.0.1:8080/video.mjpg}" 
         */
        std::string raw_config;
        
        /*
         * Video codec decoding information, default MJPEG
         */
        std::string vcodec;

        /*
         * Used Muxer, default mpjpeg
         */

        std::string mux;
        /*
         * Destination url for stream, default http://127.0.0.1:8080/video.mjpg
         */
        std::string dst;
        
        Config(){
            vcodec = std::string("MJPG");
            bitrate = 500;
            mux = std::string("mpjpeg");
            dst = std::string("127.0.0.1:8080/video.mjpg");
        }
    };
    
    struct PortConfig{
        Config config;
        std::string port_name;
    };
};


#endif 
