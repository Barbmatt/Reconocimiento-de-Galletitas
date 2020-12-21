#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <opencv2/core/utility.hpp>
using namespace cv;
using namespace std;

void umbralado(Mat imagen, Mat canal_h, Mat canal_s, Mat canal_v, int valores[6]){
  int cant_pixeles=0;
  int valorh_min = valores[0]; int valorh_max = valores[1];
  int valors_min = valores[2]; int valors_max = valores[3];
  int valorv_min = valores[4]; int valorv_max = valores[5];
  for(int i = 0; i <  imagen.rows; i++) {
      for(int j = 0; j <  imagen.cols; j++) {
        int valorh = canal_h.at<uchar>(i,j)*2;
        int valors = canal_s.at<uchar>(i,j);
        int valorv = canal_v.at<uchar>(i,j);
        if(valorh>=valorh_min && valorh<=valorh_max )
          if(valors>=valors_min && valors<=valors_max )
            if(valorv>=valorv_min && valorv<=valorv_max )
              imagen.at<uchar>(i,j) = 255;
            else
              imagen.at<uchar>(i,j) = 0;
      }
    }
}

Vec2i detectar_pixeles(Mat imagen, Vec3f circle){
  Vec2i pixeles;
  int pixeles_blancos=0;
  int pixeles_negros=0;
  for(int i=circle[1]-circle[2]; i<circle[1]+circle[2]; i++)
    for(int j=circle[0]-circle[2]; j<(circle[0]+circle[2]); j++){
       if(imagen.at<uchar>(i,j)==255){
         pixeles_blancos++;
       }
       else pixeles_negros++;
    }
  pixeles[0] = pixeles_blancos;
  pixeles[1] = pixeles_negros;
  return pixeles;
}

bool detectar_color(Mat imagen, Vec3f circle){
  Vec2i pixeles = detectar_pixeles(imagen,circle);
  int pixeles_blancos  = pixeles[0];
  int pixeles_negros = pixeles[1];
  return pixeles_blancos>9000 && pixeles_negros<150000;
}

bool esta_rota(Mat imagen, Vec3f circle){
  Vec2i pixeles = detectar_pixeles(imagen,circle);
  int pixeles_blancos = pixeles[0];
  int pixeles_negros = pixeles[1];
  int diferencia = pixeles_negros-pixeles_blancos;
  cout << diferencia;
  return  diferencia>100;
}

int main( int argc, char** argv ){
    cv::Mat imagen = imread(argv[1], cv::IMREAD_COLOR);
    if(imagen.empty()) {
        std::cout << "No se pudo encontrar la imagen." << std::endl;
        return EXIT_FAILURE;
    }
   cv::resize(imagen, imagen, cv::Size(imagen.cols * 0.25,imagen.rows * 0.25), 0, 0, INTER_LINEAR);

    imshow("Imagen original", imagen);

    int height = imagen.rows;
    int width = imagen.cols;

    Mat destino;
    cv::cvtColor(imagen, destino, COLOR_BGR2HSV);
    Mat imagen_umbralado_rosa = Mat::zeros(height, width, CV_8U);

    Mat canales_hsv[3]; 
    cv::split(destino, canales_hsv);
    Mat canal_h = canales_hsv[0];
    Mat canal_S = canales_hsv[1];
    Mat canal_V = canales_hsv[2];
    int valores[6];
    valores[0] = 315;
    valores[1] = 350;  
    valores[2] = 40;
    valores[3] = 85;
    valores[4] = 100;
    valores[5] = 200;
    umbralado(imagen_umbralado_rosa,canal_h,canal_S,canal_V,valores);
    imshow("umbralado rosa",imagen_umbralado_rosa);

    valores[0] = 0;
    valores[1] = 35;
    valores[2] = 90;
    valores[3] = 155;
    valores[4] = 30;
    valores[5] = 100;

    Mat imagen_umbralado_marron_oscuro = Mat::zeros(height, width, CV_8U);
    umbralado(imagen_umbralado_marron_oscuro,canal_h,canal_S,canal_V,valores);
    imshow("umbralado marron oscuro",imagen_umbralado_marron_oscuro);

    valores[0] = 20;
    valores[1] = 35;    
    valores[2] = 90;
    valores[3] = 180;
    valores[4] = 70;
    valores[5] = 150;

    Mat imagen_umbralado_marron_claro = Mat::zeros(height, width, CV_8U);
    umbralado(imagen_umbralado_marron_claro,canal_h,canal_S,canal_V,valores);
    imshow("umbralado marron claro",imagen_umbralado_marron_claro); 

    valores[0] = 32;
    valores[1] = 45;    
    valores[2] = 40;
    valores[3] = 150;
    valores[4] = 110;
    valores[5] = 220;

    Mat imagen_umbralado_amarillo = Mat::zeros(height, width, CV_8U);
    umbralado(imagen_umbralado_amarillo,canal_h,canal_S,canal_V,valores);
    imshow("umbralado amarillo",imagen_umbralado_amarillo);

//azul para detectar referencias de perspectiva
    valores[0] = 230;
    valores[1] = 260;
    valores[2] = 120;
    valores[3] = 200;
    valores[4] = 60;
    valores[5] = 90;

    Mat imagen_umbralado_azul = Mat::zeros(height, width, CV_8U);
    umbralado(imagen_umbralado_azul,canal_h,canal_S,canal_V,valores);

//perspectiva
  Point2f puntos_origen[4];
  bool punto1_leido=false;
  bool punto2_leido=false;
  bool punto3_leido=false;
  bool punto4_leido=false;
  for(int i=0; i<imagen.rows; i++)
    for(int j=0; j<imagen.cols; j++){
      if(imagen_umbralado_azul.at<uchar>(i,j)==255){
          if(i<imagen.rows/2 && j<imagen.cols/2){
            if(!punto1_leido){
              puntos_origen[0] = Point2f(j,i);
              imagen_umbralado_azul.at<uchar>(i,j)=100;
              punto1_leido=true;
            }
          }
          else 
            if(j>imagen.cols/2 && i<imagen.rows/2){
              if(!punto2_leido){
                puntos_origen[1] = Point2f(j,i);
                imagen_umbralado_azul.at<uchar>(i,j)=100;
                punto2_leido=true;
              }
            }
            else
              if(i>imagen.rows/2 && j<imagen.cols/2){
                if(!punto3_leido){
                  puntos_origen[2] = Point2f(j,i);
                  imagen_umbralado_azul.at<uchar>(i,j)=100;
                  punto3_leido=true;
                }
              }
              else 
                if(i>imagen.rows/2 && j>(imagen.cols)/2){
                  if(!punto4_leido){
                  imagen_umbralado_azul.at<uchar>(i,j)=100;
                  puntos_origen[3] = Point2f(j,i);
                  punto4_leido=true;
                  }
                }
      }
    }

  Point2f puntos_destino[4];

    puntos_destino[0] = Point2f(0,0);
    puntos_destino[1] = Point2f(width,0);
    puntos_destino[2] = Point2f(0,height);
    puntos_destino[3] = Point2f(width,height);

    Mat M = getPerspectiveTransform(puntos_origen,puntos_destino);
    Mat transformada;
    warpPerspective(imagen,transformada,M,Size(width,height));
    
    imshow("perspectiva", transformada);

  //filtros
  Mat filtro = Mat::zeros(height, width, CV_8U);
  medianBlur(transformada,filtro,3);

  Mat gray  = Mat::zeros(height, width, CV_8U); 
  cvtColor( filtro, gray, COLOR_BGR2GRAY );

 int rosas = 0;
 int marron_oscuro=0;
 int marron_claro=0;
 int amarillas = 0;
 int cantidad_galletitas_rotas=0;
 vector<Vec3f> circles;
  HoughCircles(gray, circles, HOUGH_GRADIENT,2, gray.rows/16, 150,70);

  for ( size_t i = 0; i < circles.size(); i++ ) {
     Point center(cvRound(circles[i][0]),
                  cvRound(circles[i][1]));
     int radius = cvRound(circles[i][2]);

     circle( imagen, center, radius, Scalar(0,0,255), 3);

    bool es_rosa = detectar_color(imagen_umbralado_rosa, circles[i]);
    if(es_rosa){
      circle( filtro, center, radius, Scalar(128,0,255), 3);
      rosas++;
      if(esta_rota(imagen_umbralado_rosa,circles[i]))
        cantidad_galletitas_rotas++;
    }
    else{
      bool es_marron_oscuro = detectar_color(imagen_umbralado_marron_oscuro, circles[i]);
      if(es_marron_oscuro){
        circle( filtro, center, radius, Scalar(0,75,120), 3);
        marron_oscuro++;
        if(esta_rota(imagen_umbralado_marron_oscuro,circles[i]))
          cantidad_galletitas_rotas++;
      }
      else{
        bool es_amarillo = detectar_color(imagen_umbralado_amarillo, circles[i]);
          if(es_amarillo){
            circle( filtro, center, radius, Scalar(0,255,255), 3);
            amarillas++;
            if(esta_rota(imagen_umbralado_amarillo,circles[i]))
              cantidad_galletitas_rotas++;
          }
        else{
          bool es_marron_claro = detectar_color(imagen_umbralado_marron_claro, circles[i]);
          if(es_marron_claro){
            circle( filtro, center, radius, Scalar(80,100,170), 3);
              marron_claro++;
            if(esta_rota(imagen_umbralado_marron_claro,circles[i]))
              cantidad_galletitas_rotas++;
          }
        }
      }
    }
  }

  cout << "cantidad de galletitas rosas: " << rosas << "\n";
  cout << "cantidad de galletitas marrón oscuro: " << marron_oscuro << "\n";
  cout << "cantidad de galletitas marrón claro: " << marron_claro << "\n";
  cout << "cantidad de galletitas amarillas: " << amarillas << "\n";
  cout << "cantidad de galletitas rotas: " << cantidad_galletitas_rotas << "\n";
  
  imshow("Resultado final",filtro);


    waitKey();
    return 0;
}