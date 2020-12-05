#include "FightingVision2020.h"

int FightingVision2020()
{
    FightingCapture* capture;
    //如果使用大恒相机测试
    //capture = new FightingDahengCapture();
    //如果选择使用视频来测试
    char* filename = "D:\\red-ev-0.MOV";
    capture = new FightingVideoCapture(filename);

    if (capture && !capture->init()) {
        printf("Video source initialization failed.\n");
        delete capture;
        return 1;
    }

    /* ArmorDetector */
    ArmorDetector armor_detector;
    cv::Mat src;
    cv::Point3f target;

    while (true) {
        if (capture->read(src)) {
            if (!(armor_detector.DetectArmor(src, target)))
                target = { 0, 0, -1 };
            ;
            //cv::imshow("src", src);
            cv::waitKey(1);
        } else
            break;
    }
    delete capture;
    return 0;
}

int main()
{
    FightingCapture* capture;

    capture = new FightingDahengCapture();
    capture->init();

 //   VideoCapture Test("/home/zxc/opencv/opencv-3.4.10/build/im/test1.MOV");

    Mat src;
    while (true) {
            capture->read(src);
            imshow("daheng",src);
            //Test>>src;
      /*      ArmorDetector Dtc;
            Color enmy=Blue;
            Dtc.setEnermyColor(enmy);
            Dtc.run(src);
            showArmors(src,Dtc.Armors);*/
       /*     Solver DtcSolver;
            double Dtc_yaw,Dtc_pitch,Dtc_dis;
            for(size_t i=0; i<Dtc.Armors.size(); i++)
            {
                DtcSolver.getAngle(Dtc.Armors.at(i).armorVertices,Dtc.Armors.at(i).center,Dtc_yaw,Dtc_pitch,Dtc_dis);


            }*/
            if(waitKey(20)=='s') break;
        }

    /*
    Mat src=imread("/home/zxc/opencv/opencv-3.4.10/build/im/Test1.jpg");
    ArmorDetector Dt;
    Color cl=Blue;
    Dt.setEnermyColor(cl);
    Dt.run(src);
    imshow("bin",Dt.Img_bin);
    showLights(src,Dt.Lights);
    showArmors(src,Dt.Armors);
    waitKey(0);
    */
    return 0;
}
