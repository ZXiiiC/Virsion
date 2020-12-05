#include "ArmorDetector.h"
#include <vector>
#define Color_Threshold 100
ArmorDetector::ArmorDetector()
{
    ArmorState=LIGHTS_NOT_FOUND;
}

ArmorDetector::~ArmorDetector()
{
}
void ArmorDetector::resetDetctor()
{
    ArmorState=LIGHTS_NOT_FOUND;
    Lights.clear();

}

void ArmorDetector::setEnermyColor(Color enemyColor)
{
    this->enemyColor=enemyColor;
}


void ArmorDetector::setImg(Mat& srcImg)
{
    srcImg.copyTo(Img_ori);
    int MatSize=Img_ori.rows*Img_ori.cols;
    Img_bin=Mat::zeros(Img_ori.size(),CV_8UC1);
    uchar* ori_data=(uchar *)Img_ori.data;
    uchar* bin_data=(uchar *)Img_bin.data;
    if(enemyColor==Red)
    {
    for(int i=0;i<MatSize;i++)
        {
        if(*(ori_data+2)-*ori_data>Color_Threshold)
            *bin_data=255;
        ori_data+=3;
        bin_data++;
        }
    }
    else if(enemyColor==Blue)
    {
        for(int i=0;i<MatSize;i++)
        {
        if(*ori_data-*(ori_data+2)>Color_Threshold)
            *bin_data=255;
        ori_data+=3;
        bin_data++;
        }
    }
    Mat kernel =getStructuringElement(MORPH_ELLIPSE,Size(3,3));//element
    dilate(Img_bin,Img_bin,kernel); //make the lightiBar area more smooth

}

void ArmorDetector::findLights()
{
    std::vector<std::vector<Point>> LightContours;
    Mat contourImg;
    Img_bin.copyTo(contourImg);
    findContours(contourImg,LightContours,0,2);//evolution
    RotatedRect LightRect;
    LightBar light;
    for(const auto& lightContour : LightContours){
        if(lightContour.size()<6) continue;
        if(contourArea(lightContour)<minArea) continue;

        LightRect =fitEllipse(lightContour);
        light =LightBar(LightRect);

        if(abs(light.angle)>maxAngle) continue;

        Lights.emplace_back(light);
    }
    if(Lights.size()<2)
    {
        ArmorState = LIGHTS_NOT_FOUND;
        return ;
    }

    sort(Lights.begin(),Lights.end(),
         [](LightBar &a1, LightBar & a2){
        return a1.center.x<a2.center.x;});
    ArmorState=LIGHTS_FOUND;
    return ;
}

void ArmorDetector::run(Mat& src)
{
    //load srcImg
    setImg(src);

    resetDetctor();

    findLights();

    if(ArmorState==LIGHTS_FOUND)
    {
        matchArmors();

        if(ArmorState==ARMOR_FOUND){
           ;//
        }
    }

}

bool ArmorDetector::DetectArmor(cv::Mat& src, cv::Point3f& target)
{
	cv::imshow("src", src);
	//target
	return 1;
}

void showLights(Mat & image, const vector<LightBar> & lights)
{
    Mat lightDisplay;//image for the use of dialaying the lights 显示灯条用的图像
    image.copyTo(lightDisplay);//get a copy of srcImg 获取源图像的拷贝
    //if detector finds lights 如果找到了灯条
    if (!lights.empty())
    {
        putText(lightDisplay, "LIGHTS FOUND!", Point(100, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 255), 1, 8, false); //title LIGHT_FOUND 大标题 “找到了灯条”
        for (auto light : lights)
        {
            Point2f lightVertices[4];
            light.LightRect.points(lightVertices);
            //draw all the lights' contours 画出所有灯条的轮廓
            for (size_t i = 0; i < 4; i++)
            {
                line(lightDisplay, lightVertices[i], lightVertices[(i + 1) % 4], Scalar(255, 0, 255), 1, 8, 0);
            }

            //draw the lights's center point 画出灯条中心
            circle(lightDisplay, light.center, 2, Scalar(0, 255, 0), 2, 8, 0);

            //show the lights' center point x,y value 显示灯条的中心坐标点
            putText(lightDisplay, to_string(int(light.center.x)), light.center, FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0), 1, 8, false);
            putText(lightDisplay, to_string(int(light.center.y)), light.center + Point2f(0, 15), FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0), 1, 8, false);
        }
    }
    //if detector does not find lights 如果没找到灯条
    else
    {
        putText(lightDisplay, "LIGHTS NOT FOUND!", Point(100, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 1, 8, false);//title LIGHT_NOT_FOUND 大标题 “没找到灯条”
    }
    //show the result image 显示结果图
    imshow("Lights Monitor", lightDisplay);
}

void showArmors(Mat & image, const vector<ArmorBox>& Armors)
{
    Mat armorDisplay;
    image.copyTo(armorDisplay);

    if(!Armors.empty())
    {
        putText(armorDisplay, "ARMOR FOUND!", Point(100, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 0), 1, 8, false); //title FOUND 大标题 “找到了装甲板”
        for(auto armor : Armors)
        {
            circle(armorDisplay, armor.center, 2, Scalar(0,255,0),2);
            for(size_t i=0;i<4;i++)
            {
                line(armorDisplay,armor.armorVertices[i], armor.armorVertices[(i+1)%4], Scalar(255,0,255),2,8,0);
            }
        }
    }
    else
        {
            putText(armorDisplay, "ARMOR NOT FOUND!", Point(100, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 1, 8, false);//title NOT FOUND 大标题 “没找到装甲板”
        }
    imshow("Armor",armorDisplay);
}
