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
  m_active(sd.m_active),
  m_magnetic(sd.m_magnetic),
  m_catch_pixels(sd.m_catch_pixels),
  m_grid(sd.m_grid),
  m_snapped(sd.m_snapped)

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
 * @brief layAdvancedEditingPlugin::update_cursor
 * @param raw
 */
void ShapeDrawer::update_cursor(const db::DPoint &raw)
{
  db::DPoint s = snap_to_grid(raw);

  /*if (m_magnetic)
    s = snap_magnetic(s);*/          // magnetic first, then we can re-grid if desired
    // or: s = snap_to_grid(snap_magnetic(raw));

  m_snapped = s;

  // go with the magnetic snapping



}
/**
 * @brief ShapeDrawer::micronToPixel
 * @param micronPos
 * @return
 */
QPointF ShapeDrawer::micron_to_pixel(const db::DPoint &micronPos) const
{
  if (!m_view || !m_view->widget())
    return QPointF();

  db::DCplxTrans micron2pixel = m_view->viewport().trans();
  db::DPoint pixel = micron2pixel * micronPos;

  QWidget *canvas = m_view->widget();
  int x = static_cast<int>(std::round(pixel.x()));
  int y = canvas->height() - 1 - static_cast<int>(std::round(pixel.y()));

  return QPointF(x, y);
}
/**
 * @brief layAdvancedEditingPlugin::snap_to_grid
 * @param p
 * @return
 */
db::DPoint ShapeDrawer::snap_to_grid(const db::DPoint &p) const
{
  double g = m_grid;
  if (g <= 0.0)
    g = grid_micron();          // editor grid of the view

  if (g <= 0.0)
    return p;

  double x = std::round(p.x() / g) * g;
  double y = std::round(p.y() / g) * g;
  return db::DPoint(x, y);
}
/**
 * @brief layAdvancedEditingPlugin::set_catch_distance
 * @param pixels
 */
void ShapeDrawer::set_catch_distance(int  pixels )
{
  if (pixels==0)
  {m_magnetic = false; m_catch_pixels=0;}
  else
    m_catch_pixels = pixels;

}

/**
 * @brief ShapeDrawer::set_new_mouse_position
 * @param x
 * @param y
 */
void ShapeDrawer::set_new_mouse_position(const db::DPoint& p)
{
  // Update the position according to the policy
  update_cursor(p);
}


double ShapeDrawer::grid_micron() const
{
  if (m_grid > 0.0)
    return m_grid;

  // Try view configuration

  int cv =  m_view->active_cellview_index();
  double g = 0.0;
  if (cv >= 0)
  {
    db::Layout* layout = &(m_view->active_cellview()->layout());
    g = layout->technology()->default_grid();
    if (g <= 0.0)
      g = layout->dbu();

  }

  if (g <= 0.0) g = 0.001; // 1nm fallback
  return g;

}




// ---------------------------------------------------------------
// Make a color brighter (simple HSV-style brightening)
// ---------------------------------------------------------------
unsigned int ShapeDrawer::brighterColor(unsigned int rgb, double factor)
{
  int r = (rgb >> 16) & 0xff;
  int g = (rgb >>  8) & 0xff;
  int b = (rgb      ) & 0xff;

  r = std::min(255, int(r * factor));
  g = std::min(255, int(g * factor));
  b = std::min(255, int(b * factor));

  return (r << 16) | (g << 8) | b;
}
// ---------------------------------------------------------------
// Helpe
// ---------------------------------------------------------------

double    ShapeDrawer::pixels_to_micron(double pixels) const
{
  if (!m_view)
    return 0.0;

  db::DCplxTrans t = m_view->viewport().trans();
  double scale = t.mag();          // pixels per micron
  if (scale <= 1e-12)
    return 0.0;

  return pixels / scale;           // microns
}

