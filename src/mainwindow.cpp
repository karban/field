// This file is part of Agros2D.
//
// Agros2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Agros2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Agros2D.  If not, see <http://www.gnu.org/licenses/>.
//
// hp-FEM group (http://hpfem.org/)
// University of Nevada, Reno (UNR) and University of West Bohemia, Pilsen
// Email: agros2d@googlegroups.com, home page: http://hpfem.org/agros2d/

#include "mainwindow.h"

#include "gui/about.h"
#include "gui/imageloader.h"

#include "util/checkversion.h"

#include "scene.h"
#include "scenebasic.h"
#include "sceneview_common.h"
#include "sceneview_geometry.h"
#include "sceneview_post3d.h"
#include "tooltipview.h"
#include "logview.h"
#include "infowidget.h"
#include "settings.h"
#include "preprocessorview.h"
#include "postprocessorview.h"
#include "chartdialog.h"
#include "confdialog.h"
#include "pythonlabagros.h"
#include "problemdialog.h"
#include "resultsview.h"
#include "scenetransformdialog.h"

#include "gl2ps/gl2ps.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    Util::createSingleton();

    createPythonEngine(new PythonEngineField());

    // scene
    postHermes = new PostView();

    sceneViewPreprocessor = new SceneViewPreprocessor(this);
    sceneViewPost3D = new SceneViewPost3D(postHermes, this);
    sceneViewBlank = new QLabel("TODO");
    sceneInfoWidget = new InfoWidget(sceneViewPreprocessor, this);
    // preprocessor
    preprocessorWidget = new PreprocessorWidget(sceneViewPreprocessor, this);
    connect(Util::problem(), SIGNAL(fieldsChanged()), preprocessorWidget, SLOT(refresh()));
    // postprocessor
    postprocessorWidget = new PostprocessorWidget(sceneViewPreprocessor, sceneViewPost3D, this);
    // settings
    settingsWidget = new SettingsWidget(this);
    // problem
    problemWidget = new ProblemWidget(this);

    scriptEditorDialog = new PythonLabAgros(currentPythonEngine(), QApplication::arguments(), this);
    sceneTransformDialog = new SceneTransformDialog(this);

    createActions();
    createViews();
    createMenus();
    createToolBars();
    createMain();

    // post hermes
    connect(problemWidget, SIGNAL(changed()), postHermes, SLOT(refresh()));
    connect(settingsWidget, SIGNAL(apply()), postHermes, SLOT(refresh()));
    connect(postprocessorWidget, SIGNAL(apply()), postHermes, SLOT(refresh()));
    currentPythonEngineAgros()->setPostHermes(postHermes);

    connect(Util::problem(), SIGNAL(meshed()), this, SLOT(setControls()));
    connect(Util::problem(), SIGNAL(solved()), this, SLOT(setControls()));

    connect(tabViewLayout, SIGNAL(currentChanged(int)), this, SLOT(setControls()));
    connect(Util::scene(), SIGNAL(invalidated()), this, SLOT(setControls()));
    connect(Util::scene(), SIGNAL(fileNameChanged(QString)), this, SLOT(doSetWindowTitle(QString)));
    connect(Util::scene()->actTransform, SIGNAL(triggered()), this, SLOT(doTransform()));

    connect(Util::scene(), SIGNAL(cleared()), this, SLOT(clear()));
    connect(postprocessorWidget, SIGNAL(apply()), this, SLOT(setControls()));
    connect(actSceneModeGroup, SIGNAL(triggered(QAction *)), this, SLOT(setControls()));
    connect(actSceneModeGroup, SIGNAL(triggered(QAction *)), sceneViewPreprocessor, SLOT(refresh()));

    // preprocessor
    connect(problemWidget, SIGNAL(changed()), sceneViewPreprocessor, SLOT(refresh()));
    connect(settingsWidget, SIGNAL(apply()), sceneViewPreprocessor, SLOT(refresh()));
    connect(sceneViewPreprocessor, SIGNAL(sceneGeometryModeChanged(SceneGeometryMode)), tooltipView, SLOT(loadTooltip(SceneGeometryMode)));
    connect(sceneViewPreprocessor, SIGNAL(sceneGeometryModeChanged(SceneGeometryMode)), tooltipView, SLOT(loadTooltipPost2D()));
    currentPythonEngineAgros()->setSceneViewGeometry(sceneViewPreprocessor);

    // postprocessor 3d
    currentPythonEngineAgros()->setSceneViewPost3D(sceneViewPost3D);

    // info
    connect(problemWidget, SIGNAL(changed()), sceneInfoWidget, SLOT(refresh()));
    connect(postprocessorWidget, SIGNAL(apply()), sceneInfoWidget, SLOT(refresh()));

    connect(Util::problem(), SIGNAL(fieldsChanged()), this, SLOT(doFieldsChanged()));

    sceneViewPreprocessor->clear();
    sceneViewPost3D->clear();

    QSettings settings;
    recentFiles = settings.value("MainWindow/RecentFiles").value<QStringList>();

    Util::scene()->clear();

    problemWidget->actProperties->trigger();
    sceneViewPreprocessor->doZoomBestFit();

    // set recent files
    setRecentFiles();

    // accept drops
    setAcceptDrops(true);

    // macx
    setUnifiedTitleAndToolBarOnMac(true);

    if (settings.value("General/CheckVersion", true).value<bool>())
        checkForNewVersion(true);

    restoreGeometry(settings.value("MainWindow/Geometry", saveGeometry()).toByteArray());
    restoreState(settings.value("MainWindow/State", saveState()).toByteArray());
    splitter->restoreState(settings.value("MainWindow/SplitterState").toByteArray());
    // show/hide control panel
    actHideControlPanel->setChecked(settings.value("MainWindow/ControlPanel", true).toBool());
    doHideControlPanel();

    setControls();

    // parameters
    QStringList args = QCoreApplication::arguments();
    for (int i = 1; i < args.count(); i++)
    {
        if (args[i] == "--verbose" || args[i] == "/verbose")
            continue;

        if (args[i] == "--run" || args[i] == "-r" || args[i] == "/r")
        {
            QString scriptName = args[++i];

            if (QFile::exists(scriptName))
            {
                consoleView->console()->connectStdOut();
                currentPythonEngineAgros()->runScript(readFileContent(scriptName));
                consoleView->console()->disconnectStdOut();
            }
            else
            {
                qWarning() << "Script " << scriptName << "not found.";
            }

            continue;
        }

        QString fileName = args[i];
        open(fileName);
    }
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("MainWindow/Geometry", saveGeometry());
    settings.setValue("MainWindow/State", saveState());
    settings.setValue("MainWindow/RecentFiles", recentFiles);
    settings.setValue("MainWindow/SplitterState", splitter->saveState());
    settings.setValue("MainWindow/ControlPanel", actHideControlPanel->isChecked());

    // remove temp files
    removeDirectory(tempProblemDir());
}

void MainWindow::open(const QString &fileName)
{
    doDocumentOpen(fileName);
}

void MainWindow::createActions()
{
    actDocumentNew = new QAction(icon("document-new"), tr("&New..."), this);
    actDocumentNew->setShortcuts(QKeySequence::New);
    actDocumentNew->setStatusTip(tr("Create a new file"));
    connect(actDocumentNew, SIGNAL(triggered()), this, SLOT(doDocumentNew()));

    actDocumentOpen = new QAction(icon("document-open"), tr("&Open..."), this);
    actDocumentOpen->setShortcuts(QKeySequence::Open);
    actDocumentOpen->setStatusTip(tr("Open an existing file"));
    connect(actDocumentOpen, SIGNAL(triggered()), this, SLOT(doDocumentOpen()));

    actDocumentSave = new QAction(icon("document-save"), tr("&Save"), this);
    actDocumentSave->setShortcuts(QKeySequence::Save);
    actDocumentSave->setStatusTip(tr("Save the file to disk"));
    connect(actDocumentSave, SIGNAL(triggered()), this, SLOT(doDocumentSave()));

    actDocumentSaveWithSolution = new QAction(icon(""), tr("Save with solution"), this);
    actDocumentSaveWithSolution->setStatusTip(tr("Save the file to disk with solution"));
    connect(actDocumentSaveWithSolution, SIGNAL(triggered()), this, SLOT(doDocumentSaveWithSolution()));

    actDocumentSaveAs = new QAction(icon("document-save-as"), tr("Save &As..."), this);
    actDocumentSaveAs->setShortcuts(QKeySequence::SaveAs);
    actDocumentSaveAs->setStatusTip(tr("Save the file under a new name"));
    connect(actDocumentSaveAs, SIGNAL(triggered()), this, SLOT(doDocumentSaveAs()));

    actDocumentClose = new QAction(tr("&Close"), this);
    actDocumentClose->setShortcuts(QKeySequence::Close);
    actDocumentClose->setStatusTip(tr("Close the file"));
    connect(actDocumentClose, SIGNAL(triggered()), this, SLOT(doDocumentClose()));

    actDocumentSaveImage = new QAction(tr("Export image..."), this);
    actDocumentSaveImage->setStatusTip(tr("Export image to file"));
    connect(actDocumentSaveImage, SIGNAL(triggered()), this, SLOT(doDocumentSaveImage()));

    actDocumentSaveGeometry = new QAction(tr("Export geometry..."), this);
    actDocumentSaveGeometry->setStatusTip(tr("Export geometry to file"));
    connect(actDocumentSaveGeometry, SIGNAL(triggered()), this, SLOT(doDocumentSaveGeometry()));

    actExit = new QAction(icon("application-exit"), tr("E&xit"), this);
    actExit->setShortcut(tr("Ctrl+Q"));
    actExit->setStatusTip(tr("Exit the application"));
    actExit->setMenuRole(QAction::QuitRole);
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));

    // undo framework
    actUndo = Util::scene()->undoStack()->createUndoAction(this);
    actUndo->setIcon(icon("edit-undo"));
    actUndo->setIconText(tr("&Undo"));
    actUndo->setShortcuts(QKeySequence::Undo);
    actUndo->setStatusTip(tr("Undo operation"));

    actRedo = Util::scene()->undoStack()->createRedoAction(this);
    actRedo->setIcon(icon("edit-redo"));
    actRedo->setIconText(tr("&Redo"));
    actRedo->setShortcuts(QKeySequence::Redo);
    actRedo->setStatusTip(tr("Redo operation"));

    actCopy = new QAction(icon("edit-copy"), tr("Copy image to clipboard"), this);
    actCopy->setShortcuts(QKeySequence::Copy);
    actCopy->setStatusTip(tr("Copy image from workspace to clipboard."));
    connect(actCopy, SIGNAL(triggered()), this, SLOT(doCopy()));

    actHelp = new QAction(icon("help-contents"), tr("&Help"), this);
    actHelp->setStatusTip(tr("Show help"));
    actHelp->setShortcut(QKeySequence::HelpContents);
    connect(actHelp, SIGNAL(triggered()), this, SLOT(doHelp()));

    actHelpShortCut = new QAction(icon(""), tr("&Shortcuts"), this);
    actHelpShortCut->setStatusTip(tr("Shortcuts"));
    connect(actHelpShortCut, SIGNAL(triggered()), this, SLOT(doHelpShortCut()));

    actOnlineHelp = new QAction(icon(""), tr("&Online help"), this);
    actOnlineHelp->setStatusTip(tr("Online help"));
    connect(actOnlineHelp, SIGNAL(triggered()), this, SLOT(doOnlineHelp()));

    actCheckVersion = new QAction(icon(""), tr("Check version"), this);
    actCheckVersion->setStatusTip(tr("Check version"));
    connect(actCheckVersion, SIGNAL(triggered()), this, SLOT(doCheckVersion()));

    actAbout = new QAction(icon("about"), tr("About &Agros2D"), this);
    actAbout->setStatusTip(tr("Show the application's About box"));
    actAbout->setMenuRole(QAction::AboutRole);
    connect(actAbout, SIGNAL(triggered()), this, SLOT(doAbout()));

    actAboutQt = new QAction(icon("help-about"), tr("About &Qt"), this);
    actAboutQt->setStatusTip(tr("Show the Qt library's About box"));
    actAboutQt->setMenuRole(QAction::AboutQtRole);
    connect(actAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    actOptions = new QAction(icon("options"), tr("&Options"), this);
    actOptions->setStatusTip(tr("Options"));
    actOptions->setMenuRole(QAction::PreferencesRole);
    connect(actOptions, SIGNAL(triggered()), this, SLOT(doOptions()));

    actSolve = new QAction(icon("run"), tr("&Solve"), this);
    actSolve->setShortcut(QKeySequence(tr("Alt+S")));
    actSolve->setStatusTip(tr("Solve problem"));
    connect(actSolve, SIGNAL(triggered()), this, SLOT(doSolve()));

    actChart = new QAction(icon("chart"), tr("&Chart"), this);
    actChart->setStatusTip(tr("Chart"));
    connect(actChart, SIGNAL(triggered()), this, SLOT(doChart()));

    actFullScreen = new QAction(icon("view-fullscreen"), tr("Fullscreen mode"), this);
    actFullScreen->setShortcut(QKeySequence(tr("F11")));
    connect(actFullScreen, SIGNAL(triggered()), this, SLOT(doFullScreen()));

    actDocumentOpenRecentGroup = new QActionGroup(this);
    connect(actDocumentOpenRecentGroup, SIGNAL(triggered(QAction *)), this, SLOT(doDocumentOpenRecent(QAction *)));

    actScriptEditor = new QAction(icon("script-python"), tr("PythonLab"), this);
    actScriptEditor->setStatusTip(tr("Script editor"));
    actScriptEditor->setShortcut(Qt::Key_F9);
    connect(actScriptEditor, SIGNAL(triggered()), this, SLOT(doScriptEditor()));

    actScriptEditorRunScript = new QAction(icon("script"), tr("Run &script..."), this);
    actScriptEditorRunScript->setStatusTip(tr("Run script..."));
    connect(actScriptEditorRunScript, SIGNAL(triggered()), this, SLOT(doScriptEditorRunScript()));

    actScriptEditorRunCommand = new QAction(icon("run"), tr("Run &command..."), this);
    actScriptEditorRunCommand->setShortcut(QKeySequence(tr("Alt+C")));
    actScriptEditorRunCommand->setStatusTip(tr("Run command..."));
    connect(actScriptEditorRunCommand, SIGNAL(triggered()), this, SLOT(doScriptEditorRunCommand()));

    // zoom actions (geometry, post2d and post3d)
    // scene - zoom
    actSceneZoomIn = new QAction(icon("zoom-in"), tr("Zoom in"), this);
    actSceneZoomIn->setShortcut(QKeySequence::ZoomIn);
    actSceneZoomIn->setStatusTip(tr("Zoom in"));

    actSceneZoomOut = new QAction(icon("zoom-out"), tr("Zoom out"), this);
    actSceneZoomOut->setShortcut(QKeySequence::ZoomOut);
    actSceneZoomOut->setStatusTip(tr("Zoom out"));

    actSceneZoomBestFit = new QAction(icon("zoom-original"), tr("Zoom best fit"), this);
    actSceneZoomBestFit->setShortcut(tr("Ctrl+0"));
    actSceneZoomBestFit->setStatusTip(tr("Best fit"));

    actSceneZoomRegion = new QAction(icon("zoom-fit-best"), tr("Zoom region"), this);
    actSceneZoomRegion->setStatusTip(tr("Zoom region"));
    actSceneZoomRegion->setCheckable(true);

    actSceneModeGroup = new QActionGroup(this);
    actSceneModeGroup->addAction(problemWidget->actProperties);
    actSceneModeGroup->addAction(sceneViewPreprocessor->actSceneModePreprocessor);
    actSceneModeGroup->addAction(sceneViewPost3D->actSceneModePost3D);
    actSceneModeGroup->addAction(settingsWidget->actSettings);

    actHideControlPanel = new QAction(icon("showhide"), tr("Show/hide control panel"), this);
    actHideControlPanel->setShortcut(tr("Alt+0"));
    actHideControlPanel->setCheckable(true);
    connect(actHideControlPanel, SIGNAL(triggered()), this, SLOT(doHideControlPanel()));
}


void MainWindow::doFieldsChanged()
{
    //TODO it is not necessary to create whole menu, it only needs to be adjusted
    //TODO what hapens to old menu object? Memory leaks???

    createMenus();
}

void MainWindow::createMenus()
{
    menuBar()->clear();

    mnuRecentFiles = new QMenu(tr("&Recent files"), this);
    mnuFileImportExport = new QMenu(tr("Import/Export"), this);
    mnuFileImportExport->addSeparator();
    mnuFileImportExport->addAction(actDocumentSaveImage);
    mnuFileImportExport->addAction(actDocumentSaveGeometry);

    mnuFile = menuBar()->addMenu(tr("&File"));
    mnuFile->addAction(actDocumentNew);
    mnuFile->addAction(actDocumentOpen);
    mnuFile->addMenu(mnuRecentFiles);
    mnuFile->addSeparator();
    mnuFile->addAction(actDocumentSave);
    mnuFile->addAction(actDocumentSaveAs);
    mnuFile->addSeparator();
    mnuFile->addMenu(mnuFileImportExport);
    mnuFile->addSeparator();
#ifndef Q_WS_MAC
    mnuFile->addSeparator();
    mnuFile->addAction(actDocumentClose);
    mnuFile->addAction(actExit);
#endif

    mnuEdit = menuBar()->addMenu(tr("E&dit"));
    mnuEdit->addAction(actUndo);
    mnuEdit->addAction(actRedo);
    mnuEdit->addSeparator();
    mnuEdit->addAction(Util::scene()->actDeleteSelected);
    mnuEdit->addSeparator();
    mnuEdit->addAction(sceneViewPreprocessor->actSceneViewSelectRegion);
    mnuEdit->addAction(Util::scene()->actTransform);
#ifdef Q_WS_X11
    mnuEdit->addSeparator();
    mnuEdit->addAction(actOptions);
#endif

    QMenu *mnuProjection = new QMenu(tr("Projection"), this);
    mnuProjection->addAction(sceneViewPost3D->actSetProjectionXY);
    mnuProjection->addAction(sceneViewPost3D->actSetProjectionXZ);
    mnuProjection->addAction(sceneViewPost3D->actSetProjectionYZ);

    QMenu *mnuShowPanels = new QMenu(tr("Panels"), this);
    mnuShowPanels->addAction(resultsView->toggleViewAction());
    mnuShowPanels->addAction(consoleView->toggleViewAction());
    mnuShowPanels->addAction(logView->toggleViewAction());
    mnuShowPanels->addAction(tooltipView->toggleViewAction());

    mnuView = menuBar()->addMenu(tr("&View"));
    mnuView->addAction(problemWidget->actProperties);
    mnuView->addAction(sceneViewPreprocessor->actSceneModePreprocessor);
    mnuView->addAction(sceneViewPost3D->actSceneModePost3D);
    mnuView->addAction(settingsWidget->actSettings);
    mnuView->addSeparator();
    mnuView->addAction(actHideControlPanel);
    mnuView->addSeparator();
    mnuView->addAction(actSceneZoomBestFit);
    mnuView->addAction(actSceneZoomIn);
    mnuView->addAction(actSceneZoomOut);
    mnuView->addAction(actSceneZoomRegion);
    mnuView->addMenu(mnuProjection);
    mnuView->addSeparator();
    mnuView->addAction(actCopy);
    mnuView->addSeparator();
    mnuView->addMenu(mnuShowPanels);
    mnuView->addSeparator();
    mnuView->addAction(actFullScreen);

    mnuProblem = menuBar()->addMenu(tr("&Problem"));
    QMenu *mnuAdd = new QMenu(tr("&Add"), this);
    mnuProblem->addMenu(mnuAdd);
    mnuAdd->addAction(Util::scene()->actNewNode);
    mnuAdd->addSeparator();
    mnuProblem->addAction(actSolve);
    mnuProblem->addSeparator();
    mnuProblem->addAction(Util::problem()->actClearSolutions);

    mnuTools = menuBar()->addMenu(tr("&Tools"));
    mnuTools->addAction(actChart);
    mnuTools->addSeparator();
    mnuTools->addAction(actScriptEditor);
    mnuTools->addAction(actScriptEditorRunScript);
    mnuTools->addAction(actScriptEditorRunCommand);
#ifdef Q_WS_WIN
    mnuTools->addSeparator();
    mnuTools->addAction(actOptions);
#endif

    mnuHelp = menuBar()->addMenu(tr("&Help"));
    mnuHelp->addAction(actHelp);
    mnuHelp->addAction(actOnlineHelp);
    mnuHelp->addAction(actHelpShortCut);
#ifndef Q_WS_MAC
    mnuHelp->addSeparator();
#else
    mnuHelp->addAction(actOptions); // will be added to "Agros2D" MacOSX menu
    mnuHelp->addAction(actExit);    // will be added to "Agros2D" MacOSX menu
#endif
    mnuHelp->addSeparator();
    mnuHelp->addAction(actCheckVersion);
    mnuHelp->addSeparator();
    mnuHelp->addAction(actAbout);   // will be added to "Agros2D" MacOSX menu
    mnuHelp->addAction(actAboutQt); // will be added to "Agros2D" MacOSX menu
}

void MainWindow::createToolBars()
{
    // top toolbar
#ifdef Q_WS_MAC
    int iconHeight = 24;
#endif

    tlbFile = addToolBar(tr("File"));
    tlbFile->setObjectName("File");
    tlbFile->setOrientation(Qt::Horizontal);
    tlbFile->setAllowedAreas(Qt::TopToolBarArea);
    // tlbFile->setMovable(false);
#ifdef Q_WS_MAC
    tlbFile->setFixedHeight(iconHeight);
    tlbFile->setStyleSheet("QToolButton { border: 0px; padding: 0px; margin: 0px; }");
#endif
    tlbFile->addAction(actDocumentNew);
    tlbFile->addAction(actDocumentOpen);
    tlbFile->addAction(actDocumentSave);
    tlbFile->addSeparator();
    tlbFile->addAction(actHideControlPanel);

    tlbView = addToolBar(tr("View"));
    tlbView->setObjectName("View");
    tlbView->setOrientation(Qt::Horizontal);
    tlbView->setAllowedAreas(Qt::TopToolBarArea);
    // tlbView->setMovable(false);
#ifdef Q_WS_MAC
    tlbView->setFixedHeight(iconHeight);
    tlbView->setStyleSheet("QToolButton { border: 0px; padding: 0px; margin: 0px; }");
#endif

    tlbZoom = addToolBar(tr("Zoom"));
    tlbZoom->setObjectName("Zoom");
    tlbZoom->addAction(actSceneZoomBestFit);
    tlbZoom->addAction(actSceneZoomRegion);
    tlbZoom->addAction(actSceneZoomIn);
    tlbZoom->addAction(actSceneZoomOut);

    tlbGeometry = addToolBar(tr("Geometry"));
    tlbGeometry->setObjectName("Geometry");
    tlbGeometry->setOrientation(Qt::Horizontal);
    tlbGeometry->setAllowedAreas(Qt::TopToolBarArea);
    // tlbGeometry->setMovable(false);
#ifdef Q_WS_MAC
    tlbProblem->setFixedHeight(iconHeight);
    tlbProblem->setStyleSheet("QToolButton { border: 0px; padding: 0px; margin: 0px; }");
#endif
    tlbGeometry->addSeparator();
    tlbGeometry->addAction(actUndo);
    tlbGeometry->addAction(actRedo);
    tlbGeometry->addSeparator();
    tlbGeometry->addAction(sceneViewPreprocessor->actOperateOnNodes);
    tlbGeometry->addSeparator();
    tlbGeometry->addAction(sceneViewPreprocessor->actSceneViewSelectRegion);
    tlbGeometry->addAction(Util::scene()->actTransform);
    tlbGeometry->addSeparator();
    tlbGeometry->addAction(Util::scene()->actDeleteSelected);
}

void MainWindow::createMain()
{
    sceneViewInfoWidget = new SceneViewWidget(sceneInfoWidget, this);
    sceneViewBlankWidget = new SceneViewWidget(sceneViewBlank, this);
    sceneViewPreprocessorWidget = new SceneViewWidget(sceneViewPreprocessor, this);
    sceneViewPost3DWidget = new SceneViewWidget(sceneViewPost3D, this);

    tabViewLayout = new QStackedLayout();
    tabViewLayout->setContentsMargins(0, 0, 0, 0);
    tabViewLayout->addWidget(sceneViewInfoWidget);
    tabViewLayout->addWidget(sceneViewBlankWidget);
    tabViewLayout->addWidget(sceneViewPreprocessorWidget);
    tabViewLayout->addWidget(sceneViewPost3DWidget);

    QWidget *viewWidget = new QWidget();
    viewWidget->setLayout(tabViewLayout);

    tabControlsLayout = new QStackedLayout();
    tabControlsLayout->setContentsMargins(0, 0, 0, 0);
    tabControlsLayout->addWidget(problemWidget);
    tabControlsLayout->addWidget(preprocessorWidget);
    tabControlsLayout->addWidget(postprocessorWidget);
    tabControlsLayout->addWidget(settingsWidget);

    viewControls = new QWidget();
    viewControls->setLayout(tabControlsLayout);

    // spacing
    QLabel *spacing = new QLabel;
    spacing->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // left toolbar
    QToolBar *tlbLeftBar = new QToolBar();
    tlbLeftBar->setOrientation(Qt::Vertical);
    // fancy layout
    tlbLeftBar->setStyleSheet("QToolBar { border: 1px solid rgba(200, 200, 200, 255); }"
                              "QToolBar { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(70, 70, 70, 255), stop:1 rgba(120, 120, 120, 255)); }"
                              "QToolButton { border: 0px; color: rgba(230, 230, 230, 255); font: bold; font-size: 8pt; width: 65px; }"
                              "QToolButton:hover { border: 0px; background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(70, 70, 70, 255), stop:0.5 rgba(160, 160, 160, 255), stop:1 rgba(150, 150, 150, 255)); }"
                              "QToolButton:checked:hover, QToolButton:checked { border: 0px; color: rgba(30, 30, 30, 255); background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(160, 160, 160, 255), stop:0.5 rgba(220, 220, 220, 255), stop:1 rgba(160, 160, 160, 255)); }");
    // system layout
    // leftToolBar->setStyleSheet("QToolButton { font: bold; font-size: 8pt; width: 65px; }");

    tlbLeftBar->setIconSize(QSize(32, 32));
    tlbLeftBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    tlbLeftBar->addAction(problemWidget->actProperties);
    tlbLeftBar->addSeparator();
    tlbLeftBar->addAction(sceneViewPreprocessor->actSceneModePreprocessor);
    tlbLeftBar->addAction(sceneViewPost3D->actSceneModePost3D);
    tlbLeftBar->addSeparator();
    tlbLeftBar->addAction(settingsWidget->actSettings);
    tlbLeftBar->addWidget(spacing);
    tlbLeftBar->addAction(actSolve);
    tlbLeftBar->addSeparator();
    tlbLeftBar->addAction(actScriptEditor);

    splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(viewControls);
    splitter->addWidget(viewWidget);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    QList<int> sizes;
    sizes << 230 << 0;
    splitter->setSizes(sizes);

    QHBoxLayout *layoutMain = new QHBoxLayout();
    layoutMain->setContentsMargins(0, 0, 0, 0);
    layoutMain->addWidget(tlbLeftBar);
    layoutMain->addWidget(splitter);

    QWidget *main = new QWidget();
    main->setLayout(layoutMain);

    setCentralWidget(main);
}

void MainWindow::createViews()
{
    consoleView = new PythonScriptingConsoleView(currentPythonEngine(), this);
    consoleView->setAllowedAreas(Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    consoleView->setVisible(false);
    addDockWidget(Qt::RightDockWidgetArea, consoleView);

    tooltipView = new TooltipView(this);
    tooltipView->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, tooltipView);

    logView = new LogView(this);
    logView->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, logView);

    resultsView = new ResultsView(this);
    resultsView->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, resultsView);

    // tabify dock together
    tabifyDockWidget(tooltipView, logView);
    tabifyDockWidget(resultsView, consoleView);
}

void MainWindow::doMouseSceneModeChanged(MouseSceneMode mouseSceneMode)
{
    /*
    lblMouseMode->setText("Mode: -");

    switch (mouseSceneMode)
    {
    case MouseSceneMode_Add:
    {
        switch (sceneViewGeometry->sceneMode())
        {
        case SceneGeometryMode_OperateOnNodes:
            lblMouseMode->setText(tr("Mode: Add node"));
            break;
        case SceneGeometryMode_OperateOnEdges:
            lblMouseMode->setText(tr("Mode: Add edge"));
            break;
        case SceneGeometryMode_OperateOnLabels:
            lblMouseMode->setText(tr("Mode: Add label"));
            break;
        default:
            break;
        }
    }
        break;
    case MouseSceneMode_Pan:
        lblMouseMode->setText(tr("Mode: Pan"));
        break;
    case MouseSceneMode_Rotate:
        lblMouseMode->setText(tr("Mode: Rotate"));
        break;
    case MouseSceneMode_Move:
    {
        switch (sceneViewGeometry->sceneMode())
        {
        case SceneGeometryMode_OperateOnNodes:
            lblMouseMode->setText(tr("Mode: Move node"));
            break;
        case SceneGeometryMode_OperateOnEdges:
            lblMouseMode->setText(tr("Mode: Move edge"));
            break;
        case SceneGeometryMode_OperateOnLabels:
            lblMouseMode->setText(tr("Mode: Move label"));
            break;
        default:
            break;
        }
    }
        break;
    default:
        break;
    }
    */
}

void MainWindow::setRecentFiles()
{
    // recent files
    if (!Util::problem()->config()->fileName().isEmpty())
    {
        QFileInfo fileInfo(Util::problem()->config()->fileName());
        if (recentFiles.indexOf(fileInfo.absoluteFilePath()) == -1)
            recentFiles.insert(0, fileInfo.absoluteFilePath());
        else
            recentFiles.move(recentFiles.indexOf(fileInfo.absoluteFilePath()), 0);

        while (recentFiles.count() > 15) recentFiles.removeLast();
    }

    mnuRecentFiles->clear();
    for (int i = 0; i<recentFiles.count(); i++)
    {
        QAction *actMenuRecentItem = new QAction(recentFiles[i], this);
        actDocumentOpenRecentGroup->addAction(actMenuRecentItem);
        mnuRecentFiles->addAction(actMenuRecentItem);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QString fileName = QUrl(event->mimeData()->urls().at(0)).toLocalFile().trimmed();
        if (QFile::exists(fileName))
        {
            doDocumentOpen(fileName);

            event->acceptProposedAction();
        }
    }
}

void MainWindow::doDocumentNew()
{
    Util::problem()->clearFieldsAndConfig();
    Util::scene()->clear();

    // add field -  TODO
    problemWidget->actProperties->trigger();
    sceneViewPreprocessor->doZoomBestFit();
    sceneViewPost3D->doZoomBestFit();
}

void MainWindow::doDocumentOpen(const QString &fileName)
{
    QSettings settings;
    QString fileNameDocument;

    if (fileName.isEmpty())
    {
        QString dir = settings.value("General/LastProblemDir", "data").toString();

        fileNameDocument = QFileDialog::getOpenFileName(this, tr("Open file"), dir, tr("Agros2D files (*.fld *.py);;Agros2D data files (*.fld);;Python script (*.py)"));
    }
    else
    {
        fileNameDocument = fileName;
    }

    if (fileNameDocument.isEmpty()) return;

    if (QFile::exists(fileNameDocument))
    {
        QFileInfo fileInfo(fileNameDocument);
        if (fileInfo.suffix() == "fld")
        {
            // fld data file
            ErrorResult result = Util::scene()->readFromFile(fileNameDocument);
            if (!result.isError())
            {
                setRecentFiles();

                problemWidget->actProperties->trigger();
                sceneViewPreprocessor->doZoomBestFit();
                sceneViewPost3D->doZoomBestFit();

                return;
            }
            else
            {
                result.showDialog();
                return;
            }
        }
        if (fileInfo.suffix() == "py")
        {
            // python script
            scriptEditorDialog->doFileOpen(fileNameDocument);
            scriptEditorDialog->showDialog();
            return;
        }
        QMessageBox::critical(this, tr("File open"), tr("Unknown suffix."));
    }
    else
    {
        QMessageBox::critical(this, tr("File open"), tr("File '%1' is not found.").arg(fileNameDocument));
    }
}

void MainWindow::doDocumentOpenRecent(QAction *action)
{
    QString fileName = action->text();
    if (QFile::exists(fileName))
    {
        ErrorResult result = Util::scene()->readFromFile(fileName);
        if (!result.isError())
        {
            setRecentFiles();

            sceneViewPreprocessor->actOperateOnNodes->trigger();
            sceneViewPreprocessor->doZoomBestFit();
            return;
        }
        else
            result.showDialog();
    }
}

void MainWindow::doDocumentSave()
{
    if (QFile::exists(Util::problem()->config()->fileName()))
    {
        ErrorResult result = Util::scene()->writeToFile(Util::problem()->config()->fileName());
        if (result.isError())
            result.showDialog();
    }
    else
        doDocumentSaveAs();
}

void MainWindow::doDocumentSaveWithSolution()
{
    QSettings settings;

    // save state
    bool state = settings.value("Solver/SaveProblemWithSolution", false).value<bool>();
    settings.setValue("Solver/SaveProblemWithSolution", true);

    doDocumentSave();

    settings.setValue("Solver/SaveProblemWithSolution", state);
}

void MainWindow::doDocumentSaveAs()
{
    QSettings settings;
    QString dir = settings.value("General/LastProblemDir", "data").toString();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), dir, tr("Agros2D files (*.fld)"));
    if (!fileName.isEmpty())
    {
        QFileInfo fileInfo(fileName);
        if (fileInfo.suffix() != "fld") fileName += ".fld";

        ErrorResult result = Util::scene()->writeToFile(fileName);
        if (result.isError())
            result.showDialog();

        setRecentFiles();
    }
}

void MainWindow::doDocumentClose()
{
    // WILL BE FIXED
    /*
    while (!Util::scene()->undoStack()->isClean())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"), tr("Problem has been modified.\nDo you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            doDocumentSave();
        else if (ret == QMessageBox::Discard)
            break;
        else if (ret == QMessageBox::Cancel)
            return;
    }
    */

    Util::scene()->clear();

    sceneViewPreprocessor->actOperateOnNodes->trigger();
    sceneViewPreprocessor->doZoomBestFit();
    sceneViewPost3D->doZoomBestFit();
}

void MainWindow::doDocumentSaveImage()
{
    QSettings settings;
    QString dir = settings.value("General/LastImageDir").toString();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export image to file"), dir, tr("PNG files (*.png)"));
    if (!fileName.isEmpty())
    {
        QFileInfo fileInfo(fileName);
        if (fileInfo.suffix().toLower() != "png") fileName += ".png";

        ErrorResult result;
        if (sceneViewPreprocessor->actSceneModePreprocessor->isChecked())
            result = sceneViewPreprocessor->saveImageToFile(fileName);
        else if (sceneViewPost3D->actSceneModePost3D->isChecked())
            result = sceneViewPost3D->saveImageToFile(fileName);

        if (result.isError())
            result.showDialog();

        if (fileInfo.absoluteDir() != tempProblemDir())
            settings.setValue("General/LastImageDir", fileInfo.absolutePath());
    }
}

void MainWindow::doDocumentSaveGeometry()
{
    QSettings settings;
    QString dir = settings.value("General/LastImageDir").toString();


    QString selected;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export geometry to file"), dir,
                                                    tr("PDF files (*.pdf);;EPS files (*.eps);;SVG files (*.svg)"),
                                                    &selected);

    if (!fileName.isEmpty())
    {
        int format = GL2PS_PDF;
        QFileInfo fileInfo(fileName);

        if (selected == "PDF files (*.pdf)")
        {
            if (fileInfo.suffix().toLower() != "pdf") fileName += ".pdf";
            format = GL2PS_PDF;
        }
        if (selected == "EPS files (*.eps)")
        {
            if (fileInfo.suffix().toLower() != "eps") fileName += ".eps";
            format = GL2PS_EPS;
        }
        if (selected == "SVG files (*.svg)")
        {
            if (fileInfo.suffix().toLower() != "svg") fileName += ".svg";
            format = GL2PS_SVG;
        }

        ErrorResult result = sceneViewPreprocessor->saveGeometryToFile(fileName, format);
        if (result.isError())
            result.showDialog();

        if (fileInfo.absoluteDir() != tempProblemDir())
            settings.setValue("General/LastImageDir", fileInfo.absolutePath());
    }
}

void MainWindow::doSolve()
{
    LogDialog *logDialog = new LogDialog(this, tr("Solver"));
    logDialog->show();

    // solve problem
    Util::problem()->solve();
    if (Util::problem()->isSolved())
    {
        sceneViewPost3D->actSceneModePost3D->trigger();

        // show local point values
        Point point = Point(0, 0);
        resultsView->showPoint(point);

        // raise postprocessor
        postprocessorWidget->raise();

        // successful run
        logDialog->close();
    }

    setFocus();
    activateWindow();
}

void MainWindow::doFullScreen()
{
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void MainWindow::doOptions()
{
    ConfigDialog configDialog(this);
    if (configDialog.exec())
    {
        postHermes->refresh();
        setControls();
    }

    activateWindow();
}

void MainWindow::doTransform()
{
    sceneTransformDialog->showDialog();
}

void MainWindow::doChart()
{
    assert(0);
    // ChartDialog chartDialog(sceneViewPost3D, this);

    // chartDialog.showDialog();
}

void MainWindow::doScriptEditor()
{
    scriptEditorDialog->showDialog();
}

void MainWindow::doScriptEditorRunScript(const QString &fileName)
{
    QString fileNameScript;
    QSettings settings;

    if (fileName.isEmpty())
    {
        QString dir = settings.value("General/LastScriptDir").toString();

        // open dialog
        fileNameScript = QFileDialog::getOpenFileName(this, tr("Open File"), dir, tr("Python script (*.py)"));
    }
    else
    {
        fileNameScript = fileName;
    }

    if (QFile::exists(fileNameScript))
    {
        consoleView->console()->consoleMessage(tr("Run script: %1\n").
                                               arg(QFileInfo(fileNameScript).fileName().left(QFileInfo(fileNameScript).fileName().length() - 3)),
                                               Qt::gray);

        consoleView->console()->connectStdOut();
        ScriptResult result = currentPythonEngineAgros()->runScript(readFileContent(fileNameScript), fileNameScript);
        consoleView->console()->disconnectStdOut();

        if (result.isError)
            consoleView->console()->stdErr(result.text);

        consoleView->console()->appendCommandPrompt();

        QFileInfo fileInfo(fileNameScript);
        if (fileInfo.absoluteDir() != tempProblemDir())
            settings.setValue("General/LastScriptDir", fileInfo.absolutePath());
    }
    else
    {
        if (!fileNameScript.isEmpty())
            QMessageBox::critical(this, tr("File open"), tr("File '%1' doesn't exists.").arg(fileNameScript));
    }
}

void MainWindow::doScriptEditorRunCommand()
{
    consoleView->show();
    consoleView->activateWindow();
}

void MainWindow::doCut()
{

}

void MainWindow::doCopy()
{
    // copy image to clipboard
    QPixmap pixmap;
    if (sceneViewPreprocessor->actSceneModePreprocessor->isChecked())
        pixmap = sceneViewPreprocessor->renderScenePixmap();
    else if (sceneViewPost3D->actSceneModePost3D->isChecked())
        pixmap = sceneViewPost3D->renderScenePixmap();

    QApplication::clipboard()->setImage(pixmap.toImage());
}

void MainWindow::doPaste()
{

}

void MainWindow::clear()
{
    sceneViewPreprocessor->actSceneModePreprocessor->trigger();

    setControls();
}

void MainWindow::setControls()
{
    // set controls
    Util::scene()->actTransform->setEnabled(false);

    sceneViewPreprocessor->actSceneZoomRegion = NULL;
    sceneViewPost3D->actSceneZoomRegion = NULL;

    tlbGeometry->setVisible(sceneViewPreprocessor->actSceneModePreprocessor->isChecked());
    bool showZoom = sceneViewPreprocessor->actSceneModePreprocessor->isChecked() ||
            sceneViewPost3D->actSceneModePost3D->isChecked();
    tlbZoom->setVisible(showZoom);
    actSceneZoomIn->setVisible(showZoom);
    actSceneZoomOut->setVisible(showZoom);
    actSceneZoomBestFit->setVisible(showZoom);
    actSceneZoomRegion->setVisible(showZoom);

    // disconnect signals
    actSceneZoomIn->disconnect();
    actSceneZoomOut->disconnect();
    actSceneZoomOut->disconnect();

    if (problemWidget->actProperties->isChecked())
    {
        tabViewLayout->setCurrentWidget(sceneViewInfoWidget);
        tabControlsLayout->setCurrentWidget(problemWidget);
    }
    if (sceneViewPreprocessor->actSceneModePreprocessor->isChecked())
    {
        tabViewLayout->setCurrentWidget(sceneViewPreprocessorWidget);
        tabControlsLayout->setCurrentWidget(preprocessorWidget);

        Util::scene()->actTransform->setEnabled(true);

        connect(actSceneZoomIn, SIGNAL(triggered()), sceneViewPreprocessor, SLOT(doZoomIn()));
        connect(actSceneZoomOut, SIGNAL(triggered()), sceneViewPreprocessor, SLOT(doZoomOut()));
        connect(actSceneZoomBestFit, SIGNAL(triggered()), sceneViewPreprocessor, SLOT(doZoomBestFit()));
        sceneViewPreprocessor->actSceneZoomRegion = actSceneZoomRegion;
    }
    if (sceneViewPost3D->actSceneModePost3D->isChecked())
    {
        tabViewLayout->setCurrentWidget(sceneViewPost3DWidget);
        tabControlsLayout->setCurrentWidget(postprocessorWidget);

        connect(actSceneZoomIn, SIGNAL(triggered()), sceneViewPost3D, SLOT(doZoomIn()));
        connect(actSceneZoomOut, SIGNAL(triggered()), sceneViewPost3D, SLOT(doZoomOut()));
        connect(actSceneZoomBestFit, SIGNAL(triggered()), sceneViewPost3D, SLOT(doZoomBestFit()));

        // hide transform dialog
        sceneTransformDialog->hide();
    }
    if (settingsWidget->actSettings->isChecked())
    {
        tabViewLayout->setCurrentWidget(sceneViewBlankWidget);
        tabControlsLayout->setCurrentWidget(settingsWidget);
    }

    actChart->setEnabled(Util::problem()->isSolved());

    QTimer::singleShot(0, postprocessorWidget, SLOT(updateControls()));

    // set current timestep
    //    cmbTimeStep->setCurrentIndex(Util::problem()->timeStep());

    //actProgressLog->setEnabled(Util::config()->enabledProgressLog);
    //actApplicationLog->setEnabled(Util::config()->enabledApplicationLog);
}

void MainWindow::doPostprocessorModeGroupChanged(SceneModePostprocessor sceneModePostprocessor)
{
    //resultsView->raise();
    if (sceneModePostprocessor == SceneModePostprocessor_LocalValue)
        resultsView->showPoint();
}

void MainWindow::doHelp()
{
    showPage("index.html");
}

void MainWindow::doHelpShortCut()
{
    showPage("getting_started/shortcut_keys.html");
}

void MainWindow::doOnlineHelp()
{
    QDesktopServices::openUrl(QUrl("http://hpfem.org/agros2d/help"));
}

void MainWindow::doCheckVersion()
{
    checkForNewVersion();
}

void MainWindow::doAbout()
{
    AboutDialog about(this);
    about.exec();
}

void MainWindow::doHideControlPanel()
{
    viewControls->setVisible(actHideControlPanel->isChecked());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // WILL BE FIXED
    /*
    if (!Util::scene()->undoStack()->isClean())
        doDocumentClose();

    if (Util::scene()->undoStack()->isClean() && !scriptEditorDialog->isScriptModified())
        event->accept();
    else
    {
        event->ignore();
        if (scriptEditorDialog->isScriptModified()) scriptEditorDialog->show();
    }
    */

    // check script editor
    scriptEditorDialog->closeTabs();

    if (!scriptEditorDialog->isScriptModified())
        event->accept();
    else
    {
        event->ignore();
        // show script editor
        if (scriptEditorDialog->isScriptModified())
            scriptEditorDialog->show();
    }
}
