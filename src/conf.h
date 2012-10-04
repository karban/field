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

#ifndef CONFIG_H
#define CONFIG_H

#include "util.h"

class Config : public QObject
{
    Q_OBJECT

public:
    Config();
    ~Config();

    // general
    QString guiStyle;
    QString language;
    QString defaultPhysicField;

    bool checkVersion;
    bool lineEditValueShowResult;
    bool saveProblemWithSolution;

    // geometry
    double nodeSize;

    // font
    QFont sceneFont;

    // zoom
    bool zoomToMouse;

    // axes
    bool showAxes;

    QString activeField;

    // post3d
    SceneViewPost3DMode showPost3D;

    // palette
    bool showScalarColorBar;
    PaletteType paletteType;
    int paletteSteps;
    bool paletteFilter;

    // colors
    QColor colorBackground;
    QColor colorGrid;
    QColor colorCross;
    QColor colorNodes;
    QColor colorEdges;
    QColor colorLabels;
    QColor colorContours;
    QColor colorVectors;
    QColor colorInitialMesh;
    QColor colorSolutionMesh;
    QColor colorHighlighted;
    QColor colorCrossed;
    QColor colorSelected;
    QColor colorNotConnected;

    // 3d
    bool scalarView3DLighting;
    double scalarView3DAngle;
    bool scalarView3DBackground;

    // global script
    QString globalScript;

    void load();
    void loadWorkspace();
    void loadPostprocessor(QDomElement *config);
    void loadAdvanced();

    void save();
    void saveWorkspace();
    void savePostprocessor(QDomElement *config);
    void saveAdvanced();

private:
    QDomElement *eleConfig;

    bool readConfig(const QString &key, bool defaultValue);
    int readConfig(const QString &key, int defaultValue);
    double readConfig(const QString &key, double defaultValue);
    QString readConfig(const QString &key, const QString &defaultValue);
    void writeConfig(const QString &key, bool value);
    void writeConfig(const QString &key, int value);
    void writeConfig(const QString &key, double value);
    void writeConfig(const QString &key, const QString &value);   
};

#endif // CONFIG_H
