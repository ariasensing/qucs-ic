// RectangleDrawer.cpp
#include "rectangledrawer.h"
#include "tlString.h"
#include <algorithm>
#include <QEvent>
#include <QMouseEvent>
#include "layMarker.h"
#include "layLayoutView_qt.h"


RectangleDrawer::RectangleDrawer(lay::LayoutView *view, QObject *parent)
    : ShapeDrawer(view, parent)

{

}

RectangleDrawer::~RectangleDrawer()
{
  //clearMarker();
}
/*
void RectangleDrawer::start()
{
  ShapeDrawer::start();

  m_p1_valid = false;
  clearMarker();
  if (m_view)
    m_view->message("Click first corner of the rectangle");
}

void RectangleDrawer::stop()
{
  ShapeDrawer::stop();
  m_p1_valid = false;
  clearMarker();
  if (m_view)
    m_view->message(std::string());
}

bool RectangleDrawer::mousePress(const db::DPoint &p, Qt::MouseButton button)
{
  if (!ShapeDrawer::mousePress(p,button))
    return false;

  if (!m_p1_valid) {
    // first corner
    m_p1 = p;
    m_p1_valid = true;
    m_view->message("Click second corner of the rectangle (Esc to cancel)");
    updateMarker(p);
    return true;
  }
  else {
    // second corner → create the box
    createBox(m_p1, p);
    stop();
    return true;
  }
}

bool RectangleDrawer::mouseMove(const db::DPoint &p)
{
  if (!ShapeDrawer::mouseMove(p)) return false;
  updateMarker(p);
  return true;
}

bool RectangleDrawer::mouseRelease(const db::DPoint &, Qt::MouseButton)
{
  // we finish on press of the second point, so nothing special here
  return false;
}

bool RectangleDrawer::eventFilter(QObject *obj, QEvent *event)
{
  if (!ShapeDrawer::eventFilter(obj,event))
      return QObject::eventFilter(obj, event);

  if (event->type() == QEvent::MouseButtonPress) {
    auto *me = static_cast<QMouseEvent*>(event);
    db::DPoint p = pixelToMicron(m_view, me->globalPosition());
    // convert widget coords → micron coordinates if needed
    if (mousePress(p, me->button()))
      return true;
      }
  else if (event->type() == QEvent::MouseMove) {
    auto *me = static_cast<QMouseEvent*>(event);
    db::DPoint p = pixelToMicron(m_view, me->globalPosition());
    if (mouseMove(p))
      return true;
      }

  return QObject::eventFilter(obj,event);
}

void RectangleDrawer::clearMarker()
{
  if (m_marker) {
    delete m_marker;
    m_marker = nullptr;
  }
}

void RectangleDrawer::createBox(const db::DPoint &p1, const db::DPoint &p2)
{
  if (!m_view)
    return;

  int cv_index = m_view->active_cellview_index();
  if (cv_index < 0)
    return;

  const lay::CellView &cv = m_view->cellview(cv_index);
  db::Layout *layout = &(cv->layout());
  db::Cell   *cell   = cv.cell();
  if (!layout || !cell)
    return;

         // ----- resolve target layer -----
  int layer_index = -1;

  lay::LayerPropertiesConstIterator lp = m_view->current_layer();
  if (!lp.at_end()) {
    // preferred: use the layer index already resolved by the view
    layer_index = lp->layer_index();
  }

  if (layer_index < 0) {
    m_view->message("No valid layer selected – rectangle not created");
    return;
  }

         // micron → database units
  const double dbu = layout->dbu();
  db::Point pi1(db::coord_traits<db::Coord>::rounded(p1.x() / dbu),
                db::coord_traits<db::Coord>::rounded(p1.y() / dbu));
  db::Point pi2(db::coord_traits<db::Coord>::rounded(p2.x() / dbu),
                db::coord_traits<db::Coord>::rounded(p2.y() / dbu));

  db::Box box(pi1, pi2);

         // insert with undo support
  m_view->transaction("Create rectangle");
  cell->shapes(static_cast<unsigned int>(layer_index)).insert(box);
  m_view->commit();

  m_view->update_content();
  m_view->message("Rectangle created");
}


void RectangleDrawer::updateMarker(const db::DPoint &p2)
{
    if (!m_view)
      return;

    if (!m_marker)
      m_marker = new lay::Marker(m_view,m_view->active_cellview_index());

    m_marker->set(db::DBox(m_p1, p2),db::DCplxTrans());

           // ----- defaults (bright green) -----
    unsigned int fillColor  = 0x00cc00;
    unsigned int frameColor = 0x00cc00;
    int dither    = 0;          // solid
    int lineStyle = 0;
    int width     = 1;

           // ----- take style from currently selected layer -----
    lay::LayerPropertiesConstIterator lp = m_view->current_layer();
    if (!lp.at_end()) {
      fillColor  = lp->eff_fill_color(true);
      frameColor = lp->eff_frame_color(true);
      dither     = static_cast<int>(lp->eff_dither_pattern(true));
      lineStyle  = static_cast<int>(lp->eff_line_style(true));
      width      = lp->width(true);
      if (width < 1)
        width = 1;
  }

           // brighter + bolder
    m_marker->set_color(brighterColor(fillColor));
    m_marker->set_frame_color(brighterColor(frameColor));
    m_marker->set_dither_pattern(dither);          // same stipple as the layer
    m_marker->set_line_style(lineStyle);
    m_marker->set_line_width(width + 1);           // one pixel bolder
    m_marker->set_vertex_size(0);
    m_marker->set_halo(0);

}
*/