#include <Eigen/Eigen>
#include <iostream>

using Eigen::Matrix3d;
using namespace std;

int main(int argc, char** argv)
{
        Matrix3d m=Matrix3d::Zero();
        cout << m << endl;
        m.transpose();
        return 0;
}