#ifndef RECTANGLEDRAWER_H
#define RECTANGLEDRAWER_H

// RectangleDrawer.h
#pragma once

#include <QObject>
#include "layLayoutView.h"
#include "layMarker.h"
#include "layLayerProperties.h"
#include "dbLayout.h"
#include "dbCell.h"
#include "dbBox.h"
#include "dbPoint.h"
#include "shapedrawer.h"
class RectangleDrawer : public ShapeDrawer
{
  Q_OBJECT
public:
  explicit RectangleDrawer(lay::LayoutViewBase *view, QObject *parent = nullptr);
  ~RectangleDrawer();

  // Start / stop interactive drawing

private:
  void clearMarker();  
  void createBox(const db::DPoint &p1, const db::DPoint &p2);


  bool m_p1_valid  = false;
  db::DPoint m_p1;
};

#endif // RECTANGLEDRAWER_H
