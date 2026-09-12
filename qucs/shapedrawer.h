#ifndef SHAPEDRAWER_H
#define SHAPEDRAWER_H

// ShapeDrawer.h
#pragma once

#include <QObject>
#include "layLayoutView_qt.h"
#include "layMarker.h"
#include "layLayerProperties.h"
#include "dbLayout.h"
#include "dbCell.h"
#include "dbBox.h"
#include "dbPoint.h"


#include "layadvancededitingplugin.h"

#include "crosshairoverlay.h"
/*
 * The shapeDrawer is the class responsible for cursor behaviour and drawing of the temporary shapes
 * during edit activities.
 **/

class ShapeDrawer : public QObject
{
  Q_OBJECT
public:
  explicit ShapeDrawer(lay::LayoutViewBase *view, QObject *parent = nullptr);
  ShapeDrawer(const ShapeDrawer& sd);
  ~ShapeDrawer();

  bool isActive() const { return m_active; }

protected:
  // helpers
  lay::LayoutViewBase *m_view       = nullptr;
  bool                 m_active     = false;
  db::Shape*           m_shape      = nullptr;
  lay::Marker*         mp_cursor    = nullptr;
  //CrosshairOverlay*    m_cursor_overlay = nullptr;

public:
  virtual void set_new_mouse_position(const db::DPoint& p);
  virtual void start_dragging(const db::DPoint&  ) {}

//------------------------------------------------------
//------------------------------------------------------
// Magnetic snapping handler
//------------------------------------------------------
//------------------------------------------------------
public:
  void                set_magnetic(bool on)               { m_magnetic = on; }
  void                set_catch_distance(int  pixels );
  void                set_grid(double grid_um);             // 0 = use view’s editor grid
  double              grid_micron() const;
  db::DPoint          current_snapped_pos() const { return m_snapped; }
  static unsigned int brighterColor(unsigned int rgb, double factor = 1.4);
  QPointF             micron_to_pixel(const db::DPoint &micronPos) const;
  double              pixels_to_micron(double pixels) const;
private:
  void       update_cursor(const db::DPoint &raw);
  db::DPoint snap_to_grid(const db::DPoint &p) const;
  db::DPoint snap_magnetic(const db::DPoint &p) const;

  bool     m_magnetic     = false;
  int      m_catch_pixels = 12;           // pixels
  double   m_grid         = 0.0;          // 0 → use view grid
  db::DPoint m_snapped;
  QPointF    m_snapped_pixels;




};



#endif // SHAPEDRAWER_H
