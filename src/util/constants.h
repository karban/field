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

#ifndef UTIL_CONSTANTS_H
#define UTIL_CONSTANTS_H

#include <QtCore>

// constants
const QString IMAGEROOT = QString("..%1resources_source%1images").arg(QDir::separator());
const QString LANGUAGEROOT = QString("%1/resources%1lang").arg(QDir::separator());
const QString XSDROOT = QString("%1resources%1xsd").arg(QDir::separator());
const QString MODULEROOT = QString("%1resources%1modules").arg(QDir::separator());
const QString COUPLINGROOT = QString("%1resources%1couplings").arg(QDir::separator());
const QString TEMPLATEROOT = QString("%1resources%1templates").arg(QDir::separator());

const QColor COLORBACKGROUND = QColor::fromRgb(255, 255, 255);
const QColor COLORGRID = QColor::fromRgb(200, 200, 200);
const QColor COLORCROSS = QColor::fromRgb(150, 150, 150);
const QColor COLORNODES = QColor::fromRgb(150, 0, 0);
const QColor COLOREDGES = QColor::fromRgb(0, 0, 150);
const QColor COLORLABELS = QColor::fromRgb(0, 150, 0);
const QColor COLORCONTOURS = QColor::fromRgb(0, 0, 0);
const QColor COLORVECTORS = QColor::fromRgb(0, 0, 0);
const QColor COLORINITIALMESH = QColor::fromRgb(250, 202, 119);
const QColor COLORSOLUTIONMESH = QColor::fromRgb(150, 70, 0);
const QColor COLORHIGHLIGHTED = QColor::fromRgb(250, 150, 0);
const QColor COLORSELECTED = QColor::fromRgb(150, 0, 0);
const QColor COLORCROSSED = QColor::fromRgb(255, 0, 0);
const QColor COLORNOTCONNECTED = QColor::fromRgb(255, 0, 0);

// workspace
const double GRIDSTEP = 0.05;
const bool SHOWGRID = true;
const bool SNAPTOGRID = true;

#ifdef Q_WS_X11
    const QFont FONT = QFont("Monospace", 9);
#endif
#ifdef Q_WS_WIN
    const QFont FONT = QFont("Courier New", 9);
#endif
#ifdef Q_WS_MAC
    const QFont FONT = QFont("Monaco", 12);
#endif

const bool SHOWAXES = true;
const bool SHOWRULERS = true;
const bool ZOOMTOMOUSE = true;

const int GEOMETRYNODESIZE = 6;
const int GEOMETRYEDGEWIDTH = 2;
const int GEOMETRYLABELSIZE = 6;

// mesh
const bool SHOWINITIALMESHVIEW = true;
const bool SHOWSOLUTIONMESHVIEW = false;
const int MESHANGLESEGMENTSCOUNT = 3;
const bool MESHCURVILINEARELEMENTS = true;

// post2d
const int PALETTESTEPSMIN = 5;
const int PALETTESTEPSMAX = 100;
const int SCALARDECIMALPLACEMIN = 0;
const int SCALARDECIMALPLACEMAX = 10;
const int CONTOURSCOUNTMIN = 1;
const int CONTOURSCOUNTMAX = 100;
const int VECTORSCOUNTMIN = 1;
const int VECTORSCOUNTMAX = 500;
const double VECTORSSCALEMIN = 0.1;
const double VECTORSSCALEMAX = 20.0;

// post3d
const SceneViewPost3DMode SCALARSHOWPOST3D = SceneViewPost3DMode_ScalarView3D;

const bool SHOWCONTOURVIEW = false;
const int CONTOURSCOUNT = 15;

const PaletteType PALETTETYPE = Palette_Agros2D;
const bool PALETTEFILTER = false;
const int PALETTESTEPS = 30;

// scalarview
const bool SHOWSCALARVIEW = true;
const bool SHOWSCALARCOLORBAR = true;
const bool SCALARFIELDRANGELOG = false;
const int SCALARFIELDRANGEBASE = 10;
const int SCALARDECIMALPLACE = 2;
const bool SCALARRANGEAUTO = true;
const double SCALARRANGEMIN = 0.0;
const double SCALARRANGEMAX = 1.0;

const bool VIEW3DLIGHTING = false;
const double VIEW3DANGLE = 230.0;
const bool VIEW3DBACKGROUND = true;

// command argument
const QString COMMANDS_TRIANGLE = "%1 -p -P -q31.0 -e -A -a -z -Q -I -n -o2 \"%2\"";

// max dofs
const int MAX_DOFS = 60e3;

#endif // UTIL_CONSTANTS_H

