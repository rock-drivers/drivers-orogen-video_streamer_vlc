require "orocos"
require "vizkit"


Orocos.run "video_streamer_vlc::Task" => "task", :valgrind => false, :valgrind_options => ["--leak-check=full"]  do 
    log = Orocos::Log::Replay.open ARGV[0]
    task = Orocos::TaskContext.get "task"
    
 
    config = Types::VideoStreamerVlc::PortConfig.new
    config.port_name = "frame"
    config.config.fps = 30
    config.config.frame_width = 640
    config.config.frame_height = 480
    config.config.vcodec = "MJPG"
    config.config.mux = "mpjpeg"
    config.config.dst= "127.0.0.1:8080/video.mjpg"

    task.createInput(config)

    log.bottom_camera.frame.connect_to task.frame

    task.configure 
    task.start
    Vizkit.control log
    Vizkit.exec
end

