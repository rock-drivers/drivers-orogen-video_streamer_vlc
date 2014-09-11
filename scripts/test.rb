require "orocos"
require "vizkit"


Orocos.run "video_streamer_vlc::Streamer" => "task2", :valgrind => false, :valgrind_options => ["--leak-check=full"]  do 
#    log = Orocos::Log::Replay.open ARGV[0]
    cam = Orocos::TaskContext.get "camera"
    task = Orocos::TaskContext.get "task2"
    
 
    config = Types::VideoStreamerVlc::PortConfig.new
    config.port_name = "frame"
#    config.config.bitrate = 10
    config.config.fps = 30 
    #config.config.frame_width = 1600 
    #config.config.frame_height = 1200
    config.config.frame_width = 640 
    config.config.frame_height = 480
    #config.config.vcodec = "mjpg"
    #config.config.mux = "mpjpeg"
    config.config.vcodec = "mp1v"
    config.config.mux = "ts"
    config.config.dst= "192.168.128.108:8080/video.ts"

    #config.config.raw_config = "#transcode{vcodec=mp1v,vb=1000,scale=0.25}:rtp{mux=ts,dst=239.255.12.42,port=5004,ttl=50}" 
    #config.config.raw_config = "#transcode{vcodec=mp1v,vb=10,scale=1.0}:rtp{dst=239.255.12.42,port=5004,ttl=50}" 
    #config.config.raw_config = "#rtp{dst=239.255.12.42,port=5004,ttl=50}"  #kaputt
    #config.config.raw_config = "#transcode{vcodec=mp1v,scale=0.25,threads=4}:rtp{dst=239.255.12.42,port=5004,ttl=50}" 
    #config.config.raw_config = "#transcode{vcodec=mp2v,scale=0.5,threads=4}:rtp{dst=239.255.12.42,port=5004,ttl=30}" 
    config.config.raw_config = "#transcode{vcodec=mjpg,scale=0.5,vb=100,threads=4}:rtp{mux=ts,dst=192.168.128.108,port=5004,ttl=30}" 
    STDOUT.puts "Creating config port...."
    task.createInput(config)
    STDOUT.puts "....done"

    cam.frame.connect_to task.frame
    STDOUT.puts "Configuring ..."
    task.configure 
    STDOUT.puts "....done"
    STDOUT.puts "Starting..."
    task.start
    STDOUT.puts "....done"
#    Vizkit.control log
    Vizkit.exec
end

