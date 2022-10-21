#include <iostream>
#include <opencv2/opencv.hpp>

int main(int, char**){
  cv::Mat image;
  cv::Vec3b val;

  int P1[2];
  int P2[2];

  image= cv::imread("biel.png",cv::IMREAD_GRAYSCALE);
  if(!image.data)
    std::cout << "nao abriu biel.png" << std::endl;

// -------------PRIMEIRA COORDENADA (P1)-------------
  std::cout << "Digite as coordenadas P1(x,y)" << std::endl;
  std::cin >> P1[0];
  while(P1[0] > image.rows || P1[0] < 0){
    std::cout << "Linha invalida. Digite novamente.\n";
    std::cin >> P1[0];
  }
  std::cin >> P1[1];
  while(P1[1] > image.cols || P1[1] < 0){
    std::cout << "Coluna invalida. Digite novamente.\n";
    std::cin >> P1[1];
  }

// --------------SEGUNDA COORDENADA (P2)----------
  std::cout << "Digite as coordenadas P2(x,y)" << std::endl;
  std::cin >> P2[0];
  while(P2[0] > image.rows || P2[0] < 0){
    std::cout << "Linha invalida. Digite novamente.\n";
    std::cin >> P2[0];
  }

  std::cin >> P2[1];
  while(P2[1] > image.cols || P2[1] < 0){
    std::cout << "Coluna invalida. Digite novamente.\n";
    std::cin >> P2[1];
  }

//-------------------------------------

  for(int i=P1[0];i<P2[0];i++){
    for(int j=P1[1];j<P2[1];j++){
      image.at<uchar>(i,j)=255-image.at<uchar>(i,j);
    }
  }

  cv::imshow("Regioes", image);
  cv::waitKey();
}
