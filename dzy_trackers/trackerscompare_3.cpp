
#include "kcf/kcftracker.hpp"
#include "eco/eco.hpp"
#include "eco/parameters.hpp"

#ifdef USE_CAFFE
#include "goturn/network/regressor.h"
#include "goturn/tracker/tracker.h"
#endif

#include "inputs/readdatasets.hpp"
#include "inputs/readvideo.hpp"
//#include "inputs/openpose.hpp"

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <opencv2/opencv.hpp>
//#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>
#include <string>

using namespace cv;
using namespace std;

// Convert to string
int main(int argc, char **argv)
{
    int key,flag = 0;
/*
    // Read using openpose============================================
    cv::Rect2f bboxGroundtruth;
    cv::Mat frame, frameDraw;

    OpenPose openpose;
    openpose.IniRead(bboxGroundtruth);
    VideoCapture capture(0); // open the default camera
    if (!capture.isOpened()) // check if we succeeded
        return -1;
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
    capture >> frame;
    //frame.copyTo(frameDraw);
    //rectangle(frameDraw, bboxGroundtruth, Scalar(0, 0, 0), 2, 1);
    //imshow("Tracking", frameDraw);
    */

    // Read from Video and choose a bbox===============================
    //::google::InitGoogleLogging(argv[0]);
    cv::Rect2f bboxGroundtruth;
    cv::Mat frame, frameDraw;
    //cv::VideoCapture capture;
    //capture.open("sequences/oneperson.mp4");
    VideoCapture capture(1); // open the default camera
    if (!capture.isOpened())
    {
        std::cout << "Capture device failed to open!" << std::endl;
        return -1;
    }
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 1080);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
    capture >> frame;
    //frameDraw.copyTo(frame);

    std::string window_name = "OpenTracker";
    cv::namedWindow(window_name, 0);
    ReadVideo readvideo;
    //readvideo.IniRead(bboxGroundtruth, frameDraw, window_name, capture);
    
    // Read from the datasets==========================================
//    ::google::InitGoogleLogging(argv[0]);
/*   cv::Rect2f bboxGroundtruth;
    cv::Mat frame, frameDraw;
    ReadDatasets readdatasets;
    readdatasets.IniRead(bboxGroundtruth, frame);
    frame.copyTo(frameDraw);
    readdatasets.DrawGroundTruth(bboxGroundtruth, frameDraw);
*/
    // Init the trackers=================================================
    // Create Opencv tracker:
//    string trackerTypes[6] = {"BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN"};
//    string trackerType = trackerTypes[2];
//    Ptr<cv::Tracker> opencvtracker;
//    if (trackerType == "BOOSTING")
//        opencvtracker = cv::TrackerBoosting::create();
//    if (trackerType == "MIL")
//        opencvtracker = cv::TrackerMIL::create();
//    if (trackerType == "KCF")
//        opencvtracker = cv::TrackerKCF::create();
//    if (trackerType == "TLD")
//        opencvtracker = cv::TrackerTLD::create();
//    if (trackerType == "MEDIANFLOW")
//        opencvtracker = cv::TrackerMedianFlow::create();
//    if (trackerType == "GOTURN")
//        opencvtracker = cv::TrackerGOTURN::create();
//    Rect2d opencvbbox((int)bboxGroundtruth.x, (int)bboxGroundtruth.y, (int)bboxGroundtruth.width, (int)bboxGroundtruth.height);
//    opencvtracker->init(frame, opencvbbox);

/*
    // Create DSSTTracker:
    DSST = true;
    kcf::KCFTracker dssttracker(HOG, FIXEDWINDOW, MULTISCALE, LAB, DSST);
    Rect2d dsstbbox((int)bboxGroundtruth.x, (int)bboxGroundtruth.y, (int)bboxGroundtruth.width, (int)bboxGroundtruth.height);
    dssttracker.init(frame, dsstbbox);

#ifdef USE_CAFFE
    // Create GOTURN tracker:
    const string model_file = "goturn/nets/deploy.prototxt";
    const string pretrain_file = "goturn/nets/goturun_tracker.caffemodel";
    int gpu_id = 0;
    Regressor regressor(model_file, pretrain_file, gpu_id, false);
    goturn::Tracker goturntracker(false);
    cv::Rect goturnbbox(bboxGroundtruth.x, bboxGroundtruth.y, bboxGroundtruth.width, bboxGroundtruth.height);
    BoundingBox bbox_gt;
    BoundingBox bbox_estimate_uncentered;
    bbox_gt.getRect(goturnbbox);
    goturntracker.Init(frame, bbox_gt, &regressor);
#endif
*/
    // Create ECO trakcer;
    eco::ECO ecotracker;
    Rect2f ecobbox(bboxGroundtruth.x, bboxGroundtruth.y, bboxGroundtruth.width, bboxGroundtruth.height);
    eco::EcoParameters parameters;
    //parameters.max_score_threshhold = 0.1;
    // when use cn feature:
    //parameters.useCnFeature = true;
    parameters.useCnFeature = false;
    parameters.cn_features.fparams.tablename = "/home/nvidia/Develop/Project/Tracker/OpenTracker/eco/look_tables/CNnorm.txt";
    //ecotracker.init(frame, ecobbox, parameters);

    // Create KCF Tracker:
    bool HOG = true, FIXEDWINDOW = true, MULTISCALE = true, LAB = true, DSST = false; //LAB color space features
    kcf::KCFTracker kcftracker(HOG, FIXEDWINDOW, MULTISCALE, LAB, DSST);
    Rect2d kcfbbox((int)bboxGroundtruth.x, (int)bboxGroundtruth.y, (int)bboxGroundtruth.width, (int)bboxGroundtruth.height);
    //kcftracker.init(frame, kcfbbox);
    
    while (frame.data)
    {
        //frame.copyTo(frameDraw);
        key = cvWaitKey(6);
        //ECO
        if(key == 'a'){
            //frame.copyTo(frameDraw);
            flag = 1;
            //putText(frame, "!Choose Target!", cv::Point(10, 20), FONT_HERSHEY_SIMPLEX,0.5, Scalar(255, 0, 0), 2);
            readvideo.IniRead(bboxGroundtruth, frame, window_name, capture);
            //ECO初始化
            Rect2f ecobbox(bboxGroundtruth.x, bboxGroundtruth.y, bboxGroundtruth.width, bboxGroundtruth.height);
            ecotracker.init(frame, ecobbox, parameters);
        }
        //KCF
        else if(key == 's'){
            //frame.copyTo(frameDraw);
            flag = 2;
            //putText(frame, "!Choose Target!", cv::Point(10, 20), FONT_HERSHEY_SIMPLEX,0.5, Scalar(255, 0, 0), 2);
            readvideo.IniRead(bboxGroundtruth, frame, window_name, capture);
            //KCF初始化
            Rect2d kcfbbox((int)bboxGroundtruth.x, (int)bboxGroundtruth.y, (int)bboxGroundtruth.width, (int)bboxGroundtruth.height);
            kcftracker.init(frame, kcfbbox);
        }
        //ECO + KCF
        else if(key == 'd'){
            //frame.copyTo(frameDraw);
            flag = 3;
            //putText(frame, "!Choose Target!", cv::Point(10, 20), FONT_HERSHEY_SIMPLEX,0.5, Scalar(255, 0, 0), 2);
            readvideo.IniRead(bboxGroundtruth, frame, window_name, capture);
            //ECO初始化
            Rect2f ecobbox(bboxGroundtruth.x, bboxGroundtruth.y, bboxGroundtruth.width, bboxGroundtruth.height);
            ecotracker.init(frame, ecobbox, parameters);
            //KCF初始化
            Rect2d kcfbbox((int)bboxGroundtruth.x, (int)bboxGroundtruth.y, (int)bboxGroundtruth.width, (int)bboxGroundtruth.height);
            kcftracker.init(frame, kcfbbox);
        }
        else if(key == 'e'){
            flag = 0;
        }
        //Opencv=====================
       // double timercv = (double)getTickCount();
       // bool okopencv = opencvtracker->update(frame, opencvbbox);
       // float fpscv = getTickFrequency() / ((double)getTickCount() - timercv);
       // if (okopencv)
       // {
       //     rectangle(frameDraw, opencvbbox, Scalar(255, 0, 0), 2, 1);
       // }
       // else
       // {
       //     putText(frameDraw, "Opencv tracking failure detected", cv::Point(10, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(255, 0, 0), 2);
       // }

/*        //KCF=========================
        double timerkcf = (double)getTickCount();
        bool okkcf = kcftracker.update(frame, kcfbbox);
        float fpskcf = getTickFrequency() / ((double)getTickCount() - timerkcf);
        if (okkcf)
        {
            rectangle(frameDraw, kcfbbox, Scalar(0, 255, 0), 2, 1);
        }
        else
        {
            putText(frameDraw, "Kcf tracking failure detected", cv::Point(10, 80), FONT_HERSHEY_SIMPLEX,
                    0.75, Scalar(0, 255, 0), 2);
        }

        //DSST========================
        double timerdsst = (double)getTickCount();
        bool okdsst = dssttracker.update(frame, dsstbbox);
        float fpsdsst = getTickFrequency() / ((double)getTickCount() - timerdsst);
        if (okdsst)
        {
            rectangle(frameDraw, dsstbbox, Scalar(0, 0, 255), 2, 1);
        }
        else
        {
            putText(frameDraw, "DSST tracking failure detected", cv::Point(10, 110), FONT_HERSHEY_SIMPLEX,
                    0.75, Scalar(0, 0, 255), 2);
        }
#ifdef USE_CAFFE
        //GOTURN=====================
        double timergoturn = (double)getTickCount();
        goturntracker.Track(frame, &regressor, &bbox_estimate_uncentered);
        bbox_estimate_uncentered.putRect(goturnbbox);
        float fpsgoturn = getTickFrequency() / ((double)getTickCount() - timergoturn);
        rectangle(frameDraw, goturnbbox, Scalar(255, 255, 0), 2, 1);
#endif
*/
        
        if(flag == 1 || flag == 3){
            //ECO Tracking
	    double timeeco = (double)getTickCount();
            bool okeco = ecotracker.update(frame, ecobbox);
            float fpseco = getTickFrequency() / ((double)getTickCount() - timeeco);
	    if (okeco){
                //frame.copyTo(frameDraw);
                rectangle(frame, ecobbox, Scalar(0, 0, 255), 3);
	    }
	    else{
                //Rect2f ecobbox(bboxGroundtruth.x, bboxGroundtruth.y, bboxGroundtruth.width, bboxGroundtruth.height);
                //ecotracker.init(frameDraw, ecobbox, parameters);
                //ecotracker.init(frame, ecobbox, parameters);
	        putText(frame, "!!!NO Target!!!", cv::Point(130, 20), FONT_HERSHEY_SIMPLEX,
		            0.5, Scalar(0, 0, 255), 2);
	    }
              // Display FPS
	    ostringstream os;
	    os << float(fpseco);
	    putText(frame, "FPS: " + os.str(), Point(10, 20), FONT_HERSHEY_SIMPLEX,
		        0.5, Scalar(0, 0, 255), 2);
		
                // Draw ground truth box===========================================
		//readdatasets.DrawGroundTruth(bboxGroundtruth, frameDraw);
         }
         if(flag == 2 || flag == 3){
            //KCF Tracking
            double timerkcf = (double)getTickCount();
            bool okkcf = kcftracker.update(frame, kcfbbox);
            float fpskcf = getTickFrequency() / ((double)getTickCount() - timerkcf);
            if (okkcf)
            {
                //frame.copyTo(frameDraw);
                rectangle(frame, kcfbbox, Scalar(0, 255, 0), 3);
            }
            else
            {
                //Rect2d kcfbbox((int)bboxGroundtruth.x, (int)bboxGroundtruth.y, (int)bboxGroundtruth.width, (int)bboxGroundtruth.height);
                //kcftracker.init(frameDraw, kcfbbox);
                //kcftracker.init(frameDraw, kcfbbox);
	        putText(frame, "!!!NO Target!!!", cv::Point(130, 50), FONT_HERSHEY_SIMPLEX,
		            0.5, Scalar(0, 255, 0), 2);
            }
	      // Display FPS
            ostringstream os1;
            os1 << float(fpskcf);
	    putText(frame, "FPS: " + os1.str(), Point(10, 50), FONT_HERSHEY_SIMPLEX,
		        0.5, Scalar(0, 255, 0), 2);
          }
		// Draw the label of trackers
		//putText(frameDraw, "Opencv ", cv::Point(frameDraw.cols - 180, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(255, 0, 0), 2);
		//line(frameDraw, cv::Point(frameDraw.cols - 100, 50), cv::Point(frameDraw.cols - 10, 50), Scalar(255, 0, 0), 2, 1);
	       // putText(frameDraw, "KCF ", cv::Point(frameDraw.cols - 180, 75), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 255, 0), 2);
	       // line(frameDraw, cv::Point(frameDraw.cols - 100, 75), cv::Point(frameDraw.cols - 10, 75), Scalar(0, 255, 0), 2, 1);
	       // putText(frameDraw, "DSST ", cv::Point(frameDraw.cols - 180, 100), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
	       // line(frameDraw, cv::Point(frameDraw.cols - 100, 100), cv::Point(frameDraw.cols - 10, 100), Scalar(0, 0, 255), 2, 1);
		//putText(frameDraw, "ECO ", cv::Point(frameDraw.cols - 180, 125), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(255, 0, 255), 2);
		//line(frameDraw, cv::Point(frameDraw.cols - 100, 125), cv::Point(frameDraw.cols - 10, 125), Scalar(255, 0, 255), 2, 1);
	#ifdef USE_CAFFE
		//putText(frameDraw, "GOTURN ", cv::Point(frameDraw.cols - 180, 150), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(255, 255, 0), 2);
		//line(frameDraw, cv::Point(frameDraw.cols - 100, 150), cv::Point(frameDraw.cols - 10, 150), Scalar(255, 255, 0), 2, 1);
	#endif
		// Display frameDraw.=========================================================
		//cvNamedWindow("Tracking", CV_WINDOW_NORMAL);

        imshow("OpenTracker", frame);

        //int c = cvWaitKey(1);
        //if (c != -1)
        //    c = c % 256;
        //if (c == 27)
        //{
        //    cvDestroyWindow("OpenTracker");
        //    return 0;
        //}
        waitKey(1);

        // Read the next frame
/* Read from dataset */
        //readdatasets.ReadNextFrame(bboxGroundtruth, frame);
// Read from video and choose a bbox 
        capture >> frame;
        if (frame.empty())
            return false;
    }
#ifdef USE_MULTI_THREAD
    void *status;
    int rc = pthread_join(ecotracker.thread_train_, &status);
    if (rc)
    {
         cout << "Error:unable to join," << rc << std::endl;
         exit(-1);
    }
#endif
    cvDestroyWindow("OpenTracker");
    return 0;
}
