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

#include "chart.h"

#include "scene.h"
#include "util.h"

#include "field/problem.h"

#include <qwt_scale_map.h>
#include <qwt_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_panner.h>
#include <qwt_counter.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_renderer.h>
#include "qwt_plot_magnifier.h"
#include "qwt_plot_rescaler.h"

Chart::Chart(QWidget *parent, bool showPicker) : QwtPlot(parent)
{
    //  chart style
    setAutoReplot(false);
    setTitle("");
    setMinimumSize(420, 260);

    // panning with the left mouse button
    // QwtPlotPanner *panner = new QwtPlotPanner(canvas());
    // panner->setMouseButton(Qt::MidButton);

    // zoom in/out with the wheel
    // QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(canvas());
    // magnifier->setWheelFactor(1 / magnifier->wheelFactor());
    // magnifier->setMouseFactor(1 / magnifier->mouseFactor());

    // QwtPlotZoomer *zoomer = new QwtPlotZoomer(canvas());
    // zoomer->setRubberBandPen(QColor(Qt::red));
    // zoomer->setTrackerPen(Qt::NoPen);
    // zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    // zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

    // legend
    /*
    QwtLegend *legend = new QwtLegend;
    legend->setFrameStyle(QFrame::Box | QFrame::Sunken);
    insertLegend(legend, QwtPlot::BottomLegend);
    */

    // canvas
    QPalette canvasPalette(Qt::white);
    canvasPalette.setColor(QPalette::Foreground, QColor(0, 0, 0));
    canvas()->setPalette(canvasPalette);
    canvas()->setFrameShape(QFrame::NoFrame);

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->setMajPen(QPen(Qt::gray, 0, Qt::DotLine));
    grid->setMinPen(QPen(Qt::NoPen));
    grid->enableX(true);
    grid->enableY(true);
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->attach(this);

    // axes
    QFont fnt = QFont(QApplication::font().family(), QApplication::font().pointSize() - 1, QFont::Normal);
    setAxisFont(QwtPlot::xBottom, fnt);
    setAxisTitle(QwtPlot::xBottom, QwtText(" "));
    setAxisFont(QwtPlot::yLeft, fnt);
    setAxisTitle(QwtPlot::yLeft, QwtText(" "));

    // curve styles
    QwtSymbol sym;

    sym.setStyle(QwtSymbol::Cross);
    sym.setPen(QColor(Qt::black));
    sym.setSize(5);

    // curve
    m_curve = new QwtPlotCurve();
    m_curve->setRenderHint(QwtPlotCurve::RenderAntialiased);
    m_curve->setPen(QPen(Qt::darkBlue, 1.5));
    m_curve->setCurveAttribute(QwtPlotCurve::Inverted);
    m_curve->setYAxis(QwtPlot::yLeft);
    m_curve->attach(this);


    // chart picker
    QwtPlotPicker *pickerValue = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                                                   QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                                   canvas());
    pickerValue->setRubberBandPen(QColor(Qt::green));
    pickerValue->setRubberBand(QwtPicker::CrossRubberBand);
    pickerValue->setTrackerMode(QwtPicker::AlwaysOn);
    pickerValue->setTrackerPen(QColor(Qt::black));

    connect(pickerValue, SIGNAL(moved(const QPoint &)), SLOT(pickerValueMoved(const QPoint &)));

    setMinimumSize(sizeHint());
}

Chart::~Chart()
{
    delete m_curve;
}

void Chart::saveImage(const QString &fileName)
{
    QSettings settings;
    QString dir = settings.value("General/LastImageDir").toString();

    QString fileNameTemp;
    if (fileName.isEmpty())
    {
        const QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();

        QStringList filter;
        filter += "PDF Documents (*.pdf)";
#ifndef QWT_NO_SVG
        filter += "SVG Documents (*.svg)";
#endif
        filter += "Postscript Documents (*.ps)";

        if (imageFormats.size() > 0)
        {
            QString imageFilter("Images (");
            for ( int i = 0; i < imageFormats.size(); i++ )
            {
                if ( i > 0 )
                    imageFilter += " ";
                imageFilter += "*.";
                imageFilter += imageFormats[i];
            }
            imageFilter += ")";

            filter += imageFilter;
        }
        fileNameTemp = QFileDialog::getSaveFileName(this, tr("Export image to file"), dir, filter.join(";;"));

        QFileInfo fileInfo(fileNameTemp);
        if (!fileNameTemp.isEmpty() && fileInfo.absoluteDir() != tempProblemDir())
            settings.setValue("General/LastImageDir", fileInfo.absolutePath());
    }
    else
    {
        fileNameTemp = fileName;
    }

    if (!fileNameTemp.isEmpty())
    {
        // QFileInfo fileInfo(fileNameTemp);
        // if (fileInfo.suffix().toLower() != "png") fileNameTemp += ".png";

        QwtPlotRenderer renderer;

        renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, false);
        renderer.setLayoutFlag(QwtPlotRenderer::KeepFrames, true);

        renderer.renderDocument(this, fileNameTemp, QSizeF(300, 200), 85);
    }
}

void Chart::setData(double *xval, double *yval, int count)
{
    const bool doReplot = autoReplot();
    setAutoReplot(false);

    m_curve->setSamples(xval, yval, count);

    setAutoReplot(doReplot);

    replot();
}

void Chart::setData(QList<double> xval, QList<double> yval)
{
    double *txval = new double[xval.count()];
    double *tyval = new double[xval.count()];

    for (int i = 0; i < xval.count(); i++)
    {
        txval[i] = xval[i];
        tyval[i] = yval[i];
    }

    setData(txval, tyval, xval.count());

    delete [] txval;
    delete [] tyval;

    replot();
}

void Chart::pickerValueMoved(const QPoint &pos)
{
    QString info;
    info.sprintf("x=%g, y=%g",
                 invTransform(QwtPlot::xBottom, pos.x()),
                 invTransform(QwtPlot::yLeft, pos.y()));
}
