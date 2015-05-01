#include "../tesis/includes/triangulacion.h"

Triangulacion::Triangulacion(CvMat P1, CvMat P2,CvMat *mx1, CvMat *my1, CvMat *mx2, CvMat *my2, CvMat _F)
{
	procI = new ProcesamientoImagenes();
	this->mx1 = mx1;
	this->my1 = my1;
	this->mx2 = mx2;
	this->my2 = my2;

	_P1 = cvCreateMat(3,4,CV_64F);
	_P2 = cvCreateMat(3,4,CV_64F);
	F = cvCreateMat(3,3,CV_64F);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cvmSet(_P1,i,j,cvmGet(&P1,i,j));
			cvmSet(_P2,i,j,cvmGet(&P2,i,j));
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			cvmSet(F,i,j,cvmGet(&_F,i,j));	
	}

}

Triangulacion::~Triangulacion()
{

}

int Triangulacion::NubePuntos(IplImage* gray, IplImage* &circles, CvPoint2D32f *points, int vista)
{
	// Obtener la nube de puntos
	IplImage* edge = cvCreateImage (cvSize (gray->width, gray->height), 32, 1);
	//cvCornerHarris(gray,edge,3,5); //2,9

	/*cvNamedWindow("edge");
	cvShowImage("edge", edge);*/

	// Convertir imagen 32-bit 1 canal a 8-bit 1 canal
	IplImage* edge8_3 = cvCreateImage (cvSize (gray->width, gray->height), 8, 3);
	cvConvertImage(edge,edge8_3);
	IplImage* edge8_1 = cvCreateImage (cvSize (gray->width, gray->height), 8, 1);
	cvCvtColor(edge8_3, edge8_1, CV_RGB2GRAY);

	// Encontrar circulos
	IplImage* eig_image = cvCreateImage (cvSize (gray->width, gray->height), IPL_DEPTH_32F, 1);
	IplImage* temp_image = cvCreateImage (cvSize (gray->width, gray->height), IPL_DEPTH_32F, 1);

	int Npuntos = 400;
	//cvThreshold(gray,gray,80,200,CV_THRESH_BINARY);

	cvGoodFeaturesToTrack(gray, eig_image, temp_image, points, &Npuntos, 0.1, 5,0,3,0);

	//int Npuntos = procI->ContarCirculos(edge8_1,circles, points);

	return Npuntos;
}

void Triangulacion::Burbuja(float *error, int *eP, int tam)
{
	float temp;
	int temp2; 

	for (int i = 1; i < tam; i++ )
	{
       for (int j = 0; j < tam - i ; j++)
		{
			if (error[j]> error[j + 1])
			{	
				temp = error[j];
	            error[j] = error[j + 1];
                error[j + 1] = temp;

				temp2 = eP[j];
	            eP[j] = eP[j + 1];
                eP[j + 1] = temp2;
			}
		}
	}
}

void Triangulacion::eightPoints(float *e, CvPoint2D32f *p1, CvPoint2D32f *p2, CvMat *eP1, CvMat *eP2, int *eP, int np)
{
	int j = 0, i = 0;
	double temp,cx,cy;

	cx = 320.0f; 
	cy = 240.0f; 

	puntosTomados.clear();
	puntosTomados.resize(640);
	for (int i = 0; i < 640; i++)
		puntosTomados[i].resize(480);

	while (j< np)
	{
		int puntoY = (int)floor(p2[eP[i]].y); 
		int puntoX = (int)floor(p2[eP[i]].x);

		if (e[i] > 0 && e[i] < 400 && abs(p1[eP[i]].y - p2[eP[i]].y)< 1 && p2[eP[i]].y < 400 && p2[eP[i]].y > 100 && p2[eP[i]].x > 10 && p2[eP[i]].x < 630 && !puntosTomados[puntoX][puntoY])  // mientras para quitar los puntos de esquinas
		{
			//temp = float((p1[eP[i]].x/1));
			temp = float((p1[eP[i]].x - cx));
			cvSetReal2D(eP1,0,j,temp); 
			//temp = float((p1[eP[i]].y/1));
			temp = float((p1[eP[i]].y - cy));
			//cvSetReal2D(eP1,1,j,temp); 
			cvSetReal2D(eP1,1,j,temp*-1); 
		
			temp = float((p2[eP[i]].x - cx));
			//temp = float((p2[eP[i]].x/1));
			cvSetReal2D(eP2,0,j,temp); 
			temp = float((p2[eP[i]].y - cy));
			//temp = float((p2[eP[i]].y/1));
			cvSetReal2D(eP2,1,j,temp*-1); 
			//cvSetReal2D(eP2,1,j,temp);
			j++;
			puntosTomados[puntoX][puntoY] = true;
			if (puntoX > 0)
				puntosTomados[puntoX-1][puntoY] = true;
			else if (puntoX < 639)
				puntosTomados[puntoX+1][puntoY] = true;

		//	printf("%d %d %d %d %d\n",(int)floor(p2[eP[i]].x),(int)floor(p2[eP[i]].y),j,np,i);

			cvCircle( img1r, cvPoint(p1[eP[i]].x,p1[eP[i]].y),2, CV_RGB(255,255,255), 1, 8, 0 );
			cvCircle( img2r, cvPoint(p2[eP[i]].x,p2[eP[i]].y),2, CV_RGB(255,255,255), 1, 8, 0 );
		}
		i++;	
	}

}

int Triangulacion::PuntosCorrespondientes(IplImage* img, IplImage* img2, IplImage* img8_1, IplImage* img28_1, 
	int* ePuntos, float* optical_flow_feature_error, int vista) {
			
	IplImage* circles = cvCreateImage (cvSize (img->width, img->height), 8, 1);
	IplImage* gray = cvCreateImage (cvSize (img->width, img->height), 8, 1);
	IplImage* gray2, *circles2;

	puntosTomados.resize(640);
	
	for (int i = 0; i < 640; i++) {
		puntosTomados[i].resize(480,false);	
	}

	cvCvtColor(img, img8_1, CV_RGB2GRAY);
	cvRemap( img8_1, img1r, mx1, my1 );               //HACER EL REMAP
	cvSmooth(img1r,gray,CV_GAUSSIAN,3,3);
	cvCanny( gray, gray, 80, 200, 3 );

	// IMAGEN IZQUIERDA
	// Obtener la nube de puntos
	numP1 = NubePuntos(gray, circles, puntos1, vista);
		
	// IMAGEN DERECHA
	cvCvtColor(img2, img28_1, CV_RGB2GRAY);
	cvRemap( img28_1, img2r, mx2, my2 );

	gray2 = cvCreateImage (cvSize (img2->width, img2->height), 8, 1);
	cvSmooth(img2r,gray2,CV_GAUSSIAN,3,3);
	cvCanny( gray2, gray2, 80, 200, 3 );

	// Obtener la nube de puntos de imagen izquierda
	circles2 = cvCreateImage (cvSize (img2->width, img2->height), 8, 1);
	numP2 = NubePuntos(gray2, circles2, puntos2, vista);

	IplImage* pyramid1 = cvCreateImage (cvSize (img->width, img->height), 8, 1);
	IplImage* pyramid2 = cvCreateImage (cvSize (img->width, img->height), 8, 1);

	CvSize optical_flow_window = cvSize(9,9);
	int number = numP1;
	char optical_flow_found_feature[400];

	CvTermCriteria optical_flow_termination_criteria 
		= cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.1 );

	cvCalcOpticalFlowPyrLK(img1r, img2r, pyramid1, pyramid2, puntos1, puntos2Corr, number, optical_flow_window,
		5, optical_flow_found_feature, optical_flow_feature_error, optical_flow_termination_criteria, 0);
	int npointsC = 0, col = 255;

	for(int i = 0; i < number; i++) {

		if ( optical_flow_found_feature[i] == 0 )	continue;
		
		int line_thickness;				
		line_thickness = 1;	
		CvScalar line_color;			
		line_color = CV_RGB(255,0,0);

		CvPoint p,q;
		p.x = (int) puntos1[i].x;			
		p.y = (int) puntos1[i].y;
		q.x = (int) puntos2Corr[i].x;
		q.y = (int) puntos2Corr[i].y;

		double angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
		double hypotenuse = sqrt( square(p.y - q.y) + square(p.x - q.x) );
		
		if (optical_flow_feature_error[i]<100) {
			q.x = (int) (p.x - 3 * hypotenuse * cos(angle));
			q.y = (int) (p.y - 3 * hypotenuse * sin(angle));
	
			cvLine( circles, p, q, line_color, line_thickness, CV_AA, 0 );
			cvLine( circles2, q, p, line_color, line_thickness, CV_AA, 0 );
				
			p.x = (int) (q.x + 9 * cos(angle + pi / 4));
			p.y = (int) (q.y + 9 * sin(angle + pi / 4));
			cvLine( circles, p, q, line_color, line_thickness, CV_AA, 0 );
			p.x = (int) (q.x + 9 * cos(angle - pi / 4));
			p.y = (int) (q.y + 9 * sin(angle - pi / 4));
			cvLine( circles, q, p, line_color, line_thickness, CV_AA, 0 );
		}

		int puntoY = (int)floor(puntos2Corr[i].y); 
		int puntoX = (int)floor(puntos2Corr[i].x);
	

		if (abs(puntos1[i].y - puntos2Corr[i].y)< 1 
				&& puntos2Corr[i].y < 400 
				&& puntos2Corr[i].y > 100 
				&& puntos2Corr[i].x > 10 
				&& puntos2Corr[i].x < 630 
				&& optical_flow_feature_error[i] < 400 
				&& !puntosTomados[puntoX][puntoY]) {
			npointsC++;

			puntosTomados[puntoX][puntoY] = true;
			if (puntoX > 0)
				puntosTomados[puntoX-1][puntoY] = true;
			else if (puntoX < 639)
				puntosTomados[puntoX+1][puntoY] = true;
		}
	}

	for (int i = 0; i < 400; i++) {
		ePuntos[i] = i;	
	}

	return npointsC;
}


void Triangulacion::Puntos3D(IplImage* img, IplImage* img2, int vista, vector <vector<double>> &puntos, CvMat *Rws, CvMat *cXt)
{
	IplImage* img8_1;
	img8_1 = cvCreateImage (cvSize (img->width, img->height), 8, 1);

	IplImage* img28_1;
	img28_1 = cvCreateImage (cvSize (img->width, img->height), 8, 1);

	float optical_flow_feature_error[400];
	int ePuntos[400];

	int npointsC = PuntosCorrespondientes(img, img2, img8_1, img28_1, 
	ePuntos, optical_flow_feature_error,vista);

	p1 = cvCreateMat(2,npointsC,CV_32F);
	p2 = cvCreateMat(2,npointsC,CV_32F);		

	eightPoints(optical_flow_feature_error, puntos1, puntos2Corr, p1, p2, ePuntos, npointsC);

	int tamX = p1->cols;
	double escalar[4];
	CvMat *_DA = cvCreateMat(4,4,CV_64F);
	CvMat *_UA = cvCreateMat(4,4,CV_64F);
	CvMat *_VA = cvCreateMat(4,4,CV_64F);
	CvMat *_A = cvCreateMat(4,4,CV_64F);
	_X = cvCreateMat(tamX,3,CV_64F);
	vector<double> point(3,0.0);
	CvMat* pAux = cvCreateMat(3,1,CV_64F);
	cvmSetZero(_A);

	double temp, sumx = 0, sumy = 0, sumz = 0;
	for (int k = 0; k < tamX ; k++)
	{
		escalar[0] = cvmGet(p1,0,k);
		escalar[1] = cvmGet(p1,1,k);
		escalar[2] = cvmGet(p2,0,k);
		escalar[3] = cvmGet(p2,1,k);


		temp = escalar[0]*cvmGet(_P1,2,0) - cvmGet(_P1,0,0);
		cvmSet(_A,0,0,temp);
		temp = escalar[0]*cvmGet(_P1,2,1) - cvmGet(_P1,0,1);
		cvmSet(_A,0,1,temp);
		temp = escalar[0]*cvmGet(_P1,2,2) - cvmGet(_P1,0,2);
		cvmSet(_A,0,2,temp);
		temp = escalar[0]*cvmGet(_P1,2,3) - cvmGet(_P1,0,3);
		cvmSet(_A,0,3,temp);

		temp = escalar[1]*cvmGet(_P1,2,0) - cvmGet(_P1,1,0);
		cvmSet(_A,1,0,temp);
		temp = escalar[1]*cvmGet(_P1,2,1) - cvmGet(_P1,1,1);
		cvmSet(_A,1,1,temp);
		temp = escalar[1]*cvmGet(_P1,2,2) - cvmGet(_P1,1,2);
		cvmSet(_A,1,2,temp);
		temp = escalar[1]*cvmGet(_P1,2,3) - cvmGet(_P1,1,3);
		cvmSet(_A,1,3,temp);

		temp = escalar[2]*cvmGet(_P2,2,0) - cvmGet(_P2,0,0);
		cvmSet(_A,2,0,temp);
		temp = escalar[2]*cvmGet(_P2,2,1) - cvmGet(_P2,0,1);
		cvmSet(_A,2,1,temp);
		temp = escalar[2]*cvmGet(_P2,2,2) - cvmGet(_P2,0,2);
		cvmSet(_A,2,2,temp);
		temp = escalar[2]*cvmGet(_P2,2,3) - cvmGet(_P2,0,3);
		cvmSet(_A,2,3,temp);
		
		temp = escalar[3]*cvmGet(_P2,2,0) - cvmGet(_P2,1,0);
		cvmSet(_A,3,0,temp);
		temp = escalar[3]*cvmGet(_P2,2,1) - cvmGet(_P2,1,1);
		cvmSet(_A,3,1,temp);
		temp = escalar[3]*cvmGet(_P2,2,2) - cvmGet(_P2,1,2);
		cvmSet(_A,3,2,temp);
		temp = escalar[3]*cvmGet(_P2,2,3) - cvmGet(_P2,1,3);
		cvmSet(_A,3,3,temp);
		
		cvSVD(_A,_DA,_UA,_VA);
		

		if (vista==0)
		{	
		//	printf("Xp3dA %f \n",cvmGet(_VA,0,2));
			point[0] = (cvmGet(_VA,0,3))/(cvmGet(_VA,3,3));
			point[1] = (cvmGet(_VA,1,3))/(cvmGet(_VA,3,3));
			point[2] = cvmGet(_VA,2,3)/(cvmGet(_VA,3,3));


			p3D.push_back(point);
			p3DSR.push_back(point);
			p0 = cvCreateMat(2,npointsC,CV_64F);
			p0 = p1;				// Puntos imagen izquiera primera vista
										// Para obtener los puntos correspondientes de angulos consecutivos
			puntos.push_back(point);
		//	printf("Xp3d %d %f %f %f %f %f\n",k,point[0],point[1],point[2],escalar[0]+320,escalar[1]+240);
			sumx += point[0];
			sumy += point[1];
			sumz += point[2];
		//	printf("VA %f %f\n",cvmGet(_VA,3,2),cvmGet(_VA,3,3));
						
		}
		else
		{
			//printf("mas aca %f %f %f\n",cvmGet(&_VA,0,2),cvmGet(&_VA,1,2),cvmGet(&_VA,2,2));
			cvmSet(_X,k,0,(cvmGet(_VA,0,3))/cvmGet(_VA,3,3));
			cvmSet(_X,k,1,(cvmGet(_VA,1,3))/cvmGet(_VA,3,3));
			cvmSet(_X,k,2,cvmGet(_VA,2,3)/cvmGet(_VA,3,3));

		//	printf("X %d %f %f %f \n",k,cvmGet(_X,k,0),cvmGet(_X,k,1),cvmGet(_X,k,2));
			sumx += cvmGet(_X,k,0);
			sumy += cvmGet(_X,k,1);
			sumz += cvmGet(_X,k,2);

		}

	}
}
