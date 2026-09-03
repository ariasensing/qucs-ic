#ifndef ICLAYOUT_H
#define ICLAYOUT_H

#include <QDialog>
#include <QFrame>
#include <qucsdoc.h>

#include "layLayoutView_qt.h"
#include "dbLayout.h"
#include "dbCell.h"

#include "ictech.h"

namespace Ui {
class icLayout;
}

class Schematic;

class icLayout : public QDialog, public QucsDoc {
  Q_OBJECT

public:
  explicit icLayout(QucsApp* owner, class Schematic* schemowner, const QString& layoutfile, const QString& techfile);
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
//----------------------------------
// Technology
  tech* getTechnology();
  void  setTechnology(tech* ict);
private:
  Ui::icLayout*           ui;
  Schematic               *a_Schematic;

  lay::LayoutViewWidget*  m_layoutWidget;
  lay::LayoutView*        m_layoutView;
  db::Manager*            m_dbManager;
  unsigned int            m_canvas_id;
  tech*                   m_technology;
public slots:
  void                    loadLayoutClicked();
  void                    saveLayoutClicked();
  void                    selectAll();


};

#endif // ICLAYOUT_H
