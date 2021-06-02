// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	printf("Press any key to continue ...\n");
	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]
		
		Mat src = imread(fname,IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height,width,CV_8UC1);
		// Asa se acceseaaza pixelii individuali pt. o imagine cu 8 biti/pixel
		// Varianta ineficienta (lenta)
		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				uchar val = src.at<uchar>(i,j);
				uchar neg = 255 - val;
				dst.at<uchar>(i,j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image",src);
		imshow("negative image",dst);
		waitKey();
	}
}

void testParcurgereSimplaDiblookStyle()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// the fastest approach using the “diblook style”
		uchar *lpSrc = src.data;
		uchar *lpDst = dst.data;
		int w = (int) src.step; // no dword alignment is done !!!
		for (int i = 0; i<height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i*w + j];
				lpDst[i*w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image",src);
		imshow("negative image",dst);
		waitKey();
	}
}

int isInside(Mat img, int i, int j) {
	int height = img.rows;
	int width = img.cols;

	if ((height > i && i >= 0) && ((width > j && j >= 0))) {
		return 1;
	}

	return 0;
}

Mat selectImage()
{
	char fname[MAX_PATH];
	Mat src = imread(fname);
	return src;
}

Mat color2Gray(Mat src)
{
	int height = src.rows;
	int width = src.cols;

	Mat dst = Mat(height, width, CV_8UC1);

	// Asa se acceseaaza pixelii individuali pt. o imagine RGB 24 biti/pixel
	// Varianta ineficienta (lenta)
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Vec3b v3 = src.at<Vec3b>(i, j);
			uchar b = v3[0];
			uchar g = v3[1];
			uchar r = v3[2];
			dst.at<uchar>(i, j) = (r + g + b) / 3;
		}
	}
	return dst;
}


void testResize()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1,dst2;
		//without interpolation
		resizeImg(src,dst1,320,false);
		//with interpolation
		resizeImg(src,dst2,320,true);
		imshow("input image",src);
		imshow("resized image (without interpolation)",dst1);
		imshow("resized image (with interpolation)",dst2);
		waitKey();
	}
}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
		{
			printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
				x, y,
				(int)(*src).at<Vec3b>(y, x)[2],
				(int)(*src).at<Vec3b>(y, x)[1],
				(int)(*src).at<Vec3b>(y, x)[0]);
		}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}


//OCR
char* tesseractExtract(Mat src) {

	tesseract::TessBaseAPI* tess = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (tess->Init("C:\\src\\tessdata", "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	tess->SetImage((uchar*)src.data, src.size().width, src.size().height, src.channels(), src.step1());
	tess->SetSourceResolution(70);
	tess->Recognize(0);
	char* out = tess->GetUTF8Text();
	
	// cleanup
	tess->Clear();

	return out;
}


void extractTextTest(Mat src)
{		
	char* text = tesseractExtract(src);
	printf("%s", text);
	
}

int* histogram(Mat src) {

	int* histValues = (int*)calloc(256, sizeof(int));

	// Asa se acceseaaza pixelii individuali pt. o imagine RGB 24 biti/pixel
	// Varianta ineficienta (lenta)
	int height = src.rows;
	int width = src.cols;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			histValues[src.at<uchar>(i, j)]++;
		}
	}
	return histValues;
}

int getObjectArray(Mat src, int row, int col) {
	std::queue<Point2i> Q;
	Q.push(Point(row, col));
	int count = 0;
	int dj[8] = { 1,  1,  0, -1, -1, -1, 0, 1 }; // rand (coordonata orizontala)
	int di[8] = { 0, -1, -1, -1,  0,  1, 1, 1 }; // coloana (coordonata verticala)
	Mat labelMatrix = Mat::zeros(src.rows, src.cols, CV_8UC1);
	labelMatrix.at<uchar>(row, col) = 1;
	while (!Q.empty()) {
		Point currentPoint = Q.front();
		Q.pop();
		int x = currentPoint.x;
		int y = currentPoint.y;
		for (int k = 0; k < 8; k++) {
			if (isInside(src, x + di[k], y + dj[k])) {
				if (src.at<uchar>(x + di[k], y + dj[k]) == 255 && labelMatrix.at<uchar>(x + di[k], y + dj[k]) == 0) {
					Q.push(Point(x + di[k], y + dj[k]));
					labelMatrix.at<uchar>(x + di[k], y + dj[k]) = 1;
					count++;
				}
			}
		}
	}
	return count;
}

void fillWithBlack(Mat src, int row, int col) {
	std::queue<Point2i> Q;
	Q.push(Point(row, col));
	int count = 0;
	int dj[8] = { 1,  1,  0, -1, -1, -1, 0, 1 }; // rand (coordonata orizontala)
	int di[8] = { 0, -1, -1, -1,  0,  1, 1, 1 }; // coloana (coordonata verticala)
	src.at<uchar>(row, col) = 0;
	while (!Q.empty()) {
		Point currentPoint = Q.front();
		Q.pop();
		int x = currentPoint.x;
		int y = currentPoint.y;
		for (int k = 0; k < 8; k++) {
			if (isInside(src, x + di[k], y + dj[k])) {
				if (src.at<uchar>(x + di[k], y + dj[k]) == 255) {
					Q.push(Point(x + di[k], y + dj[k]));
					src.at<uchar>(x + di[k], y + dj[k]) = 0;
				}
			}
		}
	}
}


void getLimitPoints(Mat src, int row, int col, int *minX, int *maxX, int *minY, int *maxY) {
	std::queue<Point2i> Q;
	Q.push(Point(row, col));
	int count = 0;
	int dj[8] = { 1,  1,  0, -1, -1, -1, 0, 1 }; // rand (coordonata orizontala)
	int di[8] = { 0, -1, -1, -1,  0,  1, 1, 1 }; // coloana (coordonata verticala)
	src.at<uchar>(row, col) = 0;
	while (!Q.empty()) {
		Point currentPoint = Q.front();
		if (currentPoint.x < *minX) {
			*minX = currentPoint.x;
		}
		if (currentPoint.x > *maxX) {
			*maxX = currentPoint.x;
		}
		if (currentPoint.y < *minY) {
			*minY = currentPoint.y;
		}
		if (currentPoint.y > *maxY) {
			*maxY = currentPoint.y;
		}
		Q.pop();
		int x = currentPoint.x;
		int y = currentPoint.y;
		for (int k = 0; k < 8; k++) {
			if (isInside(src, x + di[k], y + dj[k])) {
				if (src.at<uchar>(x + di[k], y + dj[k]) == 255) {
					Q.push(Point(x + di[k], y + dj[k]));
					src.at<uchar>(x + di[k], y + dj[k]) = 0;
				}
			}
		}
	}
}

Mat binarizare(Mat src)
{
	int height = src.rows;
	int width = src.cols;

	int* hist = histogram(src);

	int maxInt = -1, minInt = -1;
	for (int i = 0; i <= 255; i++) {
		if (hist[i] != 0 && minInt == -1) {
			minInt = i;
		}
		if (hist[255 - i] != 0 && maxInt == -1) {
			maxInt = 255 - i;
		}
	}

	float t = (maxInt + minInt) / 2.0f;
	float err = 0.1;
	float prevT = 0;

	while (abs(t - prevT) > err) {
		int m1 = 0, m2 = 0, n1 = 0, n2 = 0;
		for (int i = minInt; i <= t; i++) {
			n1 += hist[i];
			m1 += hist[i] * i;
		}
		m1 /= n1;
		for (int i = t + 1; i <= maxInt; i++) {
			n2 += hist[i];
			m2 += hist[i] * i;
		}
		m2 /= n2;
		prevT = t;
		t = (m1 + m2) / 2.0f;
	}

	Mat dst = Mat(height, width, CV_8UC1);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			uchar val = src.at<uchar>(i, j);
			dst.at<uchar>(i, j) = val < t ? 0 : 255;
		}
	}

	return dst;
		
}

void showOutline(Point start, std::vector<int> dirs, Mat dst) {
	Point directions[] = { Point(0, 1),	//0
							Point(-1, 1),	//1
							Point(-1, 0) ,	//2
							Point(-1,-1) ,	//3
							Point(0, -1) ,	//4
							Point(1, -1) ,	//5
							Point(1, 0) ,	//6
							Point(1, 1) };	//7

	Point currentPoint = start;
	dst.at<uchar>(currentPoint.x, currentPoint.y) = 0;
	for (int i = 0; i < dirs.size(); i++) {
		currentPoint += directions[dirs.at(i)];
		dst.at<uchar>(currentPoint.x, currentPoint.y) = 0;
	}
	imshow("Destination", dst);
	waitKey();
}

void getOutline(Mat src) {

	int height = src.rows;
	int width = src.cols;

	Mat dst = Mat(height, width, CV_8UC1);
	std::vector<Point > outline;

	Point directions[] = { Point(0, 1),	//0
							Point(-1, 1),	//1
							Point(-1, 0) ,	//2
							Point(-1,-1) ,	//3
							Point(0, -1) ,	//4
							Point(1, -1) ,	//5
							Point(1, 0) ,	//6
							Point(1, 1) };	//7

	int currentDir = 7, pastDir;
	std::vector<int > dirs;
	Point currentPoint = Point(-1, -1);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (src.at<uchar>(i, j) == 0) {
				currentPoint = Point(i, j);
				outline.push_back(currentPoint);
				if (currentDir % 2 == 0) {
					currentDir = (currentDir + 7) % 8;
				}
				else {
					currentDir = (currentDir + 6) % 8;
				}
				dirs.push_back(currentDir);
				break;
			}
		}
		if (outline.size() != 0) {
			break;
		}
	}
	printf("HERE\n");
	while (outline.size() < 3 || (outline.at(outline.size() - 1) != outline.at(1) && outline.at(outline.size() - 2) != outline.at(0))) {
		Point testPoint = currentPoint + directions[currentDir];
		if (isInside(src, testPoint.x, testPoint.y)) {
			while (src.at<uchar>(testPoint.x, testPoint.y) != 0) {
				currentDir = (currentDir + 1) % 8;
				testPoint = currentPoint + directions[currentDir];
			}
			dirs.push_back(currentDir);
			currentPoint = testPoint;
			outline.push_back(currentPoint);
			if (currentDir % 2 == 0) {
				currentDir = (currentDir + 7) % 8;
			}
			else {
				currentDir = (currentDir + 6) % 8;
			}
		}
	}
	printf("HERE2\n");
	printf("\nDirectii:%d, \n", dirs.at(0));
	for (int i = 1; i < outline.size(); i++) {
		printf("%d, ", dirs.at(i));
	}
	showOutline(outline.at(0), dirs, dst);

	printf("\nDerivata:\n");
	std::vector<int> derivata;
	for (int i = 1; i < dirs.size(); i++) {
		int currentDeriv = dirs.at(i) - dirs.at(i - 1);
		currentDeriv = currentDeriv < 0 ? 8 + currentDeriv : currentDeriv;
		derivata.push_back(currentDeriv);
		printf("%d, ", currentDeriv);
	}
}

Mat getIntrestZone(Mat src, Mat binSrc)
{
	int height = src.rows;
	int width = src.cols;
	int arrayPrediction = width * height / 4;
	int minX = height;
	int maxX = 0;
	int minY = width;
	int maxY = 0;

	Mat mask = Mat(height, width, CV_8UC1);
	Mat clone = binSrc.clone();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (binSrc.at<uchar>(i, j) == 255) {
				
				int currentArray = getObjectArray(clone, i, j);
				if (currentArray < arrayPrediction) {
					fillWithBlack(clone, i, j);
				}
				else {
					printf("%d\n", currentArray);
					getLimitPoints(clone, i, j, &minX, &maxX, &minY, &maxY);
					printf("minX %d maxX %d minY %d maxY %d\n", minX, maxX, minY, maxY);
					j = width;
					i = height;
				}
			}
		}
	}
	int finalHeight = maxX - minX;
	int finalWidth = maxY - minY;
	Mat dst = Mat(finalHeight, finalWidth, CV_8UC1);
	for (int i = 0; i < finalHeight; i++) {
		for (int j = 0; j < finalWidth; j++) {
			dst.at<uchar>(i, j) = src.at<uchar>(i + minX, j + minY);
		}
	}
	return dst;

}

Mat getZone(Mat src, float xMinScale, float xMaxScale, float yMinScale, float yMaxScale) {
	int height = src.rows;
	int width = src.cols;
	int minX = height / xMinScale;
	int maxX = height / xMaxScale;
	int minY = width / yMinScale;
	int maxY = width / yMaxScale;

	int finalHeight = maxX - minX;
	int finalWidth = maxY - minY;
	Mat dst = Mat(finalHeight, finalWidth, CV_8UC1);
	for (int i = 0; i < finalHeight; i++) {
		for (int j = 0; j < finalWidth; j++) {
			dst.at<uchar>(i, j) = src.at<uchar>(i + minX, j + minY);
		}
	}
	return dst;
}

int main()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		system("cls");
		destroyAllWindows();
		//Get the image
		Mat resizedImage;
		Mat blurredImage;
		Mat src = imread(fname);


		//imshow("input image", src);
		//resizeImg(src, resizedImage, 1200, false);
		//GaussianBlur(resizedImage, blurredImage, Size(5, 5), 0);
		GaussianBlur(src, blurredImage, Size(5, 5), 0);


		//transform it into a black and white image
		Mat grayImage = color2Gray(blurredImage);
		//imshow("gray Image", grayImage);
		//extractTextTest(grayImage);
		
		//binarize the image
		Mat binImage = binarizare(grayImage);
		//imshow("Binary image", binImage);
		//extractTextTest(binImage);
		
		Mat intrestZone = getIntrestZone(grayImage, binImage);
		imshow("Interest zone", intrestZone);
		//extractTextTest(intrestZone);

		Mat SERIE_NR = getZone(intrestZone, 6.7, 5.11, 1.73, 1.24);
		imshow("SERIE_NR", SERIE_NR);
		extractTextTest(SERIE_NR);

		Mat CNP = getZone(intrestZone, 5.5, 3.9, 2.76, 1.5);
		imshow("CNP zone", CNP);
		printf("CNP: ");
		extractTextTest(CNP);

		Mat NUME = getZone(intrestZone, 3.74, 3.2, 3.23, 1.5);
		imshow("NUME", NUME);
		printf("Nume: ");
		extractTextTest(NUME);
		//getOutline(binImage);

		Mat PRENUME = getZone(intrestZone, 3, 2.58, 3.23, 1.5);
		imshow("PRENUME", PRENUME);
		printf("Prenume: ");
		extractTextTest(PRENUME);

		Mat CETATENIE = getZone(intrestZone, 2.47, 2.17, 3.23, 1.5);
		imshow("CETATENIE", CETATENIE);
		printf("Cetatenie: ");
		extractTextTest(CETATENIE);

		Mat LOC_NASTERE = getZone(intrestZone, 2.09, 1.88, 3.23, 1.5);
		imshow("LOC_NASTERE", LOC_NASTERE);
		printf("Loc Nastere: ");
		extractTextTest(LOC_NASTERE);

		waitKey();
	}
	return 0;
}