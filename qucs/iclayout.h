#ifndef ICLAYOUT_H
#define ICLAYOUT_H

#include <QDialog>
#include <QFrame>
#include <qucsdoc.h>

#include "layLayoutView_qt.h"
#include "dbLayout.h"
#include "dbCell.h"


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

  void  loadLayout();
  void  attachToSchematic(Schematic *schematic = nullptr);

  bool  initKlayoutWidget();
private:
  Ui::icLayout*           ui;
  Schematic               *a_Schematic;
  QString                 m_LayoutFile;

  lay::LayoutViewWidget*  m_layoutWidget;
  lay::LayoutView*        m_layoutView;
  db::Manager*            m_dbManager;
  unsigned int            m_canvas_id;
public slots:
  void                    loadLayoutClicked();
  void                    saveLayoutClicked();
  void                    selectAll();


};

#endif // ICLAYOUT_H
