#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h> //to set type to ridgid body
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkXMLPolyDataWriter.h>
#include "../tesis/procesamientoImagenes.h"

using namespace std;


#ifndef PI
#define PI
static const double pi = 3.14159265358979323846;
#endif

struct Compare_t
  {
  // Our functor takes a reference to the table and a column to use when comparing rows
  vector <vector <double> > & matrix;

  Compare_t(
    vector <vector <double> > & matrix
    ):
    matrix( matrix )
    { }

  // This is the comparison function: taking two rows and comparing them
  // by the (previously) specified column
  bool operator () ( const vector <double> & x, const vector <double> & y ) const
    {
    return x[6] < y[6];
    }
  };

class Icp
{
public:
	Icp(vector<vector<double>> p, CvMat *x);
	~Icp(void);
	void iterativeCP(CvMat *&R, CvMat *&T, int tamx, int vista);
	void Puntos2Vistas(int vista, CvMat *_X, vector<vector<double> >p3D, IplImage* &firstI, IplImage* &secondI, vector <vector<double>> &p3DSR, CvMat *p1,CvMat *p0);
	vector<double> verificarDistancia(CvMat* R, CvMat* T);
	void CalcularTransformacion(CvMat* &RwsTws, CvMat* &cXt);

private:
	double degreeToRadian(double degree);
	vtkSmartPointer<vtkPolyData> CreatePolyDataM(CvMat *M);
	vtkSmartPointer<vtkPolyData> CreatePolyDataV(vector <vector<double>> p);

private: 
	CvMat *centroP3D, *centroX,*cP3Dt;
	vector <vector<double>> nearestP, nearestOrig;
	vtkSmartPointer<vtkPolyData> referencePolydata, newPolydata, polyData2, p2Polydata;
	vtkSmartPointer<vtkIterativeClosestPointTransform> icp;
	double* setCentroM(CvMat *M);
	double* setCentroV(vector <vector<double>> p);
};