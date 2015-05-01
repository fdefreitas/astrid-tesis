#include "../tesis/icp.h"
/**
	Toma el vector<vector<double>> p y el cvMat *x y los convierte 
	a ambos en polyData. Donde referencePolydata se utiliza como target
	y newPolydata se utiliza como source
*/
Icp::Icp(vector<vector<double>> p, CvMat *x) {
	referencePolydata = CreatePolyDataV(p);
	newPolydata = CreatePolyDataM(x);
}

Icp::~Icp(){

}

double Icp::degreeToRadian(double degree){
	return degree*pi/180;
}

void Icp::Puntos2Vistas(int vista, CvMat *_X, vector<vector<double> >p3D, IplImage* &firstI, IplImage* &secondI, vector <vector<double>> &p3DSR, CvMat *p1,CvMat *p0){

	printf("RO %f,%f,%f  \n",cvmGet(_X,1,0),cvmGet(_X,1,1),cvmGet(_X,1,2));
	int ind;
	vector<bool> tomados(p3D.size(),false);
	printf("p3D size %d\n",p3D.size());
	vector<double> point(7,0.0);
	double cp3x = 0, cp3y = 0, cp3z = 0, cXx = 0, cXy = 0, cXz = 0, min;
	CvScalar color1, color2;
	float primerX, primerY, segundoX, segundoY;
	int tamP3d = p3D.size();
	int tamX = _X->rows; 

	centroP3D = cvCreateMat(1,3,CV_64F);
	centroX = cvCreateMat(1,3,CV_64F);
	CvMat* pAux = cvCreateMat(3,1,CV_64F);
	nearestOrig.clear();
	nearestP.clear();

	printf("X %d %d p3d %d %d\n",_X->rows, p1->cols, p3D.size(), p0->cols);

	for (int i = 0; i < tamX; i++)
	{
		min = 1000000.0f;
		ind = -1;

		for (int j = 0; j < tamP3d; j++)
		{
			double x = pow(cvmGet(_X,i,0) - p3D[j][0],2); //p3dsr
			double y = pow(cvmGet(_X,i,1) - p3D[j][1],2);
			double z = pow(cvmGet(_X,i,2) - p3D[j][2],2);

	/*		primerX = cvmGet(p0,0,j) + 320;
			primerY = cvmGet(p0,1,j)*-1 + 240;
			
		// Para obtener la diferencia de color entre ambas vistas
			color1 = cvGet2D(firstI,primerY,primerX);
			color2 = cvGet2D(secondI,segundoY,segundoX);
			
			float color = sqrt(pow(color1.val[0] - color2.val[0],2) + pow(color1.val[1] - color2.val[1],2)
						 + pow(color1.val[2] - color2.val[2],2));			*/

			double de = sqrt(x + y + z);

		/*	if (i==0 && j < 3)
			{	printf("c %f %f %d\n",color1.val[0],color2.val[0],j); 
			//	printf("de %f \n",de);
			}
*/
			if(de < min && !tomados[j])
			{
				min = de;
				ind = j;	
			}
		/*	if (i==0)
			{
				cp3x += p3D[j][0];
				cp3y += p3D[j][1];
				cp3z += p3D[j][2];
			}*/
		}
		
		if (ind!=-1 && min < 0.5/*&& p3D[ind][2] > 0 && cvmGet(_X,i,2) > 0*/)
		{
			//printf("color1 %f color2 %f %f %f %f %f\n",color1.val[0],color2.val[0],primerX, primerY,cvmGet(p0,0,ind), cvmGet(p0,1,ind));
			printf("RO %f,%f,%f  SR%f,%f,%f %f ind %d\n",cvmGet(_X,i,0),cvmGet(_X,i,1),cvmGet(_X,i,2),p3D[ind][0],p3D[ind][1],p3D[ind][2],min,ind);
			point[0] = p3D[ind][0];
			point[1] = p3D[ind][1];
			point[2] = p3D[ind][2];
			point[3] = cvmGet(_X,i,0);
			point[4] = cvmGet(_X,i,1);
			point[5] = cvmGet(_X,i,2);
			point[6] = min;

			if (min < 0.5){
			float pX = cvmGet(p0,0,ind) + 320;
			float pY = cvmGet(p0,1,ind)*-1 + 240;
			cvCircle( firstI, cvPoint(pX,pY),3, CV_RGB(255,255,255), 1, 8, 0 );
		//	printf("el de arriba \n");
			//color1 = cvGet2D(firstI,pY,pX);
			
		//	printf("primerX %f y %f, %f y %f %d",pX,pY,p3D[ind][0],p3D[ind][1]);
			pX = cvmGet(p1,0,i) + 320;
			pY = cvmGet(p1,1,i)*-1 + 240;

			cvCircle( secondI, cvPoint(pX,pY),3, CV_RGB(255,255,255), 1, 8, 0 );

		//	printf("segundoX %f y %f %f y %f\n",pX,pY,cvmGet(_X,i,0),cvmGet(_X,i,1));
			
			//color2 = cvGet2D(secondI,pY,pX);
			//printf("color %f %f %d %d\n",color1.val[0],color2.val[0],ind,i);
			}

			nearestP.push_back(point);			
			tomados[ind] = true;

			cp3x += point[0];
			cp3y += point[1];
			cp3z += point[2];
			cXx += point[3];
			cXy += point[4];
			cXz += point[5];	
		}
	/*		cXx += cvmGet(_X,i,0);
			cXy += cvmGet(_X,i,1);
			cXz += cvmGet(_X,i,2);*/
	
	}
	
	int tamP = nearestP.size();
	cvmSet(centroP3D,0,0,cp3x/tamP);
	cvmSet(centroP3D,0,1,cp3y/tamP);
	cvmSet(centroP3D,0,2,cp3z/tamP);

	cvmSet(centroX,0,0,cXx/tamP);
	cvmSet(centroX,0,1,cXy/tamP);
	cvmSet(centroX,0,2,cXz/tamP);

	// obtener la X de la segunda imagen, para ser usada como primera en las prox 2 vistas
/*	p3DSR.clear();				
	vector<double> point2(3,0.0);	
	for (int i = 0; i < _X->rows; i++)       //nearestP
	{
		point2[0] = cvmGet(_X,i,0);
		point2[1] = cvmGet(_X,i,1);
		point2[2] = cvmGet(_X,i,2);
		p3DSR.push_back(point2);
	}*/

	printf("tama�o p3D guardado %d\n",p3D.size());
	sort(nearestP.begin(), nearestP.end(),Compare_t(nearestP));
	nearestOrig = nearestP;

	printf("\n");
}

void Icp::CalcularTransformacion(CvMat* &RwsTws, CvMat* &cXt)
{
	CvMat *M;
	for (int i = 0; i < nearestP.size(); i++)
	{
		nearestP[i][3] -= cvmGet(centroX,0,0);
		nearestP[i][4] -= cvmGet(centroX,0,1);
		nearestP[i][5] -= cvmGet(centroX,0,2);

		nearestP[i][0] -= cvmGet(centroP3D,0,0);
		nearestP[i][1] -= cvmGet(centroP3D,0,1);
		nearestP[i][2] -= cvmGet(centroP3D,0,2);
	}

	M = cvCreateMat(3,3,CV_64F);

	printf("PR %d\n",nearestP.size());
	double Sxx = 0, Sxy = 0, Sxz = 0, Syx = 0, Syy = 0, Syz = 0, Szx = 0, Szy = 0, Szz = 0;

	for (int i = 0; i < nearestP.size(); i++)
	{
		Sxx += (nearestP[i][0] * nearestP[i][3]);
		Sxy += (nearestP[i][0] * nearestP[i][4]);
		Sxz += (nearestP[i][0] * nearestP[i][5]);
		Syx += (nearestP[i][1] * nearestP[i][3]);
		Syy += (nearestP[i][1] * nearestP[i][4]);
		Syz += (nearestP[i][1] * nearestP[i][5]);
		Szx += (nearestP[i][2] * nearestP[i][3]);
		Szy += (nearestP[i][2] * nearestP[i][4]);
		Szz += (nearestP[i][2] * nearestP[i][5]);
	}

	cvmSet(M,0,0,Sxx);
	cvmSet(M,0,1,Sxy);
	cvmSet(M,0,2,Sxz);
	cvmSet(M,1,0,Syx);
	cvmSet(M,1,1,Syy);
	cvmSet(M,1,2,Syz);
	cvmSet(M,2,0,Szx);
	cvmSet(M,2,1,Szy);
	cvmSet(M,2,2,Szz);
		
	double VAV[3][3];
	CvMat* _DAV = cvCreateMat(3,3,CV_64F);
	cP3Dt = cvCreateMat(3,1,CV_64F);
	CvMat* _xt = cvCreateMat(3,1,CV_64F);
	CvMat* _UAV = cvCreateMat(3,3,CV_64F);
	CvMat* _UAVT = cvCreateMat(3,3,CV_64F);
	CvMat _VAV = cvMat(3,3,CV_64F,VAV);
	cvSetZero(&_VAV);
	cvSetZero(_UAV);
	cvSetZero(_UAVT);
	cvSetZero(_DAV);


	cvSVD(M,_DAV,_UAV,&_VAV);


	cvTranspose(_UAV,_UAVT);
	cvMatMulAdd(&_VAV,_UAVT,0,RwsTws);
	printf("----- GRADOS ---- %f\n",cvmGet(RwsTws,2,0));
	cvTranspose(centroP3D,cP3Dt);
	cvTranspose(centroX,cXt);
	cvMatMulAdd(RwsTws,cXt,0,cXt);
	cvSub(cP3Dt,cXt,cXt);

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			printf("Rws %d,%d %f",i,j,cvmGet(RwsTws,i,j));
		}

		for (int j = 0; j < 3; j++)
			printf("Tws %d %f",j,cvmGet(cXt,j,0));

}

vector<double> Icp::verificarDistancia(CvMat* R, CvMat* T)
{
	CvMat* pAux = cvCreateMat(3,1,CV_64F);
	double disAc = 0, disP = 0;
	vector<double> result(2,0.0);

	for (int i = 0; i < nearestOrig.size(); i++)
	{
		cvmSet(pAux,0,0,nearestOrig[i][3]);
		cvmSet(pAux,1,0,nearestOrig[i][4]);
		cvmSet(pAux,2,0,nearestOrig[i][5]);

//		cvMatMulAdd(R,pAux,T,pAux);

		double x = pow(cvmGet(pAux,0,0) - nearestOrig[i][0],2); 
		double y = pow(cvmGet(pAux,1,0) - nearestOrig[i][1],2);
		double z = pow(cvmGet(pAux,2,0) - nearestOrig[i][2],2);

		double dis = sqrt(x + y + z);
		disAc += dis;

		//	printf("%f,%f,%f SR %f,%f,%f %f,%f\n",cvmGet(pAux,0,0),cvmGet(pAux,1,0),cvmGet(pAux,2,0),nearestOriginal[i][0],nearestOriginal[i][1],nearestOriginal[i][2],nearestOriginal[i][6],dis);
	}

	/*if (disAc < disOpt)
	{
		disOpt = disAc;
		optima = c;
	}*/
	printf("distancia acumulada %f, %f %d\n",disAc/nearestOrig.size(),disAc,nearestOrig.size() );
	result[0] = disAc/nearestOrig.size();
	result[1] = disAc;
	return (result);
}

void Icp::iterativeCP(CvMat *&R, CvMat *&T, int tamx, int vista) {

	icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
	icp->SetSource(newPolydata);
	icp->SetTarget(referencePolydata);
	icp->GetLandmarkTransform()->SetModeToRigidBody();
	icp->SetMaximumNumberOfLandmarks(referencePolydata->GetNumberOfPoints());
	icp->SetMaximumNumberOfIterations(200);
	icp->SetCheckMeanDistance(1);
	icp->SetMaximumMeanDistance(0.000001);
	icp->StartByMatchingCentroidsOn();
	icp->Modified();
	icp->Update();

	int nint=icp->GetNumberOfIterations();
	double distancia = icp->GetMeanDistance();
	vtkSmartPointer<vtkMatrix4x4> M = icp->GetMatrix();
	
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			cvmSet(R,i,j,M->GetElement(i,j));
		}
	}

}

/**
This function creates a set of 4 points (the origin and a point unit distance along each axis)
@param {vector<vector>}
*/
vtkSmartPointer<vtkPolyData> Icp::CreatePolyDataV(vector <vector<double>> p){

  	vtkSmartPointer<vtkPoints> sourcePoints = vtkSmartPointer<vtkPoints>::New();

	for (int i = 0; i < p.size(); i++) {
		double dot[3] = {p[i][0], p[i][1],p[i][2]};
		sourcePoints->InsertNextPoint(dot);
	}
 
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->SetPoints(sourcePoints);
	
	vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
	vertexFilter->SetInputConnection(polydata->GetProducerPort());
	vertexFilter->Update();
	
	return vertexFilter->GetOutput();
}

/**
	Takes a cvMat *M, and turns it into a vtkPolyData
*/
vtkSmartPointer<vtkPolyData> Icp::CreatePolyDataM(CvMat *M){
 
	vtkSmartPointer<vtkPoints> sourcePoints = vtkSmartPointer<vtkPoints>::New();

	for (int i = 0; i < M->rows; i++) {
		double dot[3] = {cvmGet(M,i,0), cvmGet(M,i,1),cvmGet(M,i,2)};
		sourcePoints->InsertNextPoint(dot);
	}

	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->SetPoints(sourcePoints);
	
	vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
	vertexFilter->SetInputConnection(polydata->GetProducerPort());
	vertexFilter->Update();
	
	return vertexFilter->GetOutput();
}

double* Icp::setCentroM(CvMat *M) {
	double center[3] = {0,0,0};

	for (int i = 0; i < M->rows; i++) {
		center[0] += cvmGet(M,i,0);
		center[1] += cvmGet(M,i,1);
		center[2] += cvmGet(M,i,2);			
	}

	center[0] = (double)center[0]/M->rows;
	center[1] = (double)center[1]/M->rows;
	center[2] = (double)center[2]/M->rows;

	return center;
}

double* Icp::setCentroV(vector <vector<double>> p){
	double center[3] = {0,0,0};

	for (int i = 0; i < p.size(); i++){
		center[0] += p[i][0];
		center[1] += p[i][1];
		center[2] += p[i][2];			
	}

	center[0] = (double)center[0]/p.size();
	center[1] = (double)center[1]/p.size();
	center[2] = (double)center[2]/p.size();

	return center;
}
