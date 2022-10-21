#include <iostream>
#include <cstdio>
#include <opencv2/opencv.hpp>

float centro, altura, largura, l1, l2, decaimento;

float media[] = {0.1111, 0.1111, 0.1111, 0.1111, 0.1111,
                   0.1111, 0.1111, 0.1111, 0.1111};

int slider_altura = 0;
int slider_altura_max = 100;

int slider_decaimento = 0;
int slider_decaimento_max = 100;

int slider_deslocamento = 0;
int slider_deslocamento_max = 100;

cv::Mat image1, image2;
cv::Mat imageTop;

char TrackbarName[50];

void tiltshift();

void on_trackbar_deslocamento(int, void*){
  centro = slider_deslocamento * altura/100.0;
  tiltshift();
}

void on_trackbar_altura(int, void*){
  int height = altura * slider_altura / 100;
  l1 = -height/2;
  l2 = height/2;
  tiltshift();
}

void on_trackbar_decaimento(int, void*){
  decaimento = slider_decaimento;
  tiltshift();
}

void tiltshift(){
    cv::Mat tilt(altura, largura, CV_32F);
    cv::Mat tilt_negativo(altura, largura, CV_32F);

    for(int i = 0; i < altura; i++){
        float fx = 0.0; // Função que irá ponderar as imagens;
        if(decaimento != 0){
        //Se o decaimento (d) for igual a 0 é necessário alterá-lo para um valor
        //pequeno pois a função tem o d como denominador.
            fx = (1/2)*(tanh(((i-l1)/decaimento) - tanh((i-l2)/decaimento)));
        }else{
             fx = (1/2)*(tanh(((i-l1)/0.01) - tanh((i-l2)/0.01)));
        }

        //APlicação do tilt shift em cada camada da imagem.
        for(int j = 0; j < largura; j++){
            tilt.at<cv::Vec3f>(i, j)[0] = fx;
            tilt.at<cv::Vec3f>(i, j)[1] = fx;
            tilt.at<cv::Vec3f>(i, j)[2] = fx;
            tilt_negativo.at<cv::Vec3f>(i, j)[0] = 1.0 - fx;
            tilt_negativo.at<cv::Vec3f>(i, j)[1] = 1.0 - fx;
            tilt_negativo.at<cv::Vec3f>(i, j)[2] = 1.0 - fx;
        }
    }

    cv::Mat resultado, resultado1, resultado2;

    cv::multiply(image1, tilt, resultado1);
    cv::multiply(image2, tilt_negativo, resultado2);

    cv::addWeighted(resultado1, 1, resultado2, 1, 0, resultado);

    resultado.convertTo(resultado, CV_8UC3);

    imshow("Tilt-shift", resultado);
}

int main(int argvc, char** argv){
  std::vector<cv::Mat> canais;
  cv::Mat mask(3, 3, CV_32F);

  mask = cv::Mat(3, 3, CV_32F, media);

  image1 = cv::imread(argv[1]);
  image1.convertTo(image1, CV_32F);

  cv::split(image1, canais);

  cv::filter2D(canais[0], canais[0], canais[0].depth(), mask,
                 cv::Point(1, 1), 0);
  cv::filter2D(canais[1], canais[1], canais[1].depth(), mask,
                 cv::Point(1, 1), 0);
  cv::filter2D(canais[2], canais[2], canais[2].depth(), mask,
                 cv::Point(1, 1), 0);

  cv::merge(canais, image2);

  largura = image1.cols;
  altura = image1.rows;

  cv::namedWindow("Tilt-shift", 1);

  std::sprintf( TrackbarName, "Altura x %d", slider_altura_max );
  cv::createTrackbar( TrackbarName, "Tiltshift",
                      &slider_altura,
                      slider_altura_max,
                      on_trackbar_altura);
  on_trackbar_altura(slider_altura, 0 );

  std::sprintf( TrackbarName, "Decaimento x %d", slider_decaimento_max );
  cv::createTrackbar( TrackbarName, "Tiltshift",
                      &slider_decaimento,
                      slider_decaimento_max,
                      on_trackbar_decaimento);
  on_trackbar_decaimento(slider_decaimento, 0 );

  std::sprintf( TrackbarName, "Deslocamento x %d", slider_deslocamento_max );
  cv::createTrackbar( TrackbarName, "Tiltshift",
                      &slider_deslocamento,
                      slider_deslocamento_max,
                      on_trackbar_deslocamento);
  on_trackbar_deslocamento(slider_deslocamento, 0 );

  tiltshift();

  cv::waitKey(0);
  return 0;
}
