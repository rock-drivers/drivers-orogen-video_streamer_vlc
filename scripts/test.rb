require "orocos"
#require "vizkit"


Orocos.run "video_streamer_vlc::Streamer" => "task2", :valgrind => false, :valgrind_options => ["--leak-check=full"]  do 
    cam = Orocos::TaskContext.get "bottom_camera"
    task = Orocos::TaskContext.get "task2"
    
 
    config = Types::VideoStreamerVlc::PortConfig.new
    dst = "192.168.128.50:7333/video.mjpg"
    config.config.dst = dst
    config.port_name = "frame"
    filename = "bla.mp4"
    config.config.frame_width = 640
    config.config.frame_height = 480
    #file = "std{access=file, mux=mp4, dst=#{filename}}"
    #stream = "std{access=http{mime=multipart/x-mixed-replace;boundary=--7b3cc56e5f51db803f790dad720ed50a},mux=mpjpeg,dst=#{dst}}"
    #c_config = "transcode{vcodec=MJPG, vb=500, width = 640, height = 480}"
    #file_config = "transcode{vcodec=mp4, vb=500, width = 640, height = 480}"
    #config.config.raw_config = '#duplicate{dst="#{file_config}:#{file}",dst=#{c_config}:#{stream}'
    #config.config.raw_config = "#{c_config}:duplicate{dst=#{file},dst=#{stream}}"
    #config.config.raw_config = "#{c_config}:#{stream}"
    config.config.raw_config = "#transcode{vcodec=mjpg,scale=0.5,vb=100,threads=4}:rtp{mux=ts,dst=239.255.12.42,port=7777,ttl=30}"

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
#    Vizkit.exec
    loop do
        sleep 1
    end
end

