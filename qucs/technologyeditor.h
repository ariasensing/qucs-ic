#ifndef TECHNOLOGYEDITOR_H
#define TECHNOLOGYEDITOR_H

#include <QDialog>
#include "ictech.h"
#include "layLayoutView.h"
#include "dbLayout.h"
#include "dbCell.h"

namespace Ui {
class TechnologyEditor;
}


  /**
   * @brief The TechnologyEditor class
   */

class TechnologyEditor : public QDialog {
  Q_OBJECT
private:
  bool      m_bSaved;
  tech*     m_editedTech;   // This is the local copy of the technology


  bool      copyDataToTech();   // GUI->tech
  bool      copyDataFromTech(); // tech->GUI
public:
  explicit TechnologyEditor(const QString& filename="", QWidget* parent = nullptr);
  ~TechnologyEditor();

  bool     fileSaved();
private:


private:
  Ui::TechnologyEditor* ui;

  QString                 m_lastError;
  QWidget*                m_WidgetWithError;
  QPixmap makeLayerPreview(unsigned int fillColor, int frameColor, int ditherIndex,
                   lay::LayoutView *view = nullptr,
                   int size = 24);

  void                      update_layer_row(int row, const lay::LayerPropertiesNode &props);

  // Convenience: apply directly to a LayoutView
  void applyCadenceToView(const std::string &layermapPath,
                          const std::string &drfPath,
                          bool clearExisting = true);
public slots:
  void      ok();
  void      cancel();
  void      save();         // Save the technology into the selected files
  void      saveas();
  void      load();
  void      loadLayers();
  void      saveLayers();
  void      listLayers();
  void      createLayer();
  void      removeLayer();
  void      editLayer(const QModelIndex& model);
  void      importLayerMap();
};

#endif // TECHNOLOGYEDITOR_H
