#ifndef TECHNOLOGYEDITOR_H
#define TECHNOLOGYEDITOR_H

#include <QDialog>
#include "ictech.h"

namespace Ui {
class TechnologyEditor;
}

class TechnologyEditor : public QDialog {
  Q_OBJECT
private:
  bool      m_bSaved;
  tech*     m_existingTech; // This is the stored tech which we are attempting to modify
  tech*     m_editedTech;   // This is the local copy of the technology


  bool      copyDataToTech();
public:
  explicit TechnologyEditor(const QString& filename="", QWidget* parent = nullptr);
  ~TechnologyEditor();

  bool     fileSaved();

private:
  Ui::TechnologyEditor* ui;
public slots:
  void      ok();
  void      cancel();
  void      save();         // Save the technology into the selected files
  void      saveas();
  void      load();
};

#endif // TECHNOLOGYEDITOR_H
