#ifndef MOUSESELECTIONDRAWER_H
#define MOUSESELECTIONDRAWER_H

#include <QObject>
#include "shapedrawer.h"

class MouseSelectionDrawer  : public ShapeDrawer
{
  Q_OBJECT
public:
  explicit MouseSelectionDrawer(lay::LayoutViewBase *view, QObject *parent = nullptr);

  void set_new_mouse_position(const db::DPoint& p) override;
  void start_dragging(const db::DPoint& p ) override;
};

#endif // MOUSESELECTIONDRAWER_H
