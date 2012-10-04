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

#ifndef PYTHONLABAGROS_H
#define PYTHONLABAGROS_H

#include "pythonlab/pythonconsole.h"
#include "pythonlab/pythonengine.h"
#include "pythonlab/pythoneditor.h"

#include "util.h"
#include "scene.h"
#include "field/problem.h"

class Solution;
class SceneViewPreprocessor;
class SceneViewPost3D;
class PostView;

class PythonEngineField : public PythonEngine
{
    Q_OBJECT
public:
    PythonEngineField() : PythonEngine(),
        m_sceneViewPreprocessor(NULL), m_sceneViewPost3D(NULL) {}

    inline void setSceneViewGeometry(SceneViewPreprocessor *sceneViewGeometry) { assert(sceneViewGeometry); m_sceneViewPreprocessor = sceneViewGeometry; }
    inline SceneViewPreprocessor *sceneViewPreprocessor() { assert(m_sceneViewPreprocessor); return m_sceneViewPreprocessor; }
    inline void setSceneViewPost3D(SceneViewPost3D *sceneViewPost3D) { assert(sceneViewPost3D); m_sceneViewPost3D = sceneViewPost3D; }
    inline SceneViewPost3D *sceneViewPost3D() { assert(m_sceneViewPost3D); return m_sceneViewPost3D; }
    inline void setPostHermes(PostView *postHermes) { assert(postHermes); m_postHermes = postHermes; }
    inline PostView *postHermes() { assert(m_postHermes); return m_postHermes; }

protected:
    virtual void addCustomExtensions();
    virtual void runPythonHeader();

private:
    SceneViewPreprocessor *m_sceneViewPreprocessor;
    SceneViewPost3D *m_sceneViewPost3D;

    PostView *m_postHermes;
};

class PythonLabAgros : public PythonEditorDialog
{
    Q_OBJECT
public:
    PythonLabAgros(PythonEngine *pythonEngine, QStringList args, QWidget *parent);

private:
    QAction *actCreateFromModel;

private slots:
    void doCreatePythonFromModel();
};

// current python engine agros
PythonEngineField *currentPythonEngineAgros();

QString createPythonFromModel();

// ************************************************************************************

// problem
class PyProblem
{
    public:
        PyProblem(bool clearproblem);
        ~PyProblem() {}

        // clear
        void clear();
        void field(char *fieldId);

        // refresh
        void refresh();

        // name
        inline const char *getName() { return Util::problem()->config()->name().toStdString().c_str(); }
        void setName(const char *name) { Util::problem()->config()->setName(QString(name)); }

        void solve();
};

// functions
char *pyVersion();
void pyQuit();

char *pyInput(char *str);
void pyMessage(char *str);

void pyOpenDocument(char *str);
void pySaveDocument(char *str);
void pyCloseDocument();

void pySaveImage(char *str, int w, int h);

#endif // PYTHONLABAGROS_H
