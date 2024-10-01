#include <Eigen/Dense>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using namespace Eigen;

int main(int argc, char** argv)
{
    //Task 1
    int width, height, channels;
    unsigned char* imageData = stbi_load("einstein.jpg", &width, &height, &channels, 1);

    Matrix<short,Dynamic,Dynamic> gscale(height,width);
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            gscale(i,j)=static_cast<short>(imageData[i*width+j]);
        }
    }

    cout << "Matrix dimension: " << gscale.rows() << "x" << gscale.cols() << endl;
    //Task 2

    return 0;
}