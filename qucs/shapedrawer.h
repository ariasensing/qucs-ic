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
  static unsigned int brighter_color(unsigned int rgb, double factor = 1.4);

};



#endif // SHAPEDRAWER_H
