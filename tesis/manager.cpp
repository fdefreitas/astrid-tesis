#include "../tesis/manager.h"

Manager::Manager()
{

}

Manager::~Manager()
{

}

// tipo 1 -> fotos tablero
// tipo 2 -> fotos objeto

int Manager::tomarImagenes(int tipo, char* archivo) {
	int tablero = 1;
	int objeto = 2;
	IplImage* img = 0;
	IplImage* img2 = 0;
	int nFoto = 0;
	char respuesta[5];
	respuesta[0]='S';
	std::ofstream out;
	int nx = 0, ny = 0;

	if (tipo==1) {
		out.open(archivo);
		printf("Introduzca la cantidad de cuadrados horizontales que tiene su tablero\n");
		scanf("%d",&nx);
		
		while(nx <= 0) {
			printf("Debe ingresar un n\243mero positivo mayor que cero\n");
			scanf("%d",&nx);
		}

		printf("Introduzca la cantidad de cuadrados verticales que tiene su tablero\n");
		scanf("%d",&ny);
		while(ny <= 0) {
			printf("Debe ingresar un n\243mero positivo mayor que cero\n");
			scanf("%d",&ny);
		}

		out << (ny-1) << " " << (nx-1) << "\n";
	}

	printf("\nPresione Enter cada vez que desee capturar una imagen\n");

	CvCapture* capture = cvCaptureFromCAM(1);    //Derecha
	CvCapture* capture2 = cvCaptureFromCAM(0);   // Izquierda

	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 640 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 480 );

	cvSetCaptureProperty( capture2, CV_CAP_PROP_FRAME_WIDTH, 640 );
	cvSetCaptureProperty( capture2, CV_CAP_PROP_FRAME_HEIGHT, 480 );

	while(respuesta[0]=='S' || respuesta[0]=='s') {
		int key = -1;

		while(key!=13) {              // capture a frame
			if(!cvGrabFrame(capture)) {              // capture a frame
				printf("No se pudo grabar la imagen\n\7");
				exit(0);
			}

			img = cvRetrieveFrame(capture);

			if(!cvGrabFrame(capture2)){              // capture a frame
				printf("No se pudo grabar la imagen\n\7");
				exit(0);
			}

			img2 = cvRetrieveFrame(capture2);

			cvNamedWindow("Derecha");;
			cvShowImage("Derecha", img);
			cvNamedWindow("Izquierda");;
			cvShowImage("Izquierda", img2);

			key = cvWaitKey(1);
		}
		
		char name[10], name2[10];
		char num[5];
		itoa(nFoto,num,10);

		if (tipo == objeto)
			strcpy (name2,"i");
		else
			strcpy (name2,"izq");

		strcat (name2,num);
		strcat (name2,".JPG");
		cvSaveImage(name2,img2);

		if (tipo == tablero) out << name2 << "\n";

		if (tipo == objeto)
			strcpy (name,"d");
		else
			strcpy(name,"der");

		strcat (name,num);
		strcat (name,".JPG");
		cvSaveImage(name,img);

		if (tipo == tablero ) out << name << "\n";

		cvDestroyWindow("Izquierda");
		cvDestroyWindow("Derecha");

		cvWaitKey(2);

		printf(" %cDesea tomar otra foto? (S/N)\n",168);
		scanf("%s",respuesta);

		nFoto++;
	}

	cvReleaseCapture(&capture);
	cvReleaseCapture(&capture2);

	if (tipo == tablero) out.close();

	return nFoto;
}

void Manager::Puntos3DRotados(int vista) {
	
	int tam = triang->_X->rows;
	CvMat* pAux = cvCreateMat(4, 1, CV_64F);
	cvmSet(pAux,3,0,1);
	vector<double> point(3, 0.0); // 3 posiciones en '0.0'

	// Crea un Mat nuevo de tamaño 2xcant de columnas en triang->p1 (un malloc)
	triang->p0 = cvCreateMat(2, triang->p1->cols, CV_64F);
	
	/* Copia triang->p1 a triang->p0, ya que el resultado de la iteracion anterior
	 es el acumulado para esta iteracion 
	 */
	triang->p0 = triang->p1;
	
	//Limpia triang->p3D
	triang->p3D.clear();
	
	// triang->_X es un cvMat
	// pAux es un vector de 4x1

	for (int i = 0; i < tam; i++) {
		cvmSet(pAux,0,0,cvmGet(triang->_X,i,0));
		cvmSet(pAux,1,0,cvmGet(triang->_X,i,1));
		cvmSet(pAux,2,0,cvmGet(triang->_X,i,2));

		cvMatMulAdd(MT, pAux, 0, pAux);

		point[0] = cvmGet(pAux,0,0);
		point[1] = cvmGet(pAux,1,0);
		point[2] = cvmGet(pAux,2,0);

		//printf(" %f %f %f \n",point[0],point[1],point[2]);

		cvmSet(triang->_X,i,0,point[0]);
		cvmSet(triang->_X,i,1,point[1]);
		cvmSet(triang->_X,i,2,point[2]);

		puntos.push_back(point);
		// guardar los puntos de la segunda imagen, que se usara como primera en la prox comparacion
		triang->p3D.push_back(point);
	}
}

int Manager::StereoCalib(const char* imageList, int useUncalibrated)
{
    int nx = 0, ny = 0;
    int displayCorners = 0;
    int showUndistorted = 1;
    bool isVerticalStereo = false;//OpenCV can handle left-right
                                      //or up-down camera arrangements
    const int maxScale = 1;
    const float squareSize = 1.f; //Set this to your actual square size
    FILE* f = fopen(imageList, "rt");
    int i, j, lr, nframes, n, N = 0;
    vector<string> imageNames[2];
    vector<CvPoint3D32f> objectPoints;
    vector<CvPoint2D32f> points[2];
    vector<int> npoints;
    vector<uchar> active[2];
    vector<CvPoint2D32f> temp;
    CvSize imageSize = {0,0};
    // ARRAY AND VECTOR STORAGE:
    double M1[3][3], M2[3][3], D1[5], D2[5];
    double R[3][3], T[3], E[3][3], F[3][3];
    _M1 = cvMat(3, 3, CV_64F, M1 );
    _M2 = cvMat(3, 3, CV_64F, M2 );
    CvMat _D1 = cvMat(1, 5, CV_64F, D1 );
    CvMat _D2 = cvMat(1, 5, CV_64F, D2 );
    CvMat _R = cvMat(3, 3, CV_64F, R );
    CvMat _T = cvMat(3, 1, CV_64F, T );
    CvMat _E = cvMat(3, 3, CV_64F, E );
    _F = cvMat(3, 3, CV_64F, F );
    char buf[1024];

    /*if( displayCorners )
        cvNamedWindow( "Esquinas", 1 );*/
// READ IN THE LIST OF CHESSBOARDS:
    if( !f )
    {
        fprintf(stderr, "No se pudo abrir el archivo %s\n", imageList );
        return -1;
    }

    if( !fgets(buf, sizeof(buf)-3, f) || sscanf(buf, "%d%d", &nx, &ny) != 2 )
        return -1;
    n = nx*ny;
    temp.resize(n);

    for(i=0;;i++)
    {
        int count = 0, result=0;
        lr = i % 2;
        vector<CvPoint2D32f>& pts = points[lr];
        if( !fgets( buf, sizeof(buf)-3, f ))
            break;
        size_t len = strlen(buf);
        while( len > 0 && isspace(buf[len-1]))
            buf[--len] = '\0';
        if( buf[0] == '#')
            continue;
        IplImage* img = cvLoadImage( buf, 0 );
        if( !img )
            break;
        imageSize = cvGetSize(img);
        imageNames[lr].push_back(buf);
    //FIND CHESSBOARDS AND CORNERS THEREIN:
        for( int s = 1; s <= maxScale; s++ )
        {
            IplImage* timg = img;
            if( s > 1 )
            {
                timg = cvCreateImage(cvSize(img->width*s,img->height*s),
                    img->depth, img->nChannels );
                cvResize( img, timg, CV_INTER_CUBIC );
            }
            result = cvFindChessboardCorners( timg, cvSize(nx, ny),
                &temp[0], &count,
                CV_CALIB_CB_ADAPTIVE_THRESH |
                CV_CALIB_CB_NORMALIZE_IMAGE);
            if( timg != img )
                cvReleaseImage( &timg );
            if( result || s == maxScale )
                for( j = 0; j < count; j++ )
            {
                temp[j].x /= s;
                temp[j].y /= s;
            }
            if( result )
                break;
        }
        if( displayCorners )
        {
            printf("%s\n", buf);
            IplImage* cimg = cvCreateImage( imageSize, 8, 3 );
            cvCvtColor( img, cimg, CV_GRAY2BGR );
            cvDrawChessboardCorners( cimg, cvSize(nx, ny), &temp[0],
                count, result );
            cvShowImage( "corners", cimg );
            cvReleaseImage( &cimg );
            int c = cvWaitKey(1000);
            if( c == 27 || c == 'q' || c == 'Q' ) //Allow ESC to quit
                exit(-1);
        }
        else
            putchar('.');
        N = pts.size();
        pts.resize(N + n, cvPoint2D32f(0,0));
        active[lr].push_back((uchar)result);
    //assert( result != 0 );
        if( result )
        {
         //Calibration will suffer without subpixel interpolation
            cvFindCornerSubPix( img, &temp[0], count,
                cvSize(11, 11), cvSize(-1,-1),
                cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,
                30, 0.01) );
            copy( temp.begin(), temp.end(), pts.begin() + N );
        }
        cvReleaseImage( &img );
    }
    fclose(f);
    printf("\n");
// HARVEST CHESSBOARD 3D OBJECT POINT LIST:
    nframes = active[0].size();//Number of good chessboads found
    objectPoints.resize(nframes*n);
    for( i = 0; i < ny; i++ )
        for( j = 0; j < nx; j++ )
        objectPoints[i*nx + j] =
        cvPoint3D32f(i*squareSize, j*squareSize, 0);
    for( i = 1; i < nframes; i++ )
        copy( objectPoints.begin(), objectPoints.begin() + n,
        objectPoints.begin() + i*n );
    npoints.resize(nframes,n);
    N = nframes*n;
    CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
    CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
    CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0] );
    CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
    cvSetIdentity(&_M1);
    cvSetIdentity(&_M2);
    cvZero(&_D1);
    cvZero(&_D2);

// CALIBRATE THE STEREO CAMERAS
    printf("Realizando el proceso de calibracion de camaras ...");
    fflush(stdout);
    cvStereoCalibrate( &_objectPoints, &_imagePoints1,
        &_imagePoints2, &_npoints,
        &_M1, &_D1, &_M2, &_D2,
        imageSize, &_R, &_T, &_E, &_F,
        cvTermCriteria(CV_TERMCRIT_ITER+
        CV_TERMCRIT_EPS, 100, 1e-5),
        CV_CALIB_FIX_ASPECT_RATIO +
        CV_CALIB_ZERO_TANGENT_DIST +
        CV_CALIB_SAME_FOCAL_LENGTH  );
    printf(" Proceso finalizado\n");


	double R1[3][3], R2[3][3], P11[3][4], P21[3][4];
	CvMat _R1 = cvMat(3,3,CV_64F,R1);
	CvMat _R2 = cvMat(3,3,CV_64F,R2);
	_P11 = cvMat(3,4,CV_64F,P11);
	_P21 = cvMat(3,4,CV_64F,P21);

	cvStereoRectify( &_M1, &_M2, &_D1, &_D2, imageSize,&_R, &_T,&_R1, &_R2, &_P11, &_P21, 0,0);


	mx1 = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
	my1 = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
	mx2 = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
	my2 = cvCreateMat( imageSize.height,imageSize.width, CV_32F );

	cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_P11,mx1,my1);
	cvInitUndistortRectifyMap(&_M2,&_D2,&_R2,&_P21,mx2,my2);

	triang = new Triangulacion(_P11,_P21,mx1,my1,mx2,my2,_F);

	printf("\nMatriz de Proyeccion 1\n");

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
			printf(" (%d %d) %f",i,j,cvmGet(&_P11,i,j));
		printf("\n");
	}

	printf("\nMatriz de Proyeccion 2\n");

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
			printf(" (%d %d) %f",i,j,cvmGet(&_P21,i,j));
		printf("\n");
	}

/*	for (int i = 0; i <3; i++)
	{
		for (int j = 0; j < 3; j++)
			printf("R (%d %d) %f",i,j,cvmGet(&_R,i,j));
		printf("\n");
	}

	for (int i = 0; i < 3; i++)
			printf("T (%d 0) %f",i,cvmGet(&_T,i,0));
*/
	printf("\n");

	triang->img1r = cvCreateImage(cvSize (imageSize.width, imageSize.height), 8, 1);
	triang->img2r = cvCreateImage(cvSize (imageSize.width, imageSize.height), 8, 1);

	return 0;
}

/**
	@param totalFotos - 0. ALL THE FREAKING TIME!
	@param cant - cantidad total de fotos capturadas - 1
	@calls triang->Puntos3D(imgI, imgD, 0, puntos, RwsTws, cXt)
	@calls ICP = new Icp(triang->p3D, triang->_X)
	@calls ICP->iterativeCP(MT, cXt, triang->_X->rows, i)
	@calls Puntos3DRotados(i)
*/
void Manager::Construccion(int totalFotos, int cant) {
	
	IplImage* imgI = 0;
	IplImage* imgD = 0;
	RwsTws = cvCreateMat(3, 3, CV_64F);
	MT = cvCreateMat(4, 4, CV_64F);
	cXt = cvCreateMat(3, 1, CV_64F);
	cvSetIdentity(RwsTws);
	cvSetZero(cXt);
	char nameD[10];
	char nameI[10];
	char num[5];
	int ini;
	itoa(totalFotos, num, 10);

	strcpy (nameI,"i");
	strcat (nameI,num);
	strcat (nameI,".JPG");

	strcpy (nameD,"d");
	strcat (nameD,num);
	strcat (nameD,".JPG");

	imgI = cvLoadImage(nameI,1);
	imgD = cvLoadImage(nameD,1);

	if(!imgI || !imgD) {
		printf("Ha ocurrido un error al cargar las imagenes");
		exit(0);
	}

	triang->Puntos3D(imgI, imgD, 0, puntos, RwsTws, cXt);

	// PARA IMPRIMIR

	CvMat* pair;
	pair = cvCreateMat( imgI->height, imgI->width*3,CV_8UC3 );
	CvMat part;

	cvGetCols( pair, &part, 0, imgI->width );
	cvCvtColor( triang->img1r, &part, CV_GRAY2BGR );
	cvGetCols( pair, &part, imgI->width,imgI->width*2 );
	cvCvtColor( triang->img2r, &part, CV_GRAY2BGR );
	totalFotos++;

	for (int i = totalFotos ; i < totalFotos + cant; i++) {
		
		char nameD2[10], nameI2[10];
		char num2[5];
		itoa(i,num2,10);

		strcpy (nameI2,"i");
		strcat (nameI2,num2);
		strcat (nameI2,".JPG");

		strcpy (nameD2,"d");
		strcat (nameD2,num2);
		strcat (nameD2,".JPG");


		IplImage* imgI2 = cvLoadImage(nameI2,1);
		IplImage* imgD2 = cvLoadImage(nameD2,1);

		if(!imgI2 || !imgD2) {
			printf("Ha ocurrido un error al cargar las imagenes");
			exit(0);
		}
	
		triang->Puntos3D(imgI2,imgD2,i,puntos,RwsTws,cXt);
	
		if (i > 1) {
			CvMat* pAux = cvCreateMat(4,1,CV_64F);
			cvmSet(pAux,3,0,1);
	
			for (int j = 0; j < triang->_X->rows; j++) {
				cvmSet(pAux,0,0,cvmGet(triang->_X,j,0));
				cvmSet(pAux,1,0,cvmGet(triang->_X,j,1));
				cvmSet(pAux,2,0,cvmGet(triang->_X,j,2));
	
				/* Se multiplica pAux por MTA, matriz que es 
				 la acumulada de las transformaciones calculadas por ICP */
				cvMatMulAdd(MTA, pAux, 0, pAux);
	
				cvmSet(triang->_X,j,0,cvmGet(pAux,0,0));
				cvmSet(triang->_X,j,1,cvmGet(pAux,1,0));
				cvmSet(triang->_X,j,2,cvmGet(pAux,2,0));
			}
		}
	
		CvMat* pair;
		pair = cvCreateMat(imgI2->height, (imgI2->width * 3), CV_8UC3 );
		CvMat part;
		cvGetCols( pair, &part, 0, imgI2->width );
	    cvCvtColor( triang->img1r, &part, CV_GRAY2BGR );
	    cvGetCols( pair, &part, imgI2->width,imgI2->width*2 );
	    cvCvtColor( triang->img2r, &part, CV_GRAY2BGR );
	
	    //Si es la última iteración mostrar las imágenes
		if (i==cant) {
			cvNamedWindow("Imagen Izquierda");
			cvShowImage("Imagen Izquierda", triang->img1r);
			cvNamedWindow("Imagen Derecha");
			cvShowImage("Imagen Derecha", triang->img2r);
		}
	
		// Icp(referencePolydata, newPolydata)
		ICP = new Icp(triang->p3D, triang->_X);
		// MT: Mat => R (Aqui deja el resultado, because fuck you)
		// cXt: Mat => T (UN-FUCKING USED)
		// triang->_X->rows: cableado a 3 en triangulacion.cpp => tamx (UN-FUCKING USED) 
		// i: "foto" / iteracion => vista (UN-FUCKING USED)
		ICP->iterativeCP(MT, cXt, triang->_X->rows, i);
		
		if (i>=1) {
			// MTA (inicialmente una identidad) que va acumulando las transformaciones de cada iteración
			cvMatMulAdd(MT, MTA, 0, MTA);
		}
	
		// i: "foto" / iteracion => vista (And guest what! UN-FUCKING USED)
		Puntos3DRotados(i);
	
		//Si es la última iteración remover outliers y actualizar <code>puntos</code>
		if (i==cant){
			std::vector<PointCgal> pointsCgal;
		
			for (int j= 0 ; j < puntos.size(); j++) {
				pointsCgal.push_back(PointCgal(puntos[j][0], puntos[j][1], puntos[j][2]));
			}
		
			const double removed_percentage = 10.0; // percentage of points to remove
			const int nb_neighbors = 24; // considers 24 nearest neighbor points
			pointsCgal.erase(CGAL::remove_outliers(pointsCgal.begin(), pointsCgal.end(),
				 									CGAL::Dereference_property_map<PointCgal>(), 
													 nb_neighbors, removed_percentage), 
				 pointsCgal.end());
		
			// Optional: after erase(), use Scott Meyer's "swap trick" to trim excess capacity
			std::vector<PointCgal>(pointsCgal).swap(pointsCgal);
		
			puntos.clear();
		
			for (int j = 0; j < pointsCgal.size(); j++) {
				vector<double> temp (3,0.0);
				temp[0] = pointsCgal[j][0];
				temp[1] = pointsCgal[j][1];
				temp[2] = pointsCgal[j][2]*-1;
				puntos.push_back(temp);
			}
		}
	}
}

/**
	Punto de entrada del programa
*/
void Manager::scaner3D() {
	char respP[5];
	respP[0]='S';

	printf(" %cDesea tomar las imagenes para el proceso de calibracion? (S/N)\n", 168);
	scanf("%1s",respP);

	if (respP[0]=='S' || respP[0]=='s') {
		char resp[5];
		resp[0]='S';

		while(resp[0]=='S' || resp[0]=='s') {
			// Captura las imagenes del patron de calibración
			int tablero = 1;
			tomarImagenes(tablero,"stereoAlineada.txt");

			// Realiza la calibracion
			StereoCalib("stereoAlineada.txt", 0);

			printf(" %cDesea repetir el proceso de calibracion? (S/N)\n",168);
			scanf("%1s",resp);
		}
	} else {
		StereoCalib("stereoAlineada.txt", 0);
	}

	// Captura las imagenes del objeto en rotacion
	printf("\nRegistro de las fotos del objeto a escanear...\n");
	int objeto = 2;
	int totalFotos = tomarImagenes(objeto ,"");

	// Obtiene las coordenadas 3D de los puntos del objeto
	MTA = cvCreateMat(4, 4, CV_64F);
	cvSetIdentity(MTA);

	printf("\nProcesando la informacion 3D...\n");

	Construccion(0, totalFotos-1);

	std::ofstream out("puntos.off");        // MOVER DE AQUI

	for (int i = 0; i < puntos.size(); i++) {
		out << puntos[i][0] << " " << puntos[i][1] << " " << puntos[i][2] << "\n";
	}

	out.close();

	printf("\nAplicando la Triangulacion de Delaunay...\n");

	Del = new dela();
	Del->Tdelaunay();

	printf("\nEl proceso ha finalizado con exito...\n");
}
