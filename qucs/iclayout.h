#ifndef ICLAYOUT_H
#define ICLAYOUT_H

#include <QDialog>
#include <QFrame>
#include <qucsdoc.h>

namespace Ui {
class icLayout;
}

class Schematic;

class icLayout : public QDialog, public QucsDoc {
  Q_OBJECT

public:
  explicit icLayout(QucsApp*, const QString&);
  ~icLayout();

  void  setName(const QString&);
  bool  load() { return true; }
  int   save() { return 0; }
  void  print(QPrinter*, QPainter*, bool, bool, QMargins) {}
  void  becomeCurrent(bool) {}
  double zoomBy(double) { return 1.0; }
  void  showAll() {}
  void  zoomToSelection() {}
  void  showNoZoom() {}


  void  attachToSchematic(Schematic *schematic = nullptr);
private:
  Ui::icLayout* ui;
  Schematic   *a_Schematic;


};

#endif // ICLAYOUT_H
