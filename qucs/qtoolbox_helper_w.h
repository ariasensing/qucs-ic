#ifndef QTOOLBOX_HELPER_W_H
#define QTOOLBOX_HELPER_W_H

#include <QFrame>

namespace Ui {
class QToolbox_helper_w;
}

class QToolbox_helper_w : public QFrame {
  Q_OBJECT

public:
  explicit QToolbox_helper_w(QWidget* parent = nullptr);
  ~QToolbox_helper_w();

private:
  Ui::QToolbox_helper_w* ui;
public slots:
  // The mouse position changed
  void new_mouse_position(double , double ) {}
signals:
  void zoom_on_position(double x, double y);
};

#endif // QTOOLBOX_HELPER_W_H
