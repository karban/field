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

#include "logview.h"
#include "scene.h"

#include "gui/chart.h"

#include "field/problem.h"

Log::Log()
{
}

// *******************************************************************************************************

LogWidget::LogWidget(QWidget *parent) : QTextEdit(parent)
{
    setReadOnly(true);
    setMinimumSize(160, 160);

    createActions();

    // context menu
    mnuInfo = new QMenu(this);
    mnuInfo->addAction(actShowTimestamp);
#ifndef QT_NO_DEBUG_OUTPUT
    mnuInfo->addAction(actShowDebug);
#endif
    mnuInfo->addSeparator();
    mnuInfo->addAction(actCopy);
    mnuInfo->addAction(actClear);

    connect(Util::log(), SIGNAL(messageMsg(QString, QString, bool)), this, SLOT(printMessage(QString, QString, bool)));
    connect(Util::log(), SIGNAL(errorMsg(QString, QString, bool)), this, SLOT(printError(QString, QString, bool)));
    connect(Util::log(), SIGNAL(warningMsg(QString, QString, bool)), this, SLOT(printWarning(QString, QString, bool)));
    connect(Util::log(), SIGNAL(debugMsg(QString, QString, bool)), this, SLOT(printDebug(QString, QString, bool)));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu(const QPoint &)));
}

void LogWidget::contextMenu(const QPoint &pos)
{
    mnuInfo->exec(QCursor::pos());
}

void LogWidget::createActions()
{
    QSettings settings;

    actShowTimestamp = new QAction(icon(""), tr("Show timestamp"), this);
    actShowTimestamp->setCheckable(true);
    actShowTimestamp->setChecked(settings.value("LogWidget/ShowTimestamp", true).toBool());
    connect(actShowTimestamp, SIGNAL(triggered()), this, SLOT(showTimestamp()));

    actShowDebug = new QAction(icon(""), tr("Show debug"), this);
    actShowDebug->setCheckable(true);
    actShowDebug->setChecked(settings.value("LogWidget/ShowDebug", true).toBool());
    connect(actShowDebug, SIGNAL(triggered()), this, SLOT(showDebug()));

    actClear = new QAction(icon(""), tr("Clear"), this);
    connect(actClear, SIGNAL(triggered()), this, SLOT(clear()));

    actCopy = new QAction(icon(""), tr("Copy"), this);
    connect(actCopy, SIGNAL(triggered()), this, SLOT(copy()));
}

void LogWidget::showTimestamp()
{
    QSettings settings;
    settings.setValue("LogWidget/ShowTimestamp", actShowTimestamp->isChecked());
}

void LogWidget::showDebug()
{
    QSettings settings;
    settings.setValue("LogWidget/ShowDebug", actShowDebug->isChecked());
}

void LogWidget::printMessage(const QString &module, const QString &message, bool escaped)
{
    print(module, message, "black", escaped);
}

void LogWidget::printError(const QString &module, const QString &message, bool escaped)
{
    print(module, message, "red", escaped);
}

void LogWidget::printWarning(const QString &module, const QString &message, bool escaped)
{
    print(module, message, "blue", escaped);
}

void LogWidget::printDebug(const QString &module, const QString &message, bool escaped)
{
#ifndef QT_NO_DEBUG_OUTPUT
    if (actShowDebug->isChecked())
        print(module, message, "gray", escaped);
#endif
}

void LogWidget::print(const QString &module, const QString &message, const QString &color, bool escaped)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);

    append(QString());

    QString str;

    // debug - timestamp
    if (actShowTimestamp->isChecked())
    {
        str += "<span style=\"color: gray;\">";
        // str += Qt::escape(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") + ": ");
        str += Qt::escape(QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + ": ");
        str += "</span>";
    }

    // message
    if (!color.isEmpty())
        str += "<span style=\"color: " + color + ";\">";
    str += "<strong>" + Qt::escape(module) + "</strong>: ";
    if (escaped)
        str += Qt::escape(message);
    else
        str += message;
    if (!color.isEmpty())
        str += "</span>";

    insertHtml(str);

    repaint();
}

void LogWidget::welcomeMessage()
{
    print("Agros2D", tr("version: %1").arg(QApplication::applicationVersion()), "green");
}

// *******************************************************************************************************

LogView::LogView(QWidget *parent) : QDockWidget(tr("Applicaton log"), parent)
{
    setObjectName("LogView");

    logWidget = new LogWidget(this);
    logWidget->welcomeMessage();

    setWidget(logWidget);
}

// *******************************************************************************************************

LogDialog::LogDialog(QWidget *parent, const QString &title) : QDialog(parent)
{
    setModal(true);

    setWindowIcon(icon("run"));
    setWindowTitle(title);
    setAttribute(Qt::WA_DeleteOnClose);

    createControls();

    setMinimumSize(550, 250);

    QSettings settings;
    restoreGeometry(settings.value("LogDialog/Geometry", saveGeometry()).toByteArray());
}

LogDialog::~LogDialog()
{
    QSettings settings;
    settings.setValue("LogDialog/Geometry", saveGeometry());
}

void LogDialog::createControls()
{    
    logWidget = new LogWidget(this);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    QHBoxLayout *layoutHorizontal = new QHBoxLayout();
    layoutHorizontal->addWidget(logWidget, 1);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(layoutHorizontal);
    layout->addWidget(buttonBox);

    setLayout(layout);
}

// *******************************************************************************************

LogStdOut::LogStdOut(QWidget *parent) : QObject(parent)
{
    connect(Util::log(), SIGNAL(messageMsg(QString, QString, bool)), this, SLOT(printMessage(QString, QString, bool)));
    connect(Util::log(), SIGNAL(errorMsg(QString, QString, bool)), this, SLOT(printError(QString, QString, bool)));
    connect(Util::log(), SIGNAL(warningMsg(QString, QString, bool)), this, SLOT(printWarning(QString, QString, bool)));
    connect(Util::log(), SIGNAL(debugMsg(QString, QString, bool)), this, SLOT(printDebug(QString, QString, bool)));
}

void LogStdOut::printMessage(const QString &module, const QString &message, bool escaped)
{
    qWarning() << QString("%1: %2").arg(module).arg(message);
}

void LogStdOut::printError(const QString &module, const QString &message, bool escaped)
{
    qCritical() << QString("%1: %2").arg(module).arg(message);
}

void LogStdOut::printWarning(const QString &module, const QString &message, bool escaped)
{
    qWarning() << QString("%1: %2").arg(module).arg(message);
}

void LogStdOut::printDebug(const QString &module, const QString &message, bool escaped)
{
    qDebug() << QString("%1: %2").arg(module).arg(message);
}

