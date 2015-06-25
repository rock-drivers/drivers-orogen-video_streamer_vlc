require "orocos"
require "vizkit"

Orocos.initialize

Orocos.run "video_streamer_vlc::Streamer" => "vlc", :output => nil do
    # get input log data
    log_file = ARGV[0]
    log_task = ARGV[1]
    log_port = "frame"
    log = Orocos::Log::Replay.open log_file 
    puts "Loading Logfile from #{log_file}"
    cam = log.task(log_task)
    port_reader = cam.port(log_port).reader :type => :data

    # get the first frame, to get information on stream
    log.step
    frame = port_reader.read
    width, height = frame.size.width, frame.size.height

    # configure vlc Task
    vlc = Orocos::TaskContext.get "vlc"
    
    config = Orocos.registry.get('/video_streamer_vlc/PortConfig').new
    config.port_name = "frame"
    config.config.fps = 10
    config.config.frame_width = width
    config.config.frame_height = height
    #config.config.vcodec = "mjpg"
    #config.config.mux = "mpjpeg"
    config.config.vcodec = "h264"
    config.config.mux = "mp4"
    config.config.access = "file"
    config.config.dst = "/tmp/file.mp4"

#    #config.config.raw_config = "#transcode{vcodec=mp1v,vb=1000,scale=0.25}:rtp{mux=ts,dst=239.255.12.42,port=5004,ttl=50}" 
#    #config.config.raw_config = "#transcode{vcodec=mp1v,vb=10,scale=1.0}:rtp{dst=239.255.12.42,port=5004,ttl=50}" 
#    #config.config.raw_config = "#rtp{dst=239.255.12.42,port=5004,ttl=50}"  #kaputt
#    #config.config.raw_config = "#transcode{vcodec=mp1v,scale=0.25,threads=4}:rtp{dst=239.255.12.42,port=5004,ttl=50}" 
#    #config.config.raw_config = "#transcode{vcodec=mp2v,scale=0.5,threads=4}:rtp{dst=239.255.12.42,port=5004,ttl=30}" 
#    config.config.raw_config = "#transcode{vcodec=mjpg,scale=0.5,vb=100,threads=4}:rtp{mux=ts,dst=239.255.12.42,port=5004,ttl=30}" 
    vlc.createInput(config)

    cam.frame.connect_to vlc.frame

    vlc.configure 
    vlc.start

#    log.run
    Vizkit.control log
    Vizkit.exec
end


