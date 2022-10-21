#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv){
  cv::Mat image, realce;
  int width, height;
  int nobjects, comBolhas;

  cv::Point p;
  image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

  if(!image.data){
    std::cout << "imagem nao carregou corretamente\n";
    return(-1);
  }

  width=image.cols;
  height=image.rows;
  std::cout << width << "x" << height << std::endl;

  p.x=0;
  p.y=0;

  // -------VERIFICAÇÃO DE BORDAS--------
  // Horizontais superior e inferior
  // Verifica a linha na altura i = 0 e na altura i = height - 1;
  for (int i = 0; i < height; i = i + height - 1) {
        for (int j = 0; j < width; j++) {
            if (image.at<uchar>(i,j) == 255) {
                // achou um objeto
                p.x = j;
                p.y = i;
                floodFill(image, p, 0);
            }
        }
  }
  // Verticais esquerda e direita
  // Verifica a coluna na largura i = 0 e na largura i = width - 1;
  for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j = j + width - 1) {
            if (image.at<uchar>(i,j) == 255) {
                // achou um objeto
                p.x = j;
                p.y = i;
                floodFill(image, p, 0);
            }
        }
  }

  // busca objetos presentes
  nobjects=0;
  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){
      if(image.at<uchar>(i,j) == 255){
        // achou um objeto
        nobjects++;
        p.x=j;
        p.y=i;
  		// preenche o objeto com o contador
        cv::floodFill(image,p,nobjects);
      }
    }
  }

  comBolhas=0;
  p.x = 0;
  p.y = 0;
  floodFill(image, p, 255);
  for(int i=1; i<height; i++){
    for(int j=1; j<width; j++){
      if(image.at<uchar>(i-1, j) > 0
      && image.at<uchar>(i-1,j) < 255
      && image.at<uchar>(i,j) == 0){
        // achou um objeto
        comBolhas++;
        p.x=j;
        p.y=i;
  		// preenche o objeto com o contador
        cv::floodFill(image,p,255);
      }
    }
  }
  std::cout << "a figura tem " << nobjects << " objetos\n";
  std::cout << "a figura tem " << comBolhas << " objetos com bolhas internas\n";
  cv::imshow("image", image);
  cv::imwrite("labeling.png", image);
  cv::waitKey();
  return 0;
}
