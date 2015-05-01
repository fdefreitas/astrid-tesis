#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <vector>
#include <list>
#include <QEvent>
#include <QColorDialog>

#include "../tesis/manager.h"

using namespace std;

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent = 0);
	~GLWidget();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

public slots:
	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);
	void onAbrir();
	void onVentanaAbrir();
	void onEscalar();
	void onTrasladar();
	void onRotar();
	void onZBuffer();
	void onFog();
	void onRelleno();
	void onPlano();
	void onMalla();
	void onVertices();
	void onBox();
	void onColorMalla();
	void onColorBox();
	void onColorRelleno();
	void onColorVertices();
	void onColorFog();

	signals:
	void xRotationChanged(int angle);
	void yRotationChanged(int angle);
	void zRotationChanged(int angle);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *evento);
	void mouseMoveEvent(QMouseEvent *evento);
	void extremos();
	void malla();
	void boundingBox();
	void vertices();
	vector<vector <float> > vertex;
	vector<vector <int> > poly;
	double thePlane[4];
	bool escalar, trasladar, rotar, zbuf, fog, fill, plano, mesh, edge, box;
	Manager* p;
	bool ventana;

private:
	void normalizeAngle(int *angle);

	float xRot;
	float yRot;
	float zRot;
	float xEsc;
	float xTras;
	float yTras;
	float zTras;
	QPoint lastPos;
	float medX;
	float medY;
	float medZ;
	float maxX;
	float maxY;
	float maxZ;
	float minX;
	float minY;
	float minZ;
	QColor mallaColor, fillColor, edgeColor, boxColor, fogColor;
	QString fileName;
};

#endif