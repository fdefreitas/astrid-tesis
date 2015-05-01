#include <vtkSmartPointer.h>	
#include <vtkCleanPolyData.h>
#include <vtkDelaunay3D.h>
#include <vtkXMLDataSetWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vector>

#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSimplePointsReader.h>
#include <vtkDataSetSurfaceFilter.h>

#include <vtkIdList.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkDataSet.h>
#include <vtkSphereSource.h>
#include <vtkTriangleFilter.h>
#include <vtkExtractEdges.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCellArray.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPointSource.h>



using namespace std;

class dela
{
public:
	dela();
	~dela();
	int Tdelaunay ();
vtkSmartPointer<vtkIdList> GetConnectedVertices(vtkSmartPointer<vtkPolyData> mesh, int id);



};