#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define gH 2
#define gL 0.5
#define c 0.45
#define d0 8

using namespace cv;
using namespace std;

// troca os quadrantes da imagem da DFT
void trocaQuadrantes(Mat& image ){
   cv::Mat tmp, A, B, C, D;

  // se a imagem tiver tamanho impar, recorta a regiao para o maior
  // tamanho par possivel (-2 = 1111...1110)
  image = image(cv::Rect(0, 0, image.cols & -2, image.rows & -2));

  int centerX = image.cols / 2;
  int centerY = image.rows / 2;

  // rearranja os quadrantes da transformada de Fourier de forma que
  // a origem fique no centro da imagem
  // A B   ->  D C
  // C D       B A
  A = image(cv::Rect(0, 0, centerX, centerY));
  B = image(cv::Rect(centerX, 0, centerX, centerY));
  C = image(cv::Rect(0, centerY, centerX, centerY));
  D = image(cv::Rect(centerX, centerY, centerX, centerY));

  // swap quadrants (Top-Left with Bottom-Right)
  A.copyTo(tmp);
  D.copyTo(A);
  tmp.copyTo(D);

  // swap quadrant (Top-Right with Bottom-Left)
  C.copyTo(tmp);
  B.copyTo(C);
  tmp.copyTo(B);
}

int main(int argc, char** argv){
  Mat imaginaryInput, complexImage;
  Mat padded, filter, filter_print;
  Mat image, tmp;
  Mat_<float> zeros;
  vector<Mat> planos, filtros, dfts;

  int dft_M, dft_N;

  image = imread(argv[1], IMREAD_GRAYSCALE);
  imshow("Imagem Inicial", image);

  // identifica os tamanhos otimos para
  // calculo do FFT
  dft_M = getOptimalDFTSize(image.rows);
  dft_N = getOptimalDFTSize(image.cols);

  // realiza o padding da imagem
  copyMakeBorder(image, padded, 0,
                 dft_M - image.rows, 0,
                 dft_N - image.cols,
                 BORDER_CONSTANT, Scalar::all(0));

  // prepara a matriz complexa e preenche com 0
  imaginaryInput = Mat(padded.size(), CV_32FC1, Scalar(0));

  //Trasforma padded em float
  padded.convertTo(padded, CV_32F);

  normalize(padded, padded, 0.0, 1.0, NORM_MINMAX);

  //Soma ambas as componentes com 1 para evitar a operação ln(0)
  cv::log(padded + 1, padded);
  cv::log(imaginaryInput + 1, imaginaryInput);

  //Concatena plano real e imaginário para realizar a DFT
  planos.push_back(padded);
  planos.push_back(imaginaryInput);

  merge(planos, complexImage);

  // Construindo a função de transferência (filtro frequencial) com o
  // mesmo tamanho e tipo da matriz complexa
  filter = Mat((dft_M & -2), (dft_N & -2), CV_32FC2, Scalar(0, 0));

  // Preenche o filtro homomórfico com os valores dadod pela função
  for(int i=0; i< dft_M; i++){
    for(int j=0; j < dft_N; j++){
      float h = (float) (gH - gL)*(1.0 - exp(-c*(pow(abs(i-dft_M/2)+abs(j-dft_N/2), 2)/pow(d0, 2)))) + gL;
      filter.at<Vec2f>(i, j)[0] = h;
      filter.at<Vec2f>(i, j)[1] = h;
    }
  }

  split(filter, filtros);
  normalize(filtros[0], filter_print, 0.0, 1.0, NORM_MINMAX);
  imshow("filtro", filter_print);

  // Efetua a filtragem
  dft(complexImage, complexImage);
  trocaQuadrantes(complexImage);
  mulSpectrums(complexImage,filter,complexImage,0);
  trocaQuadrantes(complexImage);
  idft(complexImage, complexImage, DFT_SCALE);

  cv::exp(complexImage,complexImage);

  planos.clear();
  split(complexImage, planos);

  // normaliza a parte real para exibicao
  normalize(planos[0], tmp, 0.0, 1.0, NORM_MINMAX);
  imshow("Imagem Final", tmp);
  waitKey();
  return 0;
}
