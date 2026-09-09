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
db::DPoint pixelToMicron(lay::LayoutView *view, const QPointF &mousePos);

class ShapeDrawer : public QObject
{
  Q_OBJECT
public:
  explicit ShapeDrawer(lay::LayoutView *view, QObject *parent = nullptr);
  ~ShapeDrawer();

  // Start / stop interactive drawing
  virtual void start();
  virtual void stop();
  bool isActive() const { return m_active; }

         // Feed mouse events (coordinates must be in micron / D-units)
  virtual bool mousePress  (const db::DPoint &p, Qt::MouseButton button);
  virtual bool mouseMove   (const db::DPoint &p);
  virtual bool mouseRelease(const db::DPoint &p, Qt::MouseButton button);
  virtual bool eventFilter(QObject *obj, QEvent *event) override;

protected:
  // helpers
  static unsigned int brighterColor(unsigned int rgb, double factor = 1.45);

  lay::LayoutView *m_view   = nullptr;
  lay::Marker     *m_marker = nullptr;

  bool m_active    = false;
  bool m_p1_valid  = false;
  db::DPoint m_p1;
signals:
  void    started();
  void    interrupted();
  void    done();

};
#endif // SHAPEDRAWER_H
