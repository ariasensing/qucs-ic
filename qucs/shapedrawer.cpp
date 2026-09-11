// RectangleDrawer.cpp
#include "shapedrawer.h"
#include "tlString.h"
#include <algorithm>
#include "layLayoutView_qt.h"
#include "dbPoint.h"
#include "dbTrans.h"



ShapeDrawer::ShapeDrawer(lay::LayoutView *view, QObject *parent)
    : QObject(parent)
      , m_view(view)
{
}

ShapeDrawer::~ShapeDrawer()
{
}

