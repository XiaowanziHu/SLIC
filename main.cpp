#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "SLIC.h"

using namespace std;
using namespace cv;

int imgOpenCV2SLIC(Mat img, int &height, int &width, int &dim, unsigned int * &image);
int imgSLIC2openCV(unsigned int *image, int height, int width, int dim, Mat &imgSLIC);
void creatAlphaMat(Mat &mat);

int main()
{
	Mat imgRGB;
	time_t tStart, tEnd, exeT;
	
	imgRGB = imread("yangben1.tif");
	if (imgRGB.empty() == true)
	{
		cout << "can not open rgb image!" << endl;
	}
	
	unsigned int *image;
	
	int height;
	int width;
	int dim;
	long imgSize;
	
	int numlabels(0);
	//SLIC slic;
	int m_spcount = 100;
	int m_compactness = 10;


	imgOpenCV2SLIC(imgRGB, height, width, dim, image);//OpenCV 图像数据转换成SLIC图像数据
	imgSize = height* width;
	
	
	tStart = clock();
	int k = 500;// 所需的超像素数。
	double m = 30;
	int* klabels = nullptr;
	if (0 == klabels) klabels = new int[imgSize];
	//const int kstep = sqrt(double(imgSize) / double(k));
	string filename = "1.jpg";
	string savepath = "yourpathname1";
	//----------------------------------
	// Perform SLIC on the image buffer
	//----------------------------------
	SLIC segment;
	segment.PerformSLICO_ForGivenK(image, width, height, klabels, numlabels, k, m);

	segment.DrawContoursAroundSegments(image, klabels, width, height, 0xff0000);

	// 保存超像素块
	for (int K = 0; K < k; K++)
	{
		Mat imgSLIC;
		imgSLIC.create(height, width, CV_8UC3);
		for (int j = 0; j < height; ++j)
		{
			//获取第 i行首像素指针 
			Vec3b * p = imgSLIC.ptr<Vec3b>(j);
			for (int i = 0; i < width; ++i)
			{
				int index = j*width +i;
				if (klabels[index] == K)
				{
					p[i][0] = (unsigned char)(image[j*width + i] & 0xFF); //Blue 
					p[i][1] = (unsigned char)((image[j*width + i] >> 8) & 0xFF); //Green 
					p[i][2] = (unsigned char)((image[j*width + i] >> 16) & 0xFF); //Red 


				}
			}
		}
		string path = "E:\\LW\\SLIC提取\\SLIC1\\SLIC\\slic\\slic\\test-data3\\" + to_string(K + 1) + ".jpg";
		imwrite(path, imgSLIC);
	}
  //初始标签
	ofstream outfile;
	outfile.open("klabels", ios::binary | ios::app | ios::in | ios::out);
	for (int j = 0; j < height; ++j)
	{
		//获取第 i行首像素指针 
		for (int i = 0; i < width; ++i)
		{
			int index = j*width + i;
			outfile << klabels[index] << " ";
			outfile << "\r";
			outfile << "\n";
		}
	}

	Mat ReImgSLIC;

	imgSLIC2openCV(image, height, width, dim, ReImgSLIC);//SLIC结果：SLIC图像数据转换成OpenCV 图像数据
	//----------------------------------
	// Clean up
	//----------------------------------
	if (image) delete[] image;
	if (klabels) delete[] klabels;
	
	tEnd = clock();
	exeT = tEnd - tStart;
		
    //结果显示
	cout << "SLIC执行时间exeT：" << exeT << "毫秒" << endl;
	imshow("imgRGB", imgRGB);
	imshow("imgSLIC1", ReImgSLIC);
	getchar();
	return 0;
	}


//OpenCV Mat图像数据转换为SLIC图像数据
//输入：Mat img, int &height, int &width, int &dim,
//输出：unsigned int * &image，同时返回转换是否成功的标志：成功为0，识别为1
int imgOpenCV2SLIC(Mat img, int &height, int &width, int &dim, unsigned int * &image)
{
	int error = 0;
	if (img.empty()) //请一定检查是否成功读图 
		{
		         error = 1;
		     }
	
		 dim = img.channels();//图像通道数目
	     height = img.rows;
	     width = img.cols;
	
	     int imgSize = width*height;
	
		     unsigned char *pImage = new unsigned char[imgSize * 4];
	     if (dim == 1)
		 {
		         for (int j = 0; j < height; j++)
				 {
			             uchar * ptr = img.ptr<uchar>(j);
			             for (int i = 0; i < width; i++) 
						 {
				                 pImage[j * width * 4 + 4 * i + 3] = 0;
				                 pImage[j * width * 4 + 4 * i + 2] = ptr[0];
				                 pImage[j * width * 4 + 4 * i + 1] = ptr[0];
				                 pImage[j * width * 4 + 4 * i] = ptr[0];
				                 ptr++;
						}
				}
		}
		else if (dim == 3)
				 {
			             for (int j = 0; j < height; j++)
						 {
				                 Vec3b * ptr = img.ptr<Vec3b>(j);
				                 for (int i = 0; i < width; i++) 
								 {
					                     pImage[j * width * 4 + 4 * i + 3] = 0;
					                     pImage[j * width * 4 + 4 * i + 2] = ptr[0][2];//R
					                     pImage[j * width * 4 + 4 * i + 1] = ptr[0][1];//G
					                     pImage[j * width * 4 + 4 * i] = ptr[0][0];//B        
					                     ptr++;
					
								}	
						}
				}

		 else  error = 1;			
	
		 image = new unsigned int[imgSize];
	     memcpy(image, (unsigned int*)pImage, imgSize*sizeof(unsigned int));
	     delete pImage;
	
		     return error;
	
}


//SLIC图像数据转换为OpenCV Mat图像数据
//输入：unsigned int *image, int height, int width, int dim
//输出：Mat &imgSLIC ，同时返回转换是否成功的标志：成功为0，识别为1
int imgSLIC2openCV(unsigned int *image, int height, int width, int dim, Mat &imgSLIC)
{
	     int error = 0;//转换是否成功的标志：成功为0，识别为1
	
		     if (dim == 1)
			 {
		         imgSLIC.create(height, width, CV_8UC1);
		         //遍历所有像素，并设置像素值 
			         for (int j = 0; j< height; ++j)
			         {
			             //获取第 i行首像素指针 
				             uchar * p = imgSLIC.ptr<uchar>(j);
			             //对第 i行的每个像素(byte)操作 
				             for (int i = 0; i < width; ++i)
				                 p[i] = (unsigned char)(image[j*width + i] & 0xFF);
			         }
	           }

	     else if (dim == 3)
			 {
				 imgSLIC.create(height, width, CV_8UC3);
				 //遍历所有像素，并设置像素值 
				 for (int j = 0; j < height; ++j)
				 {
					 //获取第 i行首像素指针 
					 Vec3b * p = imgSLIC.ptr<Vec3b>(j);
					 for (int i = 0; i < width; ++i)
					 {
						 p[i][0] = (unsigned char)(image[j*width + i] & 0xFF); //Blue 
						 p[i][1] = (unsigned char)((image[j*width + i] >> 8) & 0xFF); //Green 
						 p[i][2] = (unsigned char)((image[j*width + i] >> 16) & 0xFF); //Red 
					 }
				 }
			 }

		         else  error = 1;

		     return error;
	}

