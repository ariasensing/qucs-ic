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
  explicit RectangleDrawer(lay::LayoutView *view, QObject *parent = nullptr);
  ~RectangleDrawer();

         // Start / stop interactive drawing
  void start() override;
  void stop()  override;

         // Feed mouse events (coordinates must be in micron / D-units)
  bool mousePress  (const db::DPoint &p, Qt::MouseButton button) override;
  bool mouseMove   (const db::DPoint &p) override;
  bool mouseRelease(const db::DPoint &p, Qt::MouseButton button) override;
  bool eventFilter(QObject *obj, QEvent *event) override;
private:
  void clearMarker();
  void updateMarker(const db::DPoint &p2);
  void createBox(const db::DPoint &p1, const db::DPoint &p2);


  bool m_p1_valid  = false;
  db::DPoint m_p1;
};

#endif // RECTANGLEDRAWER_H
