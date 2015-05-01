#include <QApplication>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>
#include <GL/glut.h>

#include "../tesis/window.h"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Window window;
	window.show();
	return app.exec();
}