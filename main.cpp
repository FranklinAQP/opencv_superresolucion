#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <opencv\cxcore.h>
//#include "CImg.h"
using namespace std;
using namespace cv;
//using namespace cimg_library;

const string windowHisto = "Histograma";
const string windowName = "Video Original";
const string windowName1 = "Imagen HSV";
const string WindowsGrab = "Grabacion";

double cubicInterpolate (double p[4], double x) {
	return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
}

double bicubicInterpolate (double p[4][4], double x, double y) {
	double arr[4];
	arr[0] = cubicInterpolate(p[0], y);
	arr[1] = cubicInterpolate(p[1], y);
	arr[2] = cubicInterpolate(p[2], y);
	arr[3] = cubicInterpolate(p[3], y);
	return cubicInterpolate(arr, x);
}

void run_bicubic (Mat& img, Mat& z_img, int f) {
	int i, j, k, l, c;
	double arr[4][4];
	double color[3];
	Vec3b* pixel3b;
	//imshow( "Imagen original", img );
	//waitKey(0);

	cout<<"Creando matriz z:img\n";
	//imshow( "Image nueva iniciada", z_img );
	//waitKey(0);
	//cout<<"Stop saltado\n";
	/*
	for(i=0; i<z_img.rows; i++) {
		for(j=0; j<z_img.cols; j++) {
            pixel3b = &(z_img.at<Vec3b>(Point(j, i)) );
			pixel3b->val[0]=123;
			pixel3b->val[1]=200;
			pixel3b->val[2]=12;
		}
    }

	cout<<"Modificando matriz z:img\n";
	imshow( "Image nueva iniciada", z_img );
	waitKey(0);
    */
	for(i=0; i<z_img.rows; i++) {
		for(j=0; j<z_img.cols; j++) {
			//Para R,G,B
			for(c=0; c<3; c++) {
				for(l=0; l<4; l++)
					for(k=0; k<4; k++){
                        pixel3b = &(img.at<Vec3b>(Point( j/f +k, i/f +l)) );
                        arr[l][k] = double(pixel3b->val[c] );
					}
				color[c] = bicubicInterpolate(arr, (double)(i%f)/f, (double)(j%f)/f);
			}
			pixel3b = &(z_img.at<Vec3b>(Point(j, i)) );
			pixel3b->val[0]=color[0];
			pixel3b->val[1]=color[1];
			pixel3b->val[2]=color[2];
			//z_img.draw_point(i,j,color);
		}
	}
	return;
}

void ejemplo_bicubic(){
    int factor = 3;
    Mat img;
    img = imread("phi.jpg");
    Mat newimg = Mat::eye(factor*(img.rows-1)-factor+1, factor*(img.cols-2)-factor+1, CV_8UC3);
    run_bicubic(img, newimg, factor);
    imshow( "Image original", img );
    imshow( "Imagen alterada", newimg );
    waitKey(0);
}

void histograma(Mat& fuente, vector<Mat>& planos_BGR, Mat& histImage){
    split( fuente, planos_BGR );
    int histSize = 256;
    float range[] = { 0, 256 } ;
    const float* histRange = { range };
    bool _uniform = true, _accumulate = false;
    Mat b_hist, g_hist, r_hist;
     /// Clacular el histograma:
    calcHist( &planos_BGR[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, _uniform, _accumulate );
    calcHist( &planos_BGR[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, _uniform, _accumulate );
    calcHist( &planos_BGR[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, _uniform, _accumulate );
    // Histogramas para B G R
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );
    histImage = Mat( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );//histImageb;
    /// normaliza los resultados a [ 0, histImage.rows ]
    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

    /// Dibuja para cada canal
    for( int i = 1; i < histSize; i++ ){
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                       Scalar( 0, 255, 0), 2, 8, 0  );
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                       Scalar( 0, 0, 255), 2, 8, 0  );
    }
}

void display_img_prueba(){
    Mat image;
    image = imread("phi.jpg");
    vector<Mat> _rgb;
    split( image, _rgb );
    cout << "\nIMAGEN RGB\ncols: " <<image.cols<<endl;
    cout << "dims: " <<image.dims<<endl;
    cout << "flags: " <<image.flags<<endl;
    cout << "rows: " <<image.rows<<endl;
    cout << "size: " <<image.size<<endl;
    cout << "capas de jpg: " << _rgb.size()<<endl;
    Mat gray_image;
    cvtColor( image, gray_image, CV_BGR2GRAY );
    cout << "\n\nIMAGEN GRIS\ncols: " <<gray_image.cols<<endl;
    cout << "dims: " <<gray_image.dims<<endl;
    cout << "flags: " <<gray_image.flags<<endl;
    cout << "rows: " <<gray_image.rows<<endl;
    cout << "size: " <<gray_image.size<<endl;
    cout << "valor de un pixel: " << 1<<endl;


    if(! image.data ){
        cout <<  "Could not open or find the image" << std::endl ;
    }else{
        //imwrite( "phi_gris.jpg", gray_image );
        //imshow( "phi.jpg", image );
        //imshow( "Gray image", gray_image );
        //waitKey(0);
        for(int y=0;y<image.rows;y++){
            for(int x=0;x<image.cols;x++){
                Vec3b* color = &(image.at<Vec3b>(Point(x,y)) );
                if(color->val[0] > 150 && color->val[1] > 150 && color->val[2] > 150){
                    color->val[0] = 0;
                    color->val[1] = 0;
                    color->val[2] = 0;
                    //cout << "Pixel >200 :" << x << "," << y << endl;
                }else{
                    color->val[0] = 255;
                    color->val[1] = 255;
                    color->val[2] = 255;
                }
                //image.at<Vec3b>(Point(x,y)) = color;
            }
        }
        imshow( "phi.jpg", image );
        imshow( "Gray image", gray_image );
        waitKey(0);
    }
}

void filtro(Mat& src, Mat& dst, int ind){
    Point anchor=Point(-1,-1);
    int ddepth = -1;
    double delta = 0;
    int kernel_size = 3;
    Mat kernel;
    //int c;
    //namedWindow( window_name, CV_WINDOW_AUTOSIZE );
    //c = waitKey(500);
    kernel_size = 3 + 2*( ind%5 );
    kernel = Mat::ones( kernel_size, kernel_size, CV_32F )/ (float)(kernel_size*kernel_size);
    filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
}

int main()
{

    //display_img_prueba();
    //ejemplo_bicubic();



    //Configuración
    bool grabar_frames_input=false;
    bool grabar_frames_output=false;
    bool grabar_video_output=true;
    int factor = 3;
    //Fin Configuracion



    Mat cameraFeed;
	Mat threshold;
	Mat HSV;
	Mat histo;


    const string filename = "video.MP4";
	VideoCapture captura;
	captura.open(filename);

	VideoWriter grabar;

    if ( !captura.isOpened() ) {
       cout << "No se pudo abrir el archivo" << endl;
       return 1;
     }
    //Recopilando datos del video
    double fps = captura.get(CV_CAP_PROP_FPS);//fps del video
    const int FRAME_WIDTH = captura.get(CV_CAP_PROP_FRAME_WIDTH);//ancho de los frames
    const int FRAME_HEIGHT = captura.get(CV_CAP_PROP_FRAME_HEIGHT);//alto de los frames
    const int FRAME_COUNT = captura.get(CV_CAP_PROP_FRAME_COUNT);//Number of frames in the video file.

    double sec = captura.get(CV_CAP_PROP_POS_MSEC);//Current position of the video file in milliseconds or video capture timestamp.
    double frames = captura.get(CV_CAP_PROP_POS_FRAMES);// 0-based index of the frame to be decoded/captured next.
    double POS_AVI_RATIO = captura.get(CV_CAP_PROP_POS_AVI_RATIO);//Relative position of the video file: 0 - start of the film, 1 - end of the film.
    double FOURCC = captura.get(CV_CAP_PROP_FOURCC);//4-character code of codec.

    double format = captura.get(CV_CAP_PROP_FORMAT);//Format of the Mat objects returned by retrieve() .
    double mode = captura.get(CV_CAP_PROP_MODE);//Backend-specific value indicating the current capture mode.
    cout<<"\nfps: "<<fps<<endl;
    cout<<"FRAME_WIDTH: "<<FRAME_WIDTH<<endl;
    cout<<"FRAME_HEIGHT: "<<FRAME_HEIGHT<<endl;
    cout<<"FRAME_COUNT: "<<FRAME_COUNT<<endl;
    cout<<"sec: "<<sec<<endl;
    cout<<"frames: "<<frames<<endl;
    cout<<"POS_AVI_RATIO: "<<POS_AVI_RATIO<<endl;
    cout<<"FOURCC: "<<FOURCC<<endl;
    cout<<"format: "<<format<<endl;
    cout<<"mode: "<<mode<<endl;

    Size frameSize(factor*(FRAME_WIDTH-1)-factor+1, factor*(FRAME_HEIGHT-2)-factor+1);;
    vector<Mat> planos_BGR;

    Mat transformFeed = Mat::eye(factor*(FRAME_HEIGHT-2)-factor+1, factor*(FRAME_WIDTH-1)-factor+1, CV_8UC3);
    Mat filtroFeed = Mat::eye(factor*(FRAME_HEIGHT-2)-factor+1, factor*(FRAME_WIDTH-1)-factor+1, CV_8UC3);



    int n=0;
    char nombre[200];
    char videooutput[200];
    if(grabar_video_output){
        sprintf(videooutput,"frame%d.png",n);
    }
    if(grabar_video_output){
        string filename_output = "final.avi";
        //fcc
        int fcc = CV_FOURCC('M','J','P','G');//CV_FOURCC('D','I','V','3');
        int fps = 12;
        grabar = VideoWriter(filename_output, fcc, fps, frameSize);
        if(!grabar.isOpened()){
            cout<<"ERROR al grabar archivo"<<endl;
            getchar();
            return -1;
        }
    }
    //VideoWriter video("out.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);

	while(n<FRAME_COUNT){

        ++n;
		captura.read(cameraFeed);

        if(grabar_frames_input){
            sprintf(nombre,"frame%d.png",n);
            cout<<"nombre: "<<nombre<<endl;
            imwrite(nombre,cameraFeed);
        }
        if(grabar_video_output){
            run_bicubic(cameraFeed, transformFeed, factor);
            filtro(transformFeed,filtroFeed, 5);
        }

		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);

        if(grabar_video_output){
            histograma(cameraFeed, planos_BGR, histo);
            grabar.write(transformFeed);
            imshow(WindowsGrab, filtroFeed );
            imshow(windowHisto,histo);
            //split(transformFeed, spl);
            //for (int i =0; i < 3; ++i)
              //  spl[i] = figura;
            //mergeq            w(spl, figura);
            //grabar.write(figura);
        }


		imshow(windowName,cameraFeed);
		imshow(windowName1,HSV);
        switch(waitKey(1)){
            case 27://ESC
                return 0;
        }
		waitKey(10);
	}

    return 0;
}
