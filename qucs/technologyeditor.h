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
  bool m_bSaved;

public:
  explicit TechnologyEditor(const QString& filename="", QWidget* parent = nullptr);
  ~TechnologyEditor();

  bool     fileSaved();

private:
  Ui::TechnologyEditor* ui;
};

#endif // TECHNOLOGYEDITOR_H
