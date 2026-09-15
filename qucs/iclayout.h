#ifndef ICLAYOUT_H
#define ICLAYOUT_H

#include <QDialog>
#include <QFrame>
#include <qucsdoc.h>

#include "layLayoutView_qt.h"
#include "dbLayout.h"
#include "dbCell.h"
#include "layPlugin.h"
#include "layadvancededitingplugin.h"

#include "ictech.h"
#include "qtoolbox_helper_w.h"

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
  void  set_adv_layout_mode();
//----------------------------------
// grid
  void setGridOn(bool );
  bool getGridOn();
//----------------------------------
// Technology
  QString getTechnology();
  void    setTechnology(QString fname);

protected:

  tech*   m_tech;

private:
  Ui::icLayout*           ui;
  Schematic               *a_Schematic;
  layAdvancedEditingPlugin* m_plugin;
  lay::LayoutView*        m_layoutView;
  lay::LayoutViewWidget*  m_layoutWidget;
  db::Manager*            m_dbManager;
  db::Layout*             m_layout;
  unsigned int            m_canvas_id;
  QString                 m_technologyFile;
  bool                    m_bGridOn;
  void                    applyTechToView();
public slots:
  void                    selectAll();

private:
  void                    initActions();
  void                    initMenuBar();
public:
  QMenuBar *menuBar;
  QMenu   *fileMenu, *editMenu, *insMenu, *viewMenu, *selectMenu, *emMenu, *schematicMenu;
  QAction *fileNew, *fileOpen, *fileSave, *fileSaveAs,*fileClose, *fileSettings, *filePrint;
  QAction *editCopy, *editPaste, *editCut, *editStretch, *editScale, *editMove, *editRotate, *editMirrorX, *editMirrorY,
          *editChop;
  QAction *insertRect, *insertPath, *insertVia, *insertInstance, *insertPolygon, *insertCircle;
  QAction *editCoordinates;
private:

protected:


public slots:
//Slot
  void     slotFileNew();
  void     slotFileOpen();
  void     slotFileSave();
  void     slotFileSaveAs();
  void     slotFileClose();
  void     slotFileSettings();
  void     slotFilePrint();
//Edit
  void     slotEditCopy();
  void     slotEditPaste();
  void     slotEditCut();
  void     slotEditScale();
  void     slotEditMove();
  void     slotEditRotate();
  void     slotEditMirrorX();
  void     slotEditMirrorY();
  void     slotEditChop();
// Insert
  void     slotInsertRect();
  void     slotInsertPath();
  void     slotInsertVia();
  void     slotInsertInstance();
  void     slotInsertPolygon();
  void     slotInsertCircle();
// Edit coordinates
  void     slotEditCoordinates();
// Mouse move
  void update_mouse_position(const db::DPoint& pt);
// Operation
  void     selection_started();
  void     insertion_rect_started();

private:
  QToolbox_helper_w   *m_toolbox_dialog;
signals:
  void  new_mouse_position(double x, double y);


};

#endif // ICLAYOUT_H
