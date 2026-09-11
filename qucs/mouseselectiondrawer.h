#ifndef MOUSESELECTIONDRAWER_H
#define MOUSESELECTIONDRAWER_H

#include <QObject>
#include "shapedrawer.h"

class MouseSelectionDrawer  : public ShapeDrawer
{
  Q_OBJECT
public:
  explicit MouseSelectionDrawer(lay::LayoutView *view, QObject *parent = nullptr);

  void set_new_mouse_position(double x, double y) override;
  void start_dragging(double x, double y ) override;
};

#endif // MOUSESELECTIONDRAWER_H
