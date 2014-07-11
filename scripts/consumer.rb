require "orocos"
require "vizkit"


Orocos.run "video_streamer_vlc::Capturer" => "consumer"  do 
    task = Orocos::TaskContext.get "consumer"
    
 
    task.createStream("frame","rtp://239.255.12.42:5004")

    Vizkit.display task
    task.configure 
    task.start
#    Vizkit.control log
    Vizkit.exec
end

