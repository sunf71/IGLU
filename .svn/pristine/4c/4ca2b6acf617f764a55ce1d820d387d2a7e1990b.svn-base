Important dependency information:

IGLU requires the following:
  * GLEW 1.7 or later, which includes OpenGL 4.2 API definitions.  On some
    Linux systems, the packaged libglew may not be new enough, so you will 
    need to download and compile from source ("make;make install").  However, 
    beware that some of these systems *also* do not store libGLEW in standard 
    locations, so "make install" may leave older versions around.  This may 
    cause link errors (on OGL 4.2 functions) when trying to use IGLU.
  * A graphics driver supporting OpenGL 4.2+ (if you just want to compile IGLU,
    having GLEW 1.7+ is sufficient).  For NVIDIA, you need drivers 285.xx or later.
  * FFmpeg for video functionality.  I used FFMpeg 0.9.1.  For Windows, I downloaded
    precompiled binaries (from here, I think: http://ffmpeg.zeranoe.com/builds/).  
    On Linux, you can either download FFmpeg 0.9.1 as source and compile yourself, or
    use packages for either FFMpeg or libav.  I've tested IGLU with libav 0.7.3 on
    Ubuntu (and a manually-compiled FFMpeg 0.9.1 on Redhat).  I have heard that the
    FFmpeg API we use is depricated, and some newer libraries may not support it (?)
    This should be fixed eventually.
  * If you have trouble getting FFmpeg/libav compiled or installed, you can disable
    the video functionality.  Video functionality is *enabled* by defining the 
    preprocessor value HAS_FFMPEG when compiling.  By default, the included
    Visual Studio project enables this.  On other systems, this needs to be done
    manually.
  * IGLU links agains FLTK 1.3.
  * Some of samples rely on GLUT, so you need either GLUT or FreeGLUT (or include
    FLTK's GLUT-emulation)
    
We have a basic CMake setup, which has been tested under various Linux systems.  
It may not be completely robust.  We have not compiling with CMake on Windows.

Our development currently occurs via Visual Studio in Windows.  The Visual Studio 
(2008) project included in the repository is NOT created by CMake.  You'll want to
use that directly (or upgrade to VS 2010, which is tested).  

