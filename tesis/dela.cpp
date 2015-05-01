#include "../tesis/dela.h"

dela::dela()
{

}

dela::~dela()
{

}

int dela::Tdelaunay ()
{

  vtkSmartPointer<vtkSimplePointsReader> reader = vtkSmartPointer<vtkSimplePointsReader>::New();
  reader->SetFileName("puntos.off");
  reader->Update();


  // Clean the polydata. This will remove duplicate points that may be
  // present in the input data.
  vtkSmartPointer<vtkCleanPolyData> cleaner =
    vtkSmartPointer<vtkCleanPolyData>::New();
  cleaner->SetInputConnection (reader->GetOutputPort());


  // Generate a tetrahedral mesh from the input points. By
  // default, the generated volume is the convex hull of the points.
  vtkSmartPointer<vtkDelaunay3D> delaunay3D =
    vtkSmartPointer<vtkDelaunay3D>::New();
  delaunay3D->SetInputConnection (reader->GetOutputPort());
 delaunay3D->SetAlpha(1.0);



  // Write the mesh as an unstructured grid
 vtkSmartPointer<vtkXMLDataSetWriter> writer =
    vtkSmartPointer<vtkXMLDataSetWriter>::New();
  writer->SetFileName ("delaunay.vtu");
  writer->SetInputConnection ( delaunay3D->GetOutputPort() );
  writer->Write();


  vtkSmartPointer<vtkUnstructuredGrid> o = delaunay3D->GetOutput();


  vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter = 
      vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
  surfaceFilter->SetInput(o);
  surfaceFilter->Update(); 
 
  vtkPolyData* polydata = surfaceFilter->GetOutput();
  polydata->GetPolys()->InitTraversal();

  vtkIdType npts, *pts;
  
	std::ofstream out("delaunay.off");        // MOVER DE AQUI
	
	out << "OFF\n"; 
	out << polydata->GetNumberOfPoints() << " " << polydata->GetNumberOfPolys() << " " << "0" << endl;

	for (vtkIdType i = 0; i < polydata->GetNumberOfPoints(); i++)
	{
		double *x = polydata->GetPoint(i);
		out << x[0] << " " << x[1] << " " << x[2] << endl;
	}


	while(polydata->GetPolys()->GetNextCell(npts,pts))
		out << "3" << "  " <<pts[0] << " " << pts[1] << " " << pts[2] << endl;



		
	out.close();


 



 
  return EXIT_SUCCESS;
}
