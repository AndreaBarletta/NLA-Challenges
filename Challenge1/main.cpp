#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <iostream>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MARGIN 50

using namespace std;
using namespace Eigen;

int saveToFile(MatrixXd imageMatrix, int width, int height, const std::string ouputFileName)
{
    Matrix<unsigned char, Dynamic, Dynamic, RowMajor> outputImage(width, height);
    outputImage = imageMatrix.unaryExpr([](double val) -> unsigned char
                                        { return static_cast<unsigned char>(val); });

    if (stbi_write_png(ouputFileName.c_str(), width, height, 1, outputImage.data(), width) == 0)
    {
        return 1;
    }

    return 0;
}

SparseMatrix<double> convToM(Matrix3d convM, int Aheight, int Awidth)
{
    SparseMatrix<double> m(Aheight * Awidth, Aheight * Awidth);
    for (int i = 0; i < Aheight * Awidth; i++)
    {
        //Top and bottom diagonal
        if (i + Aheight < Aheight * Awidth)
        {
            if ((i - 1 >= 0) && (i % Aheight != 0))
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

            if ((i + Aheight + 1 < Aheight * Awidth) && (i % Aheight != Aheight - 1))
            {
                //Top
                m.insert(i, i + Aheight + 1) = convM(2, 2);
                //Bottom
                m.insert(i + Aheight, i + 1) = convM(2, 0);
            }
        }

        //Center diagonal
        if ((i - 1 >= 0) && (i % Aheight != 0))
        {
            m.insert(i, i - 1) = convM(0, 1);
        }
        m.insert(i, i) = convM(1, 1);
        if ((i + 1 < Aheight * Awidth) && (i % Aheight != Aheight - 1))
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
    default_random_engine generator;
    uniform_int_distribution<int> distribution(-MARGIN, MARGIN);
    MatrixXd noisy(height, width);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int number = distribution(generator);
            noisy(i, j) = gscale(i, j) + distribution(generator);
            if (noisy(i, j) < 0)
                noisy(i, j) = 0;
            else if (noisy(i, j) > 255)
                noisy(i, j) = 255;
        }
    }

    cout << (saveToFile(noisy, width, height, "noisy.png") == 0 ? "Exported noisy image" : "Error Occured") << endl;
    
    //Task 3 (Reshape)
    Map<Matrix<double,Dynamic,Dynamic>> v(gscale.data(), gscale.size());
    
    //Task 4 (Hav2)
    Matrix3d Hav1 = Matrix3d::Ones();
    Hav1 /= 8.0;

    SparseMatrix<double> m = convToM(Hav1, height, width);
    cout << m.nonZeros() << endl;

    return 0;
}