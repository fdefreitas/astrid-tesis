#ifndef PROCESAMIENTOIMAGENES_H
#define PROCESAMIENTOIMAGENES_H

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>
#include <fstream>
#include <algorithm>

using namespace std;

class ProcesamientoImagenes
{
public:
	ProcesamientoImagenes(void);
	~ProcesamientoImagenes(void);
	void SumC(int i, vector<vector<int>> v, IplImage* &imagen, IplImage* original);
	void SumaImagenes(IplImage* uno, IplImage* dos, IplImage* &tres);
	int ContarCirculos(IplImage* im, IplImage* &circ, CvPoint2D32f *points);
	void Prewitt(IplImage* img, IplImage* &imOut);

private:
	double ErrorCirculo(int i, int j, IplImage* im, vector<vector<int>> k);

};
#endif