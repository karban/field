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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "util.h"

class PostView;
class SceneViewPost3D;
class SceneViewPreprocessor;
class InfoWidget;
class SettingsWidget;
class ProblemWidget;
class ResultsView;
class VolumeIntegralValueView;
class SurfaceIntegralValueView;
class PreprocessorWidget;
class PostprocessorWidget;
class PythonScriptingConsoleView;
class TooltipView;
class LogView;

class ChartDialog;
class PythonLabAgros;
class ReportDialog;
class ServerDownloadDialog;
class SceneTransformDialog;
class SceneViewWidget;
class LogStdOut;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void open(const QString &fileName);

private slots:
    inline void doSetWindowTitle(const QString &name) { setWindowTitle("Agros2D - " + name); }

    void doDocumentNew();
    void doDocumentOpen(const QString &fileName = "");
    void doDocumentOpenRecent(QAction *action);
    void doDocumentSave();
    void doDocumentSaveWithSolution();
    void doDocumentSaveAs();
    void doDocumentClose();
    void doDocumentSaveImage();
    void doDocumentSaveGeometry();

    void doMouseSceneModeChanged(MouseSceneMode mouseSceneMode);

    void doSolve();

    void doChart();
    void doScriptEditor();
    void doScriptEditorRunScript(const QString &fileName = "");
    void doScriptEditorRunCommand();
    void doOptions();
    void doTransform();

    void doHideControlPanel();
    void doFullScreen();

    void doCut();
    void doCopy();
    void doPaste();

    void doHelp();
    void doHelpShortCut();
    void doOnlineHelp();
    void doCheckVersion();
    void doAbout();
    void setControls();
    void clear();
    void doPostprocessorModeGroupChanged(SceneModePostprocessor sceneModePostprocessor);

    /// fields adeed or removed, menus need to be modified
    void doFieldsChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    QStringList recentFiles;

    QMenu *mnuFile;
    QMenu *mnuFileImportExport;
    QMenu *mnuRecentFiles;
    QMenu *mnuEdit;
    QMenu *mnuView;
    QMenu *mnuProblem;
    QMenu *mnuTools;
    QMenu *mnuHelp;

    QToolBar *tlbFile;
    QToolBar *tlbEdit;
    QToolBar *tlbView;
    QToolBar *tlbZoom;
    QToolBar *tlbGeometry;
    QToolBar *tlbPost2D;

    QAction *actDocumentNew;
    QAction *actDocumentOpen;
    QAction *actDocumentSave;
    QAction *actDocumentSaveWithSolution;
    QAction *actDocumentSaveAs;
    QAction *actDocumentClose;
    QAction *actDocumentSaveImage;
    QAction *actDocumentSaveGeometry;
    QAction *actExit;
    QActionGroup *actDocumentOpenRecentGroup;

    QAction *actHideControlPanel;

    QAction *actUndo;
    QAction *actRedo;
    QAction *actCopy;

    QAction *actOptions;
    QAction *actSolve;
    QAction *actChart;
    QAction *actFullScreen;
    QAction *actPostprocessorView;

    QAction *actScriptEditor;
    QAction *actScriptEditorRunScript;
    QAction *actScriptEditorRunCommand;

    QAction *actHelp;
    QAction *actHelpShortCut;
    QAction *actOnlineHelp;
    QAction *actCheckVersion;
    QAction *actAbout;
    QAction *actAboutQt;

    QComboBox *cmbTimeStep;

    // pointers to actions (geometry, post2d and post3d)
    QAction *actSceneZoomIn;
    QAction *actSceneZoomOut;
    QAction *actSceneZoomBestFit;
    QAction *actSceneZoomRegion;

    // scene mode
    QActionGroup *actSceneModeGroup;

    SceneViewWidget *sceneViewInfoWidget;
    SceneViewWidget *sceneViewPreprocessorWidget;
    SceneViewWidget *sceneViewMeshWidget;
    SceneViewWidget *sceneViewPost2DWidget;
    SceneViewWidget *sceneViewPost3DWidget;
    SceneViewWidget *sceneViewBlankWidget;

    PostView *postHermes;

    QStackedLayout *tabViewLayout;
    InfoWidget *sceneInfoWidget;
    SceneViewPreprocessor *sceneViewPreprocessor;
    SceneViewPost3D *sceneViewPost3D;
    QWidget *sceneViewBlank;

    QWidget *viewControls;
    QStackedLayout *tabControlsLayout;
    PreprocessorWidget *preprocessorWidget;
    PostprocessorWidget *postprocessorWidget;
    SettingsWidget *settingsWidget;
    ProblemWidget *problemWidget;

    ResultsView *resultsView;
    PythonScriptingConsoleView *consoleView;
    TooltipView *tooltipView;
    LogView *logView;

    PythonLabAgros *scriptEditorDialog;
    SceneTransformDialog *sceneTransformDialog;

    QSplitter *splitter;

    void setRecentFiles();

    void createActions();
    void createToolBox();
    void createMenus();
    void createToolBars();
    void createMain();
    void createViews();   
};

#endif // MAINWINDOW_H
