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
  explicit icLayout(QucsApp*, class Schematic*, const QString&);
  ~icLayout();

  void  setName(const QString&);
  bool  load() override;
  int   save() override;
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
