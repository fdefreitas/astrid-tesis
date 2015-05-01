#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/property_map.h>
#include <CGAL/remove_outliers.h>
#include <CGAL/jet_smooth_point_set.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "../tesis/dela.h"
#include "../tesis/icp.h"
#include "../tesis/project_config.h"
#include "../tesis/reconstruction3D.h"
#include "../tesis/triangulacion.h"

using namespace cv;

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 PointCgal;

class Manager
{
public:
	Manager(void);
	~Manager(void);
	void scaner3D();

private:
	double degreeToRadian(double degree);
	void Puntos3DRotados(int vista);
	int StereoCalib(const char* imageList, int useUncalibrated);   
	int tomarImagenes(int tipo, char* archivo);
	void Construccion(int tFotos, int cant);


public:
	vector <vector<double>> puntos, puntosCara;

private:
	CvMat _M1, _M2, _P11, _P21, _F, *R, *T, *RwsTws, *MT, *MTA;
	CvMat *mx1, *my1, *mx2, *my2, *cXt;
	Triangulacion *triang;
	Icp *ICP;
	Reconstruccion3D *r3D;
	dela *Del;
};