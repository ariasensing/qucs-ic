// RectangleDrawer.cpp
#include "shapedrawer.h"
#include "tlString.h"
#include <algorithm>
#include "layLayoutView_qt.h"
#include "dbPoint.h"
#include "dbTrans.h"

// mousePos = position relative to the canvas widget (from QMouseEvent::pos())
db::DPoint pixelToMicron(lay::LayoutView *view, const QPointF &mousePos)
{
  if (!view)
    return db::DPoint();

         // viewport_trans() maps micron → pixel (y grows upward, mathematical)
  db::DCplxTrans micron2pixel = view->viewport().trans();

         // We need the inverse: pixel → micron
  db::DCplxTrans pixel2micron = micron2pixel.inverted();

         // Important: Qt y=0 is at the top, while KLayout’s viewport y=0 is at the bottom.
         // Therefore we must flip the y coordinate.
  int h = view->widget()->height();          // height in pixels
  double x = mousePos.x();
  double y = h - 1 - mousePos.y();          // flip y

  return pixel2micron * db::DPoint(x, y);
}


ShapeDrawer::ShapeDrawer(lay::LayoutView *view, QObject *parent)
    : QObject(parent)
      , m_view(view)
{
}

ShapeDrawer::~ShapeDrawer()
{
  if (!m_active)
    emit interrupted();
}

void ShapeDrawer::start()
{
  m_active   = true;
  emit started();
}

void ShapeDrawer::stop()
{
  m_active   = false;
  emit done();
}

bool ShapeDrawer::mousePress(const db::DPoint &p, Qt::MouseButton button)
{
  if (!m_active || !m_view || button != Qt::LeftButton)
    return false;

  return true;
}

bool ShapeDrawer::mouseMove(const db::DPoint &)
{
  if (!m_active || !m_p1_valid)
    return false;

  return true;
}

bool ShapeDrawer::mouseRelease(const db::DPoint &, Qt::MouseButton)
{

  return true;
}


bool ShapeDrawer::eventFilter(QObject *obj, QEvent *)
{
  if (obj==nullptr) return false;
  if ((lay::LayoutView*)(obj) == m_view || (lay::LayoutView*)(obj->parent()) == m_view || (QWidget*)(obj)==m_view->widget())
    return true;
  return false;
}


unsigned int ShapeDrawer::brighterColor(unsigned int rgb, double factor)
{
  int r = (rgb >> 16) & 0xff;
  int g = (rgb >>  8) & 0xff;
  int b = (rgb      ) & 0xff;

  r = std::min(255, static_cast<int>(r * factor + 0.5));
  g = std::min(255, static_cast<int>(g * factor + 0.5));
  b = std::min(255, static_cast<int>(b * factor + 0.5));

  return (static_cast<unsigned int>(r) << 16) |
         (static_cast<unsigned int>(g) <<  8) |
         static_cast<unsigned int>(b);
}

