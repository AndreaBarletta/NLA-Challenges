#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <unsupported/Eigen/SparseExtra>
#include <Eigen/Eigenvalues>
#include <Eigen/SVD>
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
                                        { return static_cast<unsigned char>(std::max(std::min(val,255.0),0.0)); });

    if (stbi_write_png(ouputFileName.c_str(), width, height, 1, outputImage.data(), width) == 0)
    {
        return 1;
    }

    return 0;
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

    MatrixXd gscaleSym(width,width);
    gscaleSym = gscale.transpose()*gscale;
    cout << "Norm of A^T*A: " << gscaleSym.norm() << endl;

    //Task 2

    SelfAdjointEigenSolver<MatrixXd> es;
    es.compute(gscaleSym);
    Eigen::VectorXcd eigenvalues = es.eigenvalues();
    printf("First eigenvalue is %f\n", eigenvalues[eigenvalues.size() - 1]);
    printf("Second eigenvalue is %f\n", eigenvalues[eigenvalues.size() - 2]);

    //Task 5
    
    Eigen::BDCSVD<Eigen::MatrixXd> svd(gscale, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::VectorXd singular_values = svd.singularValues();
    printf("The euclidean norm is %f\n", singular_values.norm());

    //Task 6
    Eigen::MatrixXd U = svd.matrixU();
    Eigen::MatrixXd V = svd.matrixV();
    
    
    return 0;
}