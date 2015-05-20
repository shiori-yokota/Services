#pragma once

#include <iostream>
#include <fstream>
//#include <io.h>
//#include <conio.h>
//#include <time.h>
#include <sstream>
#include <iomanip>

#include "opencv2/opencv.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/legacy/legacy.hpp"


//#define QVGA


#define CV_LIBPATH_STR "C:/opencv2.4.8/build/x86/vc12/lib/"
#define CV_VERSION_STR CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
#if _DEBUG
#define CV_EXTENSION_STR "d.lib"
#pragma comment(lib, "C:/SIGService_v2-2-0_vc2010/lib/Debug/SIGService.lib")
#else
#define CV_EXTENSION_STR ".lib"
#pragma comment(lib, "C:/SIGService_v2-2-0_vc2010/lib/Release/SIGService.lib")
#endif

#pragma comment(lib, CV_LIBPATH_STR "opencv_core" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_imgproc" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_highgui" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_objdetect" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_contrib" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_features2d" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_flann" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_legacy" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_ts" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_video" CV_VERSION_STR CV_EXTENSION_STR)
#pragma comment(lib, CV_LIBPATH_STR "opencv_calib3d" CV_VERSION_STR CV_EXTENSION_STR)

#pragma comment(lib, CV_LIBPATH_STR "opencv_nonfree" CV_VERSION_STR CV_EXTENSION_STR)



//#ifdef QVGA
//	#define xsize    320
//	#define ysize   240
//	#define img_size 76800
//#else
//	#define xsize    640
//	#define ysize   480
//	#define img_size 307200
//#endif

using namespace cv;
using namespace std;

