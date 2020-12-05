#include"Solver.h"

Solver::Solver()
{
}

void Solver::setCameraParam(const Mat &camera_matrix, const Mat &distortion_coeff)
{
    camera_matrix.copyTo(CAMERA_MATRIX);
    distortion_coeff.copyTo((DISTORTION_COEFF));
}

/*int Solver::setCameraParam(const char *filePath, int camId)
{
    FileStorage fsRead;
    fsRead.open(filePath,FileStorage::READ);
    if(!fsRead.isOpened())
    {
        cout<<"Fialed open"<<endl;
        reuturn -1;
    }

    fsRead
}*/

void Solver::setArmorSize(double Armor_width, double Armor_height)
{
    double half_x=Armor_width/2.0;
    double half_y=Armor_height/2.0;
    switch(Type)
    {
    case SMALL :
        SMALL_ARMOR_POINTS_3D.emplace_back(Point3f(-half_x,half_y,0));//tl
        SMALL_ARMOR_POINTS_3D.emplace_back(Point3f(half_x,half_y,0));//tr
        SMALL_ARMOR_POINTS_3D.emplace_back(Point3f(half_x,-half_y,0));//br
        SMALL_ARMOR_POINTS_3D.emplace_back(Point3f(-half_x,-half_y,0));//bl
        break;

    case BIG :
        BIG_ARMOR_POINTS_3D.emplace_back(Point3f(-half_x,half_y,0));
        BIG_ARMOR_POINTS_3D.emplace_back(Point3f(half_x,half_y,0));
        BIG_ARMOR_POINTS_3D.emplace_back(Point3f(-half_x, half_y, 0));   //bl below left
        BIG_ARMOR_POINTS_3D.emplace_back(Point3f(-half_x, -half_y, 0));  //br below right
        break;

    default: break;
    }
}

void Solver::setBulletSpeed(int bulletSpeed)
{
    BULLET_SPEED=bulletSpeed;
}

void Solver::setTarget(vector<Point2f> contoursPoints, Point2f cenerPoint, ArmorType type)
{
    targetContour=contoursPoints;
    targetCenter=cenerPoint;
    Type=type;
}

void Solver::solveAngles()
{
    Mat _rvec;
    switch (Type) {
    case SMALL:
        solvePnP(SMALL_ARMOR_POINTS_3D, targetContour, CAMERA_MATRIX, DISTORTION_COEFF, _rvec, tVec, false, SOLVEPNP_ITERATIVE); break;
    case BIG:
        solvePnP(BIG_ARMOR_POINTS_3D,targetContour,CAMERA_MATRIX,DISTORTION_COEFF,_rvec, tVec, false, SOLVEPNP_ITERATIVE); break;
    default:
        break;
    }
    GUN_CAM_DISTANCE_Y =0;
    tVec.at<double>(1,0) -=GUN_CAM_DISTANCE_Y;
    double x_pos = tVec.at<double>(0,0);
    double y_pos = tVec.at<double>(1, 0);
    double z_pos = tVec.at<double>(2, 0);
    distance = sqrt(x_pos * x_pos + y_pos * y_pos + z_pos * z_pos);

    if(distance>5000)
        PinHole_solver();
    else
        P4P_solver();
}

void Solver::P4P_solver()
{
    double x_pos = tVec.at<double>(0,0);
    double y_pos = tVec.at<double>(1,0);
    double z_pos = tVec.at<double>(2,0);

    double tan_pitch=y_pos/sqrt(x_pos*x_pos+z_pos*z_pos);
    double tan_yaw=x_pos/z_pos;
    x_pitch=-atan(tan_pitch)*180/CV_PI;
    y_yaw=atan(tan_yaw)*180/CV_PI;
}

void Solver::PinHole_solver()
{
    double fx=CAMERA_MATRIX.at<double>(0,0);
    double fy = CAMERA_MATRIX.at<double>(1, 1);
    double cx = CAMERA_MATRIX.at<double>(0, 2);
    double cy = CAMERA_MATRIX.at<double>(1, 2);
    Point2f pnt;
    vector<Point2f> in;
    vector<Point2f> out;
    in.push_back(targetCenter);

    undistortPoints(in, out, CAMERA_MATRIX, DISTORTION_COEFF, noArray(), CAMERA_MATRIX);
    pnt = out.front();

    double rxNew = (pnt.x - cx)/fx;
    double ryNew = (pnt.y - cy)/fy;

    y_yaw = atan(rxNew)/CV_PI*180;
    x_pitch = -atan(ryNew)/CV_PI*180;
}

void Solver::compensateAngle()
{
    compensateOffset();
    compensateGravity();
}

void Solver::compensateOffset()
{
    float camera_target_height = distance * sin(x_pitch / 180 * CV_PI);
       float gun_target_height = camera_target_height + GUN_CAM_DISTANCE_Y;
       float gun_pitch_tan = gun_target_height / (distance * cos(x_pitch / 180 * CV_PI));
       x_pitch = atan(gun_pitch_tan) / CV_PI * 180;
}

void Solver::compensateGravity()
{
    //
}

void Solver::getAngle(vector<Point2f> &contourPoints, Point2f centerPoint, double &yaw, double &pitch, double &evaluateDistance)
{
    setTarget(contourPoints, centerPoint, Type);
        solveAngles();
        compensateAngle();
        yaw = y_yaw;
        pitch = x_pitch;
        evaluateDistance = distance;
}
