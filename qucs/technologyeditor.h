#ifndef TECHNOLOGYEDITOR_H
#define TECHNOLOGYEDITOR_H

#include <QDialog>

namespace Ui {
class TechnologyEditor;
}

class TechnologyEditor : public QDialog {
  Q_OBJECT

public:
  explicit TechnologyEditor(QWidget* parent = nullptr);
  ~TechnologyEditor();

private:
  Ui::TechnologyEditor* ui;
};

#endif // TECHNOLOGYEDITOR_H
