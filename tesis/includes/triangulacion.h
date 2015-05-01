#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <stdio.h>

#include "../tesis/procesamientoImagenes.h"

using namespace std;

#ifndef PI
#define PI
static const double pi = 3.14159265358979323846;
#endif

inline static double square(int a)
{
	return a * a;
};


class Triangulacion
{
public:
	Triangulacion(CvMat P1, CvMat P2,CvMat *mx1, CvMat *my1, CvMat *mx2, CvMat *my2, CvMat F);
	~Triangulacion(void);
	void Puntos3D(IplImage* img, IplImage* img2, int vista, vector <vector<double>> &puntos, CvMat *Rws, CvMat *cXt);

private:
	int NubePuntos(IplImage* gray, IplImage* &circles, CvPoint2D32f *points, int vista);
	void Burbuja(float *error, int *eP, int tam);
	void eightPoints(float *e, CvPoint2D32f *p1, CvPoint2D32f *p2, CvMat *eP1, CvMat *eP2, int *eP, int np);
	int PuntosCorrespondientes(IplImage* img, IplImage* img2, IplImage* img8_1, IplImage* img28_1, 
								int* ePuntos, float* optical_flow_feature_error, int vista);
	void dibujarPuntos(CvMat* p2, int Nim, int npointsC,IplImage* img);
	

public:
	CvMat *p1, *_X, *p0;
	vector <vector<double>> p3D, p3DSR;
	IplImage *img1r, *img2r;

private:
	int numP1, numP2;
	CvPoint2D32f puntos1[400], puntos2[400], puntos2Corr[400];
	CvMat *p2;
	CvMat* _P1, *_P2, *mx1, *my1, *mx2, *my2, *F;
	ProcesamientoImagenes *procI;
	vector<vector<bool>> puntosTomados;


};
