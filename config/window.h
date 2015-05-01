#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMainWindow>
 #include <QtGui>
#include <QtGui/QToolBar>

class QSlider;

class GLWidget;

class Window : public QWidget
{
	Q_OBJECT

public:
	Window();

private:
	GLWidget *glWidget;
	QAction *openAct;
	QAction *escalAct;
	QAction *trasAct;
	QAction *rotAct;
	QAction *zBufAct;
	QAction *fogAct;
	QAction *colFogAct;
	QAction *planoAct;
	QAction *meshAct;
	QAction *colMeshAct;
	QAction *boxAct;
	QAction *colBoxAct;
	QAction *fillAct;
	QAction *colFillAct;
	QAction *edgeAct;
	QAction *colEdgeAct;
	QAction *closeAct;
	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *toolMenu;

};

#endif