 #include "../tesis/window.h"

#include "../tesis/glwidget.h"

 Window::Window()
 {
     glWidget = new GLWidget;

     QHBoxLayout *mainLayout = new QHBoxLayout;
     mainLayout->addWidget(glWidget);
	
	openAct = new QAction(tr("Abrir..."), this);
    openAct->setShortcut(tr("Ctrl+A"));
    openAct->setStatusTip(tr("Abrir Archivo"));
	connect(openAct, SIGNAL(triggered()), glWidget, SLOT(onVentanaAbrir()));
    connect(openAct, SIGNAL(triggered()), glWidget, SLOT(onAbrir()));


	closeAct = new QAction(tr("Cerrar..."), this);
    closeAct->setShortcut(tr("Ctrl+C"));
    closeAct->setStatusTip(tr("Cerrar Archivo"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

	escalAct = new QAction(tr("Escalar..."), this);
    escalAct->setShortcut(tr("Ctrl+E"));
    escalAct->setStatusTip(tr("Escalar objeto"));
    connect(escalAct, SIGNAL(triggered()), glWidget, SLOT(onEscalar()));

	trasAct = new QAction(tr("Trasladar..."), this);
    trasAct->setShortcut(tr("Ctrl+T"));
    trasAct->setStatusTip(tr("Trasladar objeto"));
    connect(trasAct, SIGNAL(triggered()), glWidget, SLOT(onTrasladar()));

	rotAct = new QAction(tr("Rotar..."), this);
    rotAct->setShortcut(tr("Ctrl+R"));
    rotAct->setStatusTip(tr("Rotar objeto"));
    connect(rotAct, SIGNAL(triggered()), glWidget, SLOT(onRotar()));

	zBufAct = new QAction(tr("On/Off Z-Buffer..."), this);
    zBufAct->setShortcut(tr("Ctrl+Z"));
    zBufAct->setStatusTip(tr("Habilitar/Deshabilitar ZBuffer"));
    connect(zBufAct, SIGNAL(triggered()), glWidget, SLOT(onZBuffer()));

	fogAct = new QAction(tr("On/Off Niebla..."), this);
    fogAct->setShortcut(tr("Ctrl+F"));
    fogAct->setStatusTip(tr("Habilitar/Deshabilitar Niebla"));
    connect(fogAct, SIGNAL(triggered()), glWidget, SLOT(onFog()));

	colFogAct = new QAction(tr("Color Niebla..."), this);
    colFogAct->setStatusTip(tr("Cambiar Color Niebla"));
    connect(colFogAct, SIGNAL(triggered()), glWidget, SLOT(onColorFog()));

	planoAct = new QAction(tr("On/Off Plano de Corte..."), this);
    planoAct->setShortcut(tr("Ctrl+P"));
    planoAct->setStatusTip(tr("Habilitar/Deshabilitar Plano de Corte"));
    connect(planoAct, SIGNAL(triggered()), glWidget, SLOT(onPlano()));

	meshAct = new QAction(tr("On/Off Mallado..."), this);
    meshAct->setShortcut(tr("Ctrl+M"));
    meshAct->setStatusTip(tr("Mostrar/Ocultar Mallado"));
    connect(meshAct, SIGNAL(triggered()), glWidget, SLOT(onMalla()));

	colMeshAct = new QAction(tr("Color Mallado..."), this);
    colMeshAct->setStatusTip(tr("Cambiar Color Mallado"));
    connect(colMeshAct, SIGNAL(triggered()), glWidget, SLOT(onColorMalla()));

	boxAct = new QAction(tr("On/Off Bounding Box..."), this);
    boxAct->setShortcut(tr("Ctrl+B"));
    boxAct->setStatusTip(tr("Mostrar/Ocultar Bounding Box"));
    connect(boxAct, SIGNAL(triggered()), glWidget, SLOT(onBox()));

	colBoxAct = new QAction(tr("Color Bounding Box..."), this);
    colBoxAct->setStatusTip(tr("Cambiar Color Bounding Box"));
    connect(colBoxAct, SIGNAL(triggered()), glWidget, SLOT(onColorBox()));

	fillAct = new QAction(tr("On/Off Relleno..."), this);
    fillAct->setShortcut(tr("Ctrl+R"));
    fillAct->setStatusTip(tr("Mostrar/Ocultar Relleno"));
    connect(fillAct, SIGNAL(triggered()), glWidget, SLOT(onRelleno()));

	colFillAct = new QAction(tr("Color Relleno..."), this);
    colFillAct->setStatusTip(tr("Cambiar Color Relleno"));
    connect(colFillAct, SIGNAL(triggered()), glWidget, SLOT(onColorRelleno()));

	edgeAct = new QAction(tr("On/Off Vertices..."), this);
    edgeAct->setShortcut(tr("Ctrl+V"));
    edgeAct->setStatusTip(tr("Mostrar/Ocultar Vertices"));
    connect(edgeAct, SIGNAL(triggered()), glWidget, SLOT(onVertices()));

	colEdgeAct = new QAction(tr("Color Vertices..."), this);
    colEdgeAct->setStatusTip(tr("Cambiar Color Vertices"));
    connect(colEdgeAct, SIGNAL(triggered()), glWidget, SLOT(onColorVertices()));

	fileMenu = new QMenu;
	QMenuBar *m = new QMenuBar;
	fileMenu = m->addMenu(tr("&Archivo"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(closeAct);

	editMenu = new QMenu;
	editMenu = m->addMenu(tr("&Editar"));
	editMenu->addAction(escalAct);
	editMenu->addAction(rotAct);
	editMenu->addAction(trasAct);
	editMenu->addAction(colFogAct);
	editMenu->addAction(colMeshAct);
	editMenu->addAction(colBoxAct);
	editMenu->addAction(colFillAct);
	editMenu->addAction(colEdgeAct);

	toolMenu = new QMenu;
	toolMenu = m->addMenu(tr("&Herramientas"));
	toolMenu->addAction(zBufAct);
	toolMenu->addAction(fogAct);
	toolMenu->addAction(planoAct);
	toolMenu->addAction(meshAct);
	toolMenu->addAction(boxAct);
	toolMenu->addAction(fillAct);
	toolMenu->addAction(edgeAct);

    setLayout(mainLayout);
    setWindowTitle(tr("3D SCAN"));
	mainLayout->setMenuBar(m);

 }
