#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv){
  cv::Mat image, imageCinza,imageCinzaEq, hist;
  cv::VideoCapture cap;
  int width, height;
  int nbins = 64;
  float range[] = {0, 255};
  const float *histrange = { range };
  bool uniform = true;
  bool acummulate = false;
  int key;

  //Escolhe a câmera do celular - DroidCam.
  cap.open(0);

  //Testa se a câmera esta aberta
  if(!cap.isOpened()){
    std::cout << "cameras indisponiveis";
    return -1;
  }

  //Define o tamanho da imagem que irá aparecer em tela
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
  width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

  std::cout << "largura = " << width << std::endl;
  std::cout << "altura  = " << height << std::endl;

  //Cria um histograma com um unsigned de tam histh x histw
  int histw = nbins, histh = nbins/2;
  cv::Mat histImg(histh, histw, CV_8U, cv::Scalar(0));

  while(1){
    //Captura a imagem e converte para a cor cinza
    cap >> image;
    cv::cvtColor(image, imageCinza, cv::COLOR_BGR2GRAY);

    //Faz uma copia da imagem cinza para a imagem a ser equalizada
    imageCinza.copyTo(imageCinzaEq);

    //Utiliza a função equalize do openCV para equalizar a imagem
    cv::equalizeHist(imageCinzaEq, imageCinzaEq);

    //Calculo do histograma
    cv::calcHist(&imageCinzaEq, 1, 0, cv::Mat(), hist, 1,
                 &nbins, &histrange,
                 uniform, acummulate);

    //Normalização do histograma
    cv::normalize(hist, hist, 0, histImg.rows, cv::NORM_MINMAX, -1, cv::Mat());
    histImg.setTo(cv::Scalar(0));

    //Desenha o histograma
    for(int i=0; i<nbins; i++){
      cv::line(histImg,
               cv::Point(i, histh),
               cv::Point(i, histh-cvRound(hist.at<float>(i))),
               cv::Scalar(255), 1, 8, 0);
    }

    //A imagem do histograma é copiada para o canto da tela.
    histImg.copyTo(imageCinzaEq(cv::Rect(0, 0,nbins, histh)));

    //Ao executar o programa, serão exibidas:
    // 1 - A imagem que esta sendo capturada pela câmera;
    // 2 - A imagem capturada em tons de Cinza;
    // 3 - A imagem em tons de cinza equalizada.
    cv::imshow("Imagem Normal", image);
    cv::imshow("Imagem Cinza", imageCinza);
    cv::imshow("Imagem Cinza Equalizada", imageCinzaEq);
    key = cv::waitKey(30);
    if(key == 27) break;
  }
  return 0;
}

