#ifndef SOLVER_H
#define SOLVER_H
#include"ArmorDetector.h"
#define UNIT_PIXEL_W 0.0008234375
#define UNIT_PIXEL_H 0.000825

using namespace std;
using namespace cv;

enum ArmorType{
    SMALL=0,
    BIG=1
};

class Solver
{
public:
    Solver();
    void setCameraParam(const Mat& camera_matrix, const Mat & distortion_coeff);

    int setCameraParam(const char* filePath, int camId);

    void setArmorSize(double width, double height);

    void setBulletSpeed(int bulletSpeed);

    void setTarget(vector<Point2f> contoursPoints, Point2f cenerPoint, ArmorType type);

    void solveAngles();

    void P4P_solver();

    void PinHole_solver();

    void compensateAngle();

    void compensateOffset();

    void compensateGravity();

    void getAngle(vector<Point2f> & contourPoints, Point2f centerPoint, double& yaw, double& pitch, double& evaluateDistance) ;

private:

    Mat CAMERA_MATRIX;
    Mat DISTORTION_COEFF;

    vector<Point3f> SMALL_ARMOR_POINTS_3D;
    vector<Point3f> BIG_ARMOR_POINTS_3D;

    double BULLET_SPEED;

    ArmorType Type;

    double GUN_CAM_DISTANCE_Y;

    vector<Point2f> targetContour;
    Point2f targetCenter;

    Mat rVec;
    Mat tVec;
    float y_yaw;
    float x_pitch;
    double distance;

};
#endif // SOLVER_H
