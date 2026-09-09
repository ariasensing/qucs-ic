#ifndef ICLAYOUT_H
#define ICLAYOUT_H

#include <QDialog>
#include <QFrame>
#include <qucsdoc.h>

#include "layLayoutView_qt.h"
#include "dbLayout.h"
#include "dbCell.h"

#include "ictech.h"
#include "rectangledrawer.h"

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
// grid
  void setGridOn(bool value);
  bool getGridOn();
//----------------------------------
// Technology
  QString getTechnology();
  void    setTechnology(QString fname);

protected:
// Event
  bool eventFilter(QObject *obj, QEvent *event) override;

  tech*   m_tech;
private:
  Ui::icLayout*           ui;
  Schematic               *a_Schematic;

  lay::LayoutViewWidget*  m_layoutWidget;
  lay::LayoutView*        m_layoutView;
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
  // Helper for shape editing
    ShapeDrawer *m_shapeDrawer;

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
protected:
  void    terminatePreviousInsertion();
// Cursor management
  // Public control
  void setMagnetic(bool on);
  void setCatchDistance(double um);
  void setGrid(double um);               // 0 = use view grid
  void clearMagneticLayers();
  void addMagneticLayer(int layer, int datatype = 0);
  void addMagneticLayerIndex(unsigned int idx);
  void updateSnapCursor(const QPointF &widgetPos);
  db::DPoint pixelToMicron(const QPointF &pt) const;
  db::DPoint snapToGrid(const db::DPoint &p) const;
  db::DPoint snapMagnetic(const db::DPoint &p) const;
  bool isLayerAllowed(unsigned int layerIndex) const;
  double gridMicron() const;

  lay::ShapeMarker           *mp_cursor     = nullptr;

  bool   m_magnetic   = false;
  double m_catchDist  = 0.5;      // µm
  double m_grid       = 0.0;      // 0 = view grid
  std::set<unsigned int> m_magneticLayers;

         // cache for performance
  db::DPoint m_lastRaw;
  db::DPoint m_lastSnapped;
  bool       m_hasCache = false;
  double     m_cacheRadius2 = 0.0;
};

#endif // ICLAYOUT_H
