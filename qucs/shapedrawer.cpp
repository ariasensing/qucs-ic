// RectangleDrawer.cpp
#include "shapedrawer.h"
#include "tlString.h"
#include <algorithm>
#include "layLayoutView_qt.h"
#include "dbPoint.h"
#include "dbTrans.h"
#include "layadvancededitingplugin.h"


ShapeDrawer::ShapeDrawer(lay::LayoutViewBase *view, QObject *parent)
    : QObject(parent)
      , m_view(view)
{
  m_shape   = nullptr;

}

ShapeDrawer::ShapeDrawer(const ShapeDrawer& sd) :
  QObject(sd.parent()),
  m_view(sd.m_view),
  m_active(sd.m_active)
{
  // The base class does not define a marker or a shape
  mp_cursor = nullptr;
  m_shape   = nullptr;

}

ShapeDrawer::~ShapeDrawer()
{
  if (mp_cursor!=nullptr) delete mp_cursor;
  if (m_shape!=nullptr) delete m_shape;

}


/**
 * @brief ShapeDrawer::set_new_mouse_position
 * @param x
 * @param y
 */
void ShapeDrawer::set_new_mouse_position(const db::DPoint& )
{
}


// ---------------------------------------------------------------
// Make a color brighter (simple HSV-style brightening)
// ---------------------------------------------------------------
unsigned int ShapeDrawer::brighter_color(unsigned int rgb, double factor)
{
  int r = (rgb >> 16) & 0xff;
  int g = (rgb >>  8) & 0xff;
  int b = (rgb      ) & 0xff;

  r = std::min(255, int(r * factor));
  g = std::min(255, int(g * factor));
  b = std::min(255, int(b * factor));

  return (r << 16) | (g << 8) | b;
}
