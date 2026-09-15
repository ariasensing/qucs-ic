#ifndef QTOOLBOX_SELECTION_W_H
#define QTOOLBOX_SELECTION_W_H

#include <QFrame>
#include <qtoolbox_helper_w.h>
namespace Ui {
class QToolbox_Selection_w;
}

class QToolbox_Selection_w : public QToolbox_helper_w {
  Q_OBJECT

public:
  explicit QToolbox_Selection_w(QWidget* parent = nullptr);
  ~QToolbox_Selection_w();

private:
  Ui::QToolbox_Selection_w* ui;

public slots:
  // The mouse position changed
  void new_mouse_position(double x, double y);
  void zoom_pressed();
signals:
  void zoom_on_position(double x, double y);

};

#endif // QTOOLBOX_SELECTION_W_H
