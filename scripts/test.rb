require "orocos"
require "vizkit"


Orocos.run "video_streamer_vlc::Task" => "task", :valgrind => true, :valgrind_options => ["--leak-check=full"]  do 
    log = Orocos::Log::Replay.open ARGV[0]
    task = Orocos::TaskContext.get "task"
    
   
    task.frame_width=320
    task.frame_height=240

    log.bottom_camera.frame.connect_to task.frame

    task.configure 
    task.start
    Vizkit.control log
    Vizkit.exec
end

