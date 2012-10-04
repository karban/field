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

#ifndef SCENE_H
#define SCENE_H

#include "util.h"
#include "conf.h"

class Problem;

class Scene;
class SceneNode;
struct SceneViewSettings;

class SceneNodeContainer;

class ScriptEngineRemote;

class ProblemWidget;
class SceneTransformDialog;

class Log;

class Scene : public QObject
{
    Q_OBJECT

public slots:
    void doNewNode(const Point3 &point = Point3());
    void doDeleteSelected();

signals:
    void invalidated();
    void cleared();

    void defaultValues();
    void fileNameChanged(const QString &fileName);

public:
    Scene();
    ~Scene();

    SceneNodeContainer* nodes;

    QAction *actNewNode;
    QAction *actDeleteSelected;

    QAction *actTransform;

    void clear();

    SceneNode *addNode(SceneNode *node);
    void removeNode(SceneNode *node);
    SceneNode *getNode(const Point3 &point);

    CubePoint boundingBox() const;

    void selectNone();
    void selectAll(SceneGeometryMode sceneMode);
    int selectedCount();
    void deleteSelected();

    void highlightNone();
    int highlightedCount();

    void transformTranslate(const Point3 &point, bool copy);
    void transformRotateXY(const Point &point, double angle, bool copy);
    void transformRotateYZ(const Point &point, double angle, bool copy) {} // TODO
    void transformRotateZX(const Point &point, double angle, bool copy) {} // TODO
    void transformScale(const Point3 &point, double scaleFactor, bool copy);

    inline void invalidate() { emit invalidated(); }

    ErrorResult readFromFile(const QString &fileName);
    ErrorResult writeToFile(const QString &fileName);

    inline QUndoStack *undoStack() const { return m_undoStack; }

private:
    QUndoStack *m_undoStack;

    void createActions();

private slots:
    void doInvalidated();
};

// **************************************************************************************

class Util
{
public:
    static void createSingleton();
    static Util* singleton();
    static inline Scene *scene() { return Util::singleton()->m_scene; }
    static inline Config *config() { return Util::singleton()->m_config; }
    static inline Problem *problem() { return Util::singleton()->m_problem; }
    static inline Log *log() { return Util::singleton()->m_log; }

protected:
    Util();
    Util(const Util &);
    Util & operator = (const Util &);
    ~Util();

private:
    static Util *m_singleton;

    Scene *m_scene;
    Config *m_config;
    Problem *m_problem;
    ScriptEngineRemote *m_scriptEngineRemote;
    Log *m_log;
};

#endif /* SCENE_H */
