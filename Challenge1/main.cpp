#include <Eigen/Eigen>
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

    MatrixXd gscale(height,width);
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            gscale(i,j)=static_cast<double>(imageData[i*width+j]);
        }
    }

    cout << "Matrix dimension: " << gscale.rows() << "x" << gscale.cols() << endl;
    //Task 2

    //Task 3
    
    Map<VectorXd> v(gscale.data(), gscale.size());
    Map<VectorXd> w(noisy.data(), noisy.size());

    int vComponents = v.rows();
    int wComponents = w.rows();

    if (vComponents == height*width && wComponents == height*width) {
        printf("Number of components is correct\n");
    }

    double norm = sqrt(v.dot(v));
    printf("The norm is %f\n", norm);

    //Task 4

    //height = 100;
    //width = 100;
    MatrixXd smooth = MatrixXd::Ones(3, 3);
    smooth << 1, 2, 3,
    4, 5, 6,
    7, 8, 9;
    Eigen::SparseMatrix<double> A(height*width, height*width);
    bool finishedRow = false;
    bool finishedCol = false;
    int smRow = smooth.rows()/2;
    int smCol = smooth.cols()/2;
    for (int colA = 0; colA < width; colA++)
    {
        for (int rowA = 0; rowA < height; rowA++)
        {
            int idx = colA*height + rowA;
            for (int colIm = (colA < smCol ? 0 : colA - smCol); 
            colIm <= colA + smCol && colIm < width; 
            colIm++)
            {
                for (int rowIm = (rowA < smRow ? 0 : rowA - smRow); 
                rowIm <= rowA + smRow && rowIm < height; 
                rowIm++)
                {
                    A.insert(idx, colIm*height + rowIm) =
                    smooth(smRow - rowA + rowIm, smCol - colA + colIm);
                }
            }
        }
    }
    
    //cout << A << endl;
    printf("Finished!\nDid I win?\n%d\n", A.rows());
    return 0;
}