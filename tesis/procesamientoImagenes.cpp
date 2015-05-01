#include "../tesis/procesamientoImagenes.h"

ProcesamientoImagenes::ProcesamientoImagenes()
{

}

ProcesamientoImagenes::~ProcesamientoImagenes()
{

}

void ProcesamientoImagenes::SumC(int i, vector<vector<int>> v, IplImage* &imagen, IplImage* original)
{
	int w = original->width;
	int h = original->height;

	int If = 0, Jf = 0, tam = 0, maxtam = w - 1, maxx = i + 3, P = 0;

	for(tam = 1; tam <maxtam; tam++)
	{
		for (int x = i; x < maxx; x++)
		{
			for (int y = tam-1; y < tam -1 + 3; y++)
			{
				CvScalar s;
				s = cvGet2D(original,x,y);
				s.val[0] *= v[Jf][If];
				P += s.val[0];
		
				Jf++;
			}			
			If++;
			Jf = 0;	
		}		

		if (P<0) P = 0;
		else if(P>255) P = 255;

		CvScalar s2;
		s2.val[0]= P;
		cvSet2D(imagen,i+1,tam,s2);

		If = 0;
		P = 0; 
	}
}

void ProcesamientoImagenes::SumaImagenes(IplImage* uno, IplImage* dos, IplImage* &tres)
{	
	for (int i = 0; i < uno->width; i++)
	{
		for (int j = 0; j < uno->height; j++)
		{
			CvScalar s;
			s = cvGet2D(uno,j,i);

			CvScalar s2;
			s2 = cvGet2D(dos,j,i);
			
			s.val[0] += s2.val[0];
			cvSet2D(tres,j,i,s);			
		}
	}
}

double ProcesamientoImagenes::ErrorCirculo(int i, int j, IplImage* im, vector<vector<int>> k)
{
	double error = 0.0;
	int Ik = 0, Jk = 0;


	for (int x = i; x < i + 3; x++)
	{
		for (int y = j; y < j + 3; y++)
		{
			if (!(x < im->height - 3 && y < im->width - 3)) continue;

			CvScalar s;
			s = cvGet2D(im,i,j);
			int t= k[Ik][Jk];
			double pot = pow((double)abs(s.val[0]-t),2);
			error += pot;
			Jk++;
		}
		Ik++;
		Jk = 0;
	}

	return(sqrt(error/(3*(3-1))));
}

int ProcesamientoImagenes::ContarCirculos(IplImage* im, IplImage* &circ, CvPoint2D32f *points)
{
	int c = 0;

	vector<vector<int>> k(3);
	
	for (int i=0; i< 3; i++)
		k[i].resize(3,255);	

	for (int i=0; i< im->height - 3; i=i+4)
	{
		for (int j=0; j< im->width - 3; j=j+4)
		{
			if (ErrorCirculo(i,j,im,k) < 50)
			{
				printf("EEROR menor\n");
				cvCircle( circ, cvPoint(j,i),2, CV_RGB(255,255,255), 1, 8, 0 );
				points[c].x = j;
				points[c].y = i;
				i = i + 3;
				j = j + 3;			
				c++;
			}
		}
	}
	
	printf("c %d\n",c);
	return c;
}

void ProcesamientoImagenes::Prewitt(IplImage* img, IplImage* &imOut)
{
	// Obtener el contorno con el operador de Prewitt 
	vector <vector<int>> v(3);
	int max = img->height - v.size() +1;

	for (int i = 0; i < 3; i++)
		v[i].resize(3,0);

	v[0][0] = v[1][0] = v[2][0] = -1;
	v[0][2] = v[1][2] = v[2][2] = 1;

	IplImage* imgX = cvCreateImage (cvSize (img->width, img->height), 8, 1);

	for(int i=0; i< max; i++)
		SumC(i,v,imgX,img);

	vector <vector<int>> v2(3);
	for (int i = 0; i < 3; i++)
		v2[i].resize(3,0);

	v2[0][0] = v2[0][1] = v2[0][2] = -1;
	v2[2][0] = v2[2][1] = v2[2][2] = 1;

	IplImage* imgY = cvCreateImage (cvSize (img->width, img->height), 8, 1);

	for(int i=0; i< max; i++)
		SumC(i,v2,imgY,img);

	SumaImagenes(imgX,imgY,imOut);
}