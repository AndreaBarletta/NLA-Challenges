#include <Eigen/Eigen>
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

int saveToFile(MatrixXd imageMatrix, int height, int width, const std::string ouputFileName)
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
        cout << i << endl;
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

    cout << (saveToFile(noisy, height, width, "noisy.png") == 0 ? "Exported noisy image" : "Error Occured") << endl;
    
    //Task 3 (Reshape)
    Map<VectorXd> v(gscale.data(), gscale.size());
    Map<VectorXd> w(noisy.data(), noisy.size());

    int vComponents = v.rows();
    int wComponents = w.rows();

    double norm = sqrt(v.dot(v));
    cout << "The norm is " << norm << endl;
    
    //Task 4 (Hav2)
    // Matrix3d Hav2 = Matrix3d::Ones();
    // Hav2 /= 9.0;
    // SparseMatrix<double> A1 = convToM(Hav2, height, width);
    // cout << "Non-zero entries in A1:" << A1.nonZeros() << endl;

    // //Task 5 (Apply Hav2)
    // VectorXd smoothedw(height*width);
    // smoothedw = A1 * w;
    // MatrixXd smoothed = Map<MatrixXd>(smoothedw.data(),height,width);
    // cout << (saveToFile(smoothed, height, width, "smoothed.png") == 0 ? "Exported smoothed image" : "Error Occured") << endl;

    //Task 6 (Hsh2)
    Matrix3d Hsh2;
    Hsh2 << 0, -3, 0,
            -1, 9,-3,
            0, -1, 0; 
    SparseMatrix<double> A2 = convToM(Hsh2, height, width);
    cout << "Non-zero entries in A2:" << A2.nonZeros() << endl;
    cout << "Symmetric: " << A2.isApprox(A2.transpose()) << endl;

    //Task 6 (Apply Hsh2)
    VectorXd sharpenedv(height*width);
    sharpenedv = A2 * v;
    MatrixXd sharpened = Map<MatrixXd>(sharpenedv.data(),height,width);
    cout << (saveToFile(sharpened, height, width, "sharpened.png") == 0 ? "Exported sharpened image" : "Error Occured") << endl;
    return 0;
}