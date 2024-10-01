#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using namespace Eigen;

SparseMatrix<double> convToM(Matrix3d convM, int Aheight, int Awidth)
{
    SparseMatrix<double> m(Aheight * Awidth, Aheight * Awidth);
    for (int i = 0; i < Aheight * Awidth; i++)
    {
        //Top and bottom diagonal
        if (i + Aheight < Aheight * Awidth)
        {
            if ((i - 1 >= 0)&&(i%Aheight!=0))
            {
                //Top
                m.insert(i, i + Aheight - 1) = convM(0, 2);
                //Bottom
                m.insert(i + Aheight, i - 1) = convM(0, 0);
            }

            //Top
            m.insert(i, i + Aheight) = convM(1, 2);
            //Bottom
            m.insert(i + Aheight, i) = convM(1, 0);

            if ((i + Aheight + 1 < Aheight * Awidth)&&(i%Aheight!=Aheight-1))
            {
                //Top
                m.insert(i, i + Aheight + 1) = convM(2, 2);
                //Bottom
                m.insert(i + Aheight, i + 1) = convM(2, 0);
            }
        }

        //Center diagonal
        if ((i - 1 >= 0)&&(i%Aheight!=0))
        {
            m.insert(i, i - 1) = convM(0, 1);
        }
        m.insert(i, i) = convM(1, 1);
        if ((i + 1 < Aheight * Awidth)&&(i%Aheight!=Aheight-1))
        {
            m.insert(i, i + 1) = convM(2, 1);
        }
    }

    return m;
}

int main(int argc, char **argv)
{
    //Task 1 (Load image)
    int width, height, channels;
    unsigned char *imageData = stbi_load("einstein.jpg", &width, &height, &channels, 1);

    MatrixXd gscale(height, width);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            gscale(i, j) = static_cast<double>(imageData[i * width + j]);
        }
    }

    cout << "Matrix dimension: " << gscale.rows() << "x" << gscale.cols() << endl;
    //Task 2 (Noisy image)
    //Task 3 (Reshape)
    //Task 4 (Hav2)
    // VectorXd A1row = VectorXd::Zero(width*height);
    // for (int i = 0; i < width; i++)
    // {
    //     for (int j = 0; j < 3; j++)
    //     {
    //         A1row(i * height + j) = 1 / 9.0;
    //     }
    // }

    // cout << "A1row: " << A1row << endl;

    Matrix3d Hav1;
    Hav1 << 0, -1, 0,
        -1, 4, -1,
        0, -1, 0;

    SparseMatrix<double> m = convToM(Hav1, height, width);
    cout << m.nonZeros() << endl;
    return 0;
}