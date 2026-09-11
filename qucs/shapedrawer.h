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

class ShapeDrawer : public QObject
{
  Q_OBJECT
public:
  explicit ShapeDrawer(lay::LayoutView *view, QObject *parent = nullptr);
  ~ShapeDrawer();

  bool isActive() const { return m_active; }

protected:
  // helpers
  lay::LayoutView *m_view       = nullptr;
  bool             m_active     = false;
  db::Shape*       m_currShape  = nullptr;
private:
  virtual void updateMarker(const db::DPoint &p) {}
public:
  virtual void set_new_mouse_position(double x, double y) {}
  virtual void start_dragging(const db::DPoint& p ) {}



};
#endif // SHAPEDRAWER_H
