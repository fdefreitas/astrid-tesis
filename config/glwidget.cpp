#include "../tesis/glwidget.h"

#include <QtGui>
#include <QtOpenGL>
#include <math.h>

GLWidget::GLWidget(QWidget *parent)
 : QGLWidget(parent)
{
	xRot = 0;
	yRot = 0;
	zRot = 0;
	xEsc = 0;
	medX = 0;
	medY = 0;
	medZ = 0;
	xTras = 0.0;
	yTras = 0.0;
	zTras = -7.0;
	rotar = fill = true;
	escalar = trasladar = zbuf  = fog  = plano = edge = box = false;
	mesh = true;
	mallaColor = QColor::fromRgb(255,0,0);
	fillColor = QColor::fromRgb(0,255,0);
	edgeColor = QColor::fromRgb(255,255,255);
	boxColor = QColor::fromRgb(255,0,0);
	fogColor = QColor::fromRgb(25,25,25);
	ventana = false;
	p = new Manager();
	p->scaner3D();

	fileName = "delaunay.off";
	onAbrir();
}

GLWidget::~GLWidget()
{
	makeCurrent();
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(50,50);
}

QSize GLWidget::sizeHint() const
{
	return QSize(400,400);
}

void GLWidget::setXRotation(int angle)
{
	normalizeAngle(&angle);
		if (angle != xRot) 
			xRot = angle;

		updateGL();
}

void GLWidget::setYRotation(int angle)
{
	normalizeAngle(&angle);

	if (angle != yRot) 
		yRot = angle;

	updateGL();
}

void GLWidget::setZRotation(int angle)
{
	normalizeAngle(&angle);
		if (angle != zRot) 
			zRot = angle;
}

void GLWidget::initializeGL()
{
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0,1.0);
}

void GLWidget::paintGL()
{
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0.0);       // Color de fondo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(xTras, yTras, zTras);

	glRotatef(xRot / 8, 1.0, 0.0, 0.0);
	glRotatef(yRot / 8, 0.0, 1.0, 0.0);
	glRotatef(zRot / 8, 0.0, 0.0, 1.0);	
			
	if (escalar && xEsc>0 ){
		glScalef(xEsc,xEsc,xEsc);
	}
	else if(!escalar && xEsc!=0)
	{
		glScalef(xEsc,xEsc,xEsc);
	}

	if (plano)
	{
		thePlane[0] = medX;			//	Para plano de corte
		thePlane[1] = 0.0;
		thePlane[2] = 0.0;
		thePlane[3] = 0.0;
		glClipPlane(GL_CLIP_PLANE0, thePlane);
	}
	
	
	glTranslatef(medX, medY, medZ);

	if (fill)
	{
		glColor3f((float)fillColor.red()/255,(float)fillColor.green()/255,(float)fillColor.blue()/255);

		glBegin(GL_TRIANGLES);
			for (int i = 0; i < poly.size(); i++)
			{
				int v1 = poly[i][0];
				int v2 = poly[i][1];
				int v3 = poly[i][2];	

				glVertex3f(vertex[v1][0],vertex[v1][1],vertex[v1][2]);
				glVertex3f(vertex[v2][0],vertex[v2][1],vertex[v2][2]);
				glVertex3f(vertex[v3][0],vertex[v3][1],vertex[v3][2]);
			}
		glEnd();
	}

	if (edge || (!fill && !mesh))
		vertices();

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		if(mesh)
		malla();

		if (box)
		boundingBox();

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void GLWidget::resizeGL(int width, int height)
{
	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat x = GLfloat(width)/height;
	//glOrtho(-1.5, +1.5, +1.5, -1.5, -1.0, 15);
	gluPerspective(45.0f, 1.333f, .0001f, 30.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *evento)
{
	lastPos = evento->pos();

}

void GLWidget::mouseMoveEvent(QMouseEvent *evento)
{
	int dx = evento->x() - lastPos.x();
	int dy = evento->y() - lastPos.y();

	if (evento->buttons() & Qt::LeftButton) {
		if (rotar){
			setXRotation(xRot + 16 * dy);
			setYRotation(yRot + 16 * dx);
		}
		if (escalar){
			xEsc += (float)dx/8;
		}
		if (trasladar){
			xTras += ((float)dx/500);
			yTras -= ((float)dy/500);
		}
	} else if (evento->buttons() & Qt::RightButton) {
		if (rotar){
			setXRotation(xRot + 16 * dy);
			setZRotation(zRot + 16 * dx);
		}
		if (trasladar){
			zTras += ((float)dy/500);
		}
	}
	lastPos = evento->pos();
	updateGL();
}

void GLWidget::normalizeAngle(int *angle)
{
	while (*angle < 0)
		*angle += 360 * 8;
	while (*angle > 360 * 8)
		*angle -= 360 * 8;
}

void GLWidget::onVentanaAbrir()
{
	ventana = true;
}

void GLWidget::onAbrir()
{
	if (ventana)
	fileName = QFileDialog::getOpenFileName(this, QObject::tr("Open File"),"/",tr("*.off"));
	
	QFile file( fileName );

	if ( file.open( QIODevice::ReadOnly) ) {
		QTextStream f( &file );
        
		QString off = f.readLine();

		if (off!="OFF") return;
		while (!f.atEnd()){

			QString s = f.readLine();
			QStringList list1 = s.split(" ");
			int numV = list1[0].toInt();
			int numP = list1[1].toInt();
			vertex.resize(numV);
			for (int i=0; i < numV; i++)
				vertex[i].resize(3);

			poly.resize(numP);

			for (int i=0; i < vertex.size(); i++)
			{
				s = f.readLine();
				QStringList num = s.split(" ");
				for (int j = 0; j < 3; j++)
					vertex[i][j] = num[j].toFloat();
			}

			extremos();

			medX = (float)(minX + maxX)/2;
			medX *= -1;
			medY = (float)(minY + maxY)/2;
			medY *= -1;
			medZ = (float)(minZ + maxZ)/2;
			medZ *= -1;

			for (int i=0; i< poly.size(); i++)
			{
				s = f.readLine();
				QStringList num = s.split(" ");
				poly[i].resize(num.size()-2);
				for (int j = 2; j < num.size(); j++)
					poly[i][j-2] = num[j].toInt();
			}
		}
	}
       
	file.close();
	updateGL();
    
}


void GLWidget::onEscalar()
{
	escalar = true;
	trasladar = false;
	rotar = false;
}

void GLWidget::onTrasladar()
{
	escalar = false;
	trasladar = true;
	rotar = false;
}

void GLWidget::onRotar()
{
	escalar = false;
	trasladar = false;
	rotar = true;
}

void GLWidget::extremos()
{
	minX = INT_MAX;
	minY = INT_MAX;
	minZ = INT_MAX;
	maxX = INT_MIN;
	maxY = INT_MIN;
	maxZ = INT_MIN;

/*	printf("AQUI\n");

	printf("tam %d\n",p->puntos.size());
	for (int i=0; i < p->puntos.size(); i++)
	{
		if (p->puntos[i][0]<minX) minX = p->puntos[i][0];
		if (p->puntos[i][0]>maxX) maxX = p->puntos[i][0];

		if (p->puntos[i][1]<minY) minY = p->puntos[i][1];
		if (p->puntos[i][1]>maxY) maxY = p->puntos[i][1];

		if (p->puntos[i][2]<minZ) minZ = p->puntos[i][2];
		if (p->puntos[i][2]>maxZ) maxZ = p->puntos[i][2];
	}
*/

	for (int i=0; i < vertex.size(); i++)
	{
		if (vertex[i][0]<minX) minX = vertex[i][0];
		if (vertex[i][0]>maxX) maxX = vertex[i][0];

		if (vertex[i][1]<minY) minY = vertex[i][1];
		if (vertex[i][1]>maxY) maxY = vertex[i][1];

		if (vertex[i][2]<minZ) minZ = vertex[i][2];
		if (vertex[i][2]>maxZ) maxZ = vertex[i][2];
	}

}

void GLWidget::boundingBox()
{
	glColor3f((float)boxColor.red()/255, (float)boxColor.green()/255,(float)boxColor.blue()/255);
	glBegin(GL_POLYGON);
		  glVertex3f(minX,minY,minZ);
		  glVertex3f(minX,maxY,minZ);
		  glVertex3f(maxX,maxY,minZ);
		  glVertex3f(maxX,minY,minZ);
	glEnd();
	
	glBegin(GL_POLYGON);
		  glVertex3f(minX,minY,maxZ);
		  glVertex3f(minX,maxY,maxZ);
		  glVertex3f(maxX,maxY,maxZ);
		  glVertex3f(maxX,minY,maxZ);
	glEnd();

	glBegin(GL_POLYGON);
		  glVertex3f(minX,maxY,minZ);
		  glVertex3f(minX,maxY,maxZ);
		  glVertex3f(minX,minY,maxZ);
		  glVertex3f(minX,minY,minZ);
	glEnd();

	glBegin(GL_POLYGON);
		  glVertex3f(maxX,maxY,minZ);
		  glVertex3f(maxX,maxY,maxZ);
		  glVertex3f(maxX,minY,maxZ);
		  glVertex3f(maxX,minY,minZ);
	glEnd();
}

void GLWidget::malla()
{
	glColor3f((float)mallaColor.red()/255,(float)mallaColor.green()/255,(float)mallaColor.blue()/255);
	glBegin(GL_TRIANGLES);
		for (int i = 0; i < poly.size(); i++)
		{
			int v1 = poly[i][0];
			int v2 = poly[i][1];
			int v3 = poly[i][2];	

			glVertex3f(vertex[v1][0],vertex[v1][1],vertex[v1][2]);
			glVertex3f(vertex[v2][0],vertex[v2][1],vertex[v2][2]);
			glVertex3f(vertex[v3][0],vertex[v3][1],vertex[v3][2]);
		}
	glEnd();
}

void GLWidget::vertices()
{
	glColor3f((float)edgeColor.red()/255,(float)edgeColor.green()/255,(float)edgeColor.blue()/255);
	/*glBegin(GL_POINTS);
		for (int i = 0; i < p->puntos.size(); i++)
		{
			glVertex3f(p->puntos[i][0],p->puntos[i][1],p->puntos[i][2]);
		}
	glEnd();*/

	glBegin(GL_POINTS);
		for (int i = 0; i < vertex.size(); i++)
		{
			glVertex3f(vertex[i][0],vertex[i][1],vertex[i][2]);
		}
	glEnd();

}

void GLWidget::onZBuffer()
{
	zbuf = !zbuf;

	if (!zbuf)
		glDisable(GL_DEPTH_TEST);
	else
	{
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0);
	}
}
void GLWidget::onFog()
{
	fog = !fog;

	if (!fog)
		glDisable(GL_FOG);
	else
	{
		GLfloat fog_color[4] = { (float)fogColor.red()/255, (float)fogColor.green()/255, (float)fogColor.blue()/255, 0.0};

		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, 0.0);
		glFogf(GL_FOG_END, 15.0);
		glFogfv(GL_FOG_COLOR, fog_color);	// COLOR DE LA NIEBLA
//		glClearColor(0.1, 0.1, 0.1, 0.0);
		glEnable(GL_FOG);
	}
}
void GLWidget::onRelleno()
{
	fill = !fill;
}
void GLWidget::onPlano()
{
	plano = !plano;

	if (!plano)
		glDisable(GL_CLIP_PLANE0);
	else
		glEnable(GL_CLIP_PLANE0);
}
void GLWidget::onMalla()
{
	mesh = !mesh;
}
void GLWidget::onVertices()
{
	edge = !edge;
}
void GLWidget::onBox()
{
	box = !box;
}
void GLWidget::onColorMalla()
{
	mallaColor = QColorDialog::getColor();
}
void GLWidget::onColorBox()
{
	boxColor = QColorDialog::getColor();
}
void GLWidget::onColorRelleno()
{
	fillColor = QColorDialog::getColor();
}
void GLWidget::onColorVertices()
{
	edgeColor = QColorDialog::getColor();
}
void GLWidget::onColorFog()
{
	fogColor = QColorDialog::getColor();
}