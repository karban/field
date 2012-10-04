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

#ifndef SCENEBASIC_H
#define SCENEBASIC_H

#include "util.h"

class LineEditDouble;
class ValueLineEdit;

struct Point;

class SceneBasic;
class SceneNode;

Q_DECLARE_METATYPE(SceneBasic *)
Q_DECLARE_METATYPE(SceneNode *)

class SceneBasic 
{

public:
    SceneBasic();

    void setSelected(bool value = true) { m_isSelected = value; }
    inline bool isSelected() const { return m_isSelected; }

    void setHighlighted(bool value = true) { m_isHighlighted = value; }
    inline bool isHighlighted() const { return m_isHighlighted; }

    virtual int showDialog(QWidget *parent, bool isNew = false) = 0;

    QVariant variant();

private:
    bool m_isSelected;
    bool m_isHighlighted;
};

template <typename BasicType>
class SceneBasicContainer
{
public:
    /// items() should be removed step by step from the code.
    /// more methods operating with list data should be defined here
    QList<BasicType*> items() { return data; }

    bool add(BasicType *item);
    virtual bool remove(BasicType *item);
    BasicType *at(int i);
    inline int length() { return data.length(); }
    inline int count() {return length(); }
    inline int isEmpty() { return data.isEmpty(); }
    void clear();

    /// selects or unselects all items
    void setSelected(bool value = true);

    /// highlights or unhighlights all items
    void setHighlighted(bool value = true);

    void deleteWithUndo(QString message);

protected:
    QList<BasicType*> data;

    QString containerName;
};


// *************************************************************************************************************************************

template <typename MarkerType>
class MarkedSceneBasic : public SceneBasic
{
public:
    MarkedSceneBasic() {}
    ~MarkedSceneBasic() {}

    /// gets marker that corresponds to the given field

    void doFieldsChanged();
};


// *************************************************************************************************************************************

class SceneBasicDialog : public QDialog
{
    Q_OBJECT

public:
    SceneBasicDialog(QWidget *parent, bool isNew = false);

protected:
    bool m_isNew;
    SceneBasic *m_object;
    QDialogButtonBox *buttonBox;

    virtual QLayout *createContent() = 0;
    void createControls();

    virtual bool load() = 0;
    virtual bool save() = 0;

protected slots:
    void evaluated(bool isError);

private:    

    QVBoxLayout *layout;

private slots:
    void doAccept();
    void doReject();
};


// *************************************************************************************************************************************

//TODO general undo framework should reduce code repetition.... TODO

template <typename BasicType>
class SceneBasicTrace
{
public:
    virtual void save(const BasicType& original) = 0;
    virtual void load(BasicType& destination) const = 0;
    virtual void remove() const = 0;
};

template <typename MarkedBasicType>
class MarkedSceneBasicTrace
{
    void saveMarkers(const MarkedBasicType& original);
    void loadMarkers(MarkedBasicType& destination) const;
};

template <typename BasicType, typename BasicHistoryType>
class SceneUndoCommand : public QUndoCommand
{

};

template <typename BasicType, typename BasicHistoryType>
class SceneCommandAdd : public SceneUndoCommand<BasicType, BasicHistoryType>
{
public:
    SceneCommandAdd();
    void undo();
    void redo();
};

#endif // SCENEBASIC_H
