
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <iostream>
#include <cstdlib>
#include "cxcore.h"


using namespace cv;
using namespace std;
using cv::CLAHE;

void show_histogram(std::string const& name, cv::Mat1b const& image)
{
    // Set histogram bins count
    int bins = 256;
    int histSize[] = {bins};
    // Set ranges for histogram bins
    float lranges[] = {0, 256};
    const float* ranges[] = {lranges};
    // create matrix for histogram
    cv::Mat hist;
    int channels[] = {0};

    // create matrix for histogram visualization
    int const hist_height = 256;
    cv::Mat3b hist_image = cv::Mat3b::zeros(hist_height, bins);

    cv::calcHist(&image, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);

    double max_val=0;
    minMaxLoc(hist, 0, &max_val);

    // visualize each bin
    for(int b = 0; b < bins; b++) {
        float const binVal = hist.at<float>(b);
        int   const height = cvRound(binVal*hist_height/max_val);
        cv::line
            ( hist_image
            , cv::Point(b, hist_height-height), cv::Point(b, hist_height)
            , cv::Scalar::all(255)
            );
    }
    cv::imshow(name, hist_image);
}

int main(int argc, char** argv) {
    Mat m = imread("/home/lg_more/Lenna.png", CV_LOAD_IMAGE_GRAYSCALE); //input image
    imshow("lena_GRAYSCALE", m);

    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(3);
    clahe->setTilesGridSize(Size(8,8));
    
    Mat dst;
    clahe->apply(m, dst);
    imshow("lena_CLAHE", dst);
    show_histogram("lena_hist",m);
    show_histogram("lena_clahe_hist",dst);
    waitKey();
}


