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

MatrixXd createCheckerboard(int dimension){
    MatrixXd checkerboard(dimension,dimension);
    int sizeSquare = dimension/8;
    for(int i=0;i<dimension;i++){
        for(int j=0;j<dimension;j++){
            if((i/sizeSquare+j/sizeSquare)%2){
                //White square
                checkerboard(i,j)=255;
            }else{
                //Black square
                checkerboard(i,j)=0;
            }
        }
    }

    return checkerboard;
}

void Compression(Eigen::MatrixXd U, Eigen::MatrixXd V, Eigen::VectorXd S, int k) {
    printf("k = %d\n", k);
    Eigen::MatrixXd C(U.rows(), k);
    Eigen::MatrixXd D(V.rows(), k);
    printf("n_col of U: %d\n", U.cols());
    printf("n_rows of U: %d\n", U.rows());
    printf("n_col of V: %d\n", V.cols());
    printf("n_rows of V: %d\n", V.rows());
    printf("n_col of C: %d\n", C.cols());
    printf("n_rows of C: %d\n", C.rows());
    printf("n_col of D: %d\n", D.cols());
    printf("n_rows of D: %d\n", D.rows());

    for (int i = 0; i < C.cols(); i++) {
        C.col(i) = U.col(i);
    }
    for (int i = 0; i < D.cols(); i++) {
        D.col(i) = V.col(i)*S[i];
    }
    int nonzero_C = (C.array() != 0).count();
    int nonzero_D = (D.array() != 0).count();
    printf("Number of nonzero entries in C: %d\n", nonzero_C);
    printf("Number of nonzero entries in D: %d\n", nonzero_D);
    printf("\n");
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
    Eigen::VectorXd eigenvalues = es.eigenvalues();
    printf("First eigenvalue is %f\n", eigenvalues[eigenvalues.size() - 1]);
    printf("Second eigenvalue is %f\n", eigenvalues[eigenvalues.size() - 2]);

    //Task 3
    saveMarket(gscaleSym,"ATA.mtx");
    cout << "Matrix A^T*A exported" << endl;
    cout << "Computing largest eigeinvalue using LIS: " << endl;
    system("./etest1 ATA.mtx eigvec.mtx hist.txt -e pi -etol 1.e-8");

    //Task 4
    cout << "Computing largest eigeinvalue using LIS with an appropriate shift: " << endl;
    system("./etest1 ATA.mtx eigvec.mtx hist.txt -e pi -etol 1.e-8 -shift 2.0");

    //Task 5
    
    Eigen::BDCSVD<Eigen::MatrixXd> svd(gscale, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::VectorXd S = svd.singularValues();
    printf("The euclidean norm is %f\n", S.norm());

    //Task 6
    Eigen::MatrixXd U = svd.matrixU();
    Eigen::MatrixXd V = svd.matrixV();
    Compression(U, V, S, 40);
    Compression(U, V, S, 80);
    
    //Task 8
    MatrixXd checkerboard = createCheckerboard(200);
    cout << "Norm of the checkerboard matrix: " << checkerboard.norm() << endl;
    cout << "Saving checkerboard to file" << endl;
    saveToFile(checkerboard,200,200,"checkerboard.png");

    //Task 9
    default_random_engine generator;
    uniform_int_distribution<int> distribution(-MARGIN, MARGIN);
    MatrixXd noisy(200, 200);
    for (int i = 0; i < 200; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            int number = distribution(generator);
            noisy(i, j) = checkerboard(i, j) + distribution(generator);
            if (noisy(i, j) < 0)
                noisy(i, j) = 0;
            else if (noisy(i, j) > 255)
                noisy(i, j) = 255;
        }
    }
    cout << "Saving noisy checkerboard to file" << endl;
    saveToFile(noisy,200,200,"noisy_checkerboard.png");
    return 0;
}

