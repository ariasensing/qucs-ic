#include "iclayout.h"
#include "ui_iclayout.h"
#include "schematic.h"
#include "dbManager.h"          // optional, for undo/redo
#include "main.h"
#include "layLayoutView_qt.h"
#include "layMarker.h"

// ----------------------------------------------------------------------
// Public control API
// ----------------------------------------------------------------------
void icLayout::setMagnetic(bool on)          { m_magnetic = on; }
void icLayout::setCatchDistance(double um)   { m_catchDist = um; }
void icLayout::setGrid(double um)            { m_grid = um; }

void icLayout::clearMagneticLayers()
{
  m_magneticLayers.clear();
}

void icLayout::addMagneticLayerIndex(unsigned int idx)
{
  m_magneticLayers.insert(idx);
}

void icLayout::addMagneticLayer(int layer, int datatype)
{
  if (!m_layout) return;

  unsigned int li = m_layout->get_layer_maybe(db::LayerProperties(layer,datatype));
  if (li>=0)
    m_magneticLayers.insert(li);
}

// ----------------------------------------------------------------------
// Coordinate conversion (pixel → micron)
// ----------------------------------------------------------------------
db::DPoint icLayout::pixelToMicron(const QPointF &pt) const
{
  if (!m_layoutView) return db::DPoint();

  db::DBox view_box = m_layoutView->box();          // micron

  double scale = m_layoutWidget->width() / view_box.width();
  db::DCplxTrans micron2pixel(scale, 0.0, false,
                              db::DVector(-view_box.left()*scale,
                                          -view_box.bottom()*scale));


  db::DCplxTrans pixel2micron = micron2pixel.inverted();

  int h = m_layoutView->widget() ? m_layoutView->widget()->height() : 0;
  double x = pt.x();
  double y = h - 1 - pt.y();               // Qt → KLayout y-flip

  return pixel2micron * db::DPoint(x, y);
}

// ----------------------------------------------------------------------
// Grid snap
// ----------------------------------------------------------------------
db::DPoint icLayout::snapToGrid(const db::DPoint &p) const
{
  double g = m_grid;
  if (g <= 0.0)
    g = m_layout->dbu();

  if (g <= 0.0)
    return p;

  return db::DPoint(std::round(p.x()/g)*g, std::round(p.y()/g)*g);
}

// ----------------------------------------------------------------------
// Layer filter
// ----------------------------------------------------------------------
bool icLayout::isLayerAllowed(unsigned int layerIndex) const
{
  if (m_magneticLayers.empty())
    return true;                       // no filter → all layers
  return m_magneticLayers.count(layerIndex) > 0;
}

// ----------------------------------------------------------------------
// High-performance magnetic snap
// ----------------------------------------------------------------------
db::DPoint icLayout::snapMagnetic(const db::DPoint &raw) const
{
  // cache
  if (m_hasCache) {
    double dx = raw.x() - m_lastRaw.x();
    double dy = raw.y() - m_lastRaw.y();
    if (dx*dx + dy*dy < m_cacheRadius2)
      return m_lastSnapped;
  }

  const double catch2 = m_catchDist * m_catchDist;
  double best2 = catch2;
  db::DPoint best = raw;
  bool found = false;

  int cv = m_layoutView->active_cellview_index();
  if (cv < 0) return raw;

  const lay::CellView &cellview = m_layoutView->cellview(cv);
  const db::Cell   *cell   = cellview.cell();
  if (!m_layout || !cell) return raw;

  const double dbu = m_layout->dbu();
  const db::CplxTrans to_um(dbu);
  const db::VCplxTrans to_dbu = to_um.inverted();

  db::Point pt_dbu = to_dbu * raw;
  db::Coord margin = db::coord_traits<db::Coord>::rounded(m_catchDist / dbu) + 2;
  db::Box search = db::Box(pt_dbu, pt_dbu).enlarged(db::Vector(margin, margin));

  const int maxShapes = 40;
  int tested = 0;

         // Pass 1 – vertices (preferred)
  for (auto lp = m_layoutView->begin_layers(); !lp.at_end() && tested < maxShapes; ++lp) {
    if (!lp->visible(true) || lp->layer_index()<0) continue;
    unsigned int li = lp->layer_index();
    if (!isLayerAllowed(li)) continue;

    for (db::ShapeIterator si = cell->begin_touching(li, search, db::ShapeIterator::Points);
         !si.at_end() && tested < maxShapes; ++si, ++tested)
    {
      for (auto pt = si->begin_point(); !pt.at_end(); ++pt) {
        db::DPoint dpt = to_um * *pt;
        double d2 = dpt.sq_distance(raw);
        if (d2 < best2) {
          best2 = d2;
          best = dpt;
          found = true;
          if (best2 < 1e-8) goto done;
        }
      }
    }
  }

         // Pass 2 – edges (only if needed)
  if (best2 > catch2 * 0.25) {
    tested = 0;
    for (auto lp = m_layoutView->begin_layers(); !lp.at_end() && tested < maxShapes; ++lp) {
      if (!lp->visible(true) || lp->layer_index()<0) continue;
      unsigned int li = lp->layer_index();
      if (!isLayerAllowed(li)) continue;
/*
      for (db::ShapeIterator si = cell->begin_touching(li, search,  db::ShapeIterator::Edges);
           !si.at_end() && tested < maxShapes; ++si, ++tested)
      {
        for (auto e = si->begin_edge(); !e.at_end(); ++e) {
          db::DEdge de = to_um * *e;
          db::DPoint closest = de.closest_point(raw);
          double d2 = closest.sq_distance(raw);
          if (d2 < best2) {
            best2 = d2;
            best = closest;
            found = true;
          }
        }
      }*/
    }
  }

done:
  /*
  m_lastRaw      = raw;
  m_lastSnapped  = found ? best : raw;
  m_hasCache     = true;
  m_cacheRadius2 = (m_catchDist * 0.4) * (m_catchDist * 0.4);
*/
  return m_lastSnapped;
}

// ----------------------------------------------------------------------
// Update the visible cursor
// ----------------------------------------------------------------------
void icLayout::updateSnapCursor(const QPointF &widgetPos)
{
  db::DPoint raw = pixelToMicron(widgetPos);
  db::DPoint snapped = snapToGrid(raw);

  if (m_magnetic)
    snapped = snapMagnetic(snapped);

         // draw a small target
  const double s = 0.25; // µm
  db::box<db::DCoord>  x(snapped.x() - s, snapped.y() - s,
                         snapped.x() + s, snapped.y() + s);
  db::ICplxTrans i;
  //mp_cursor->set(x, i);
}

// ----------------------------------------------------------------------
// Event filter
// ----------------------------------------------------------------------
bool icLayout::eventFilter(QObject *watched, QEvent *event)
{
  if (event->type() == QEvent::MouseMove ||
      event->type() == QEvent::MouseButtonPress ||
      event->type() == QEvent::MouseButtonRelease)
  {
    QMouseEvent *me = static_cast<QMouseEvent*>(event);

           // Make sure the position is relative to the canvas
    QPointF pos = me->position();
    if (watched != m_layoutWidget) {
      // map from the watched widget to the canvas if necessary
      pos = m_layoutWidget->mapFromGlobal(me->globalPosition());
    }

    updateSnapCursor(pos);

           // You can still let the event propagate so KLayout tools keep working
           // return false;
  }

  return QDialog::eventFilter(watched, event);
}

double icLayout::gridMicron() const
{
  if (m_grid > 0.0)
    return m_grid;

  // try view config
  std::string v = m_layoutView->config_get("grid-micron");
  double g = 0.0;
  if (!v.empty())
    tl::from_string(v, g);

  return (g > 0.0) ? g : m_layout->dbu();
}