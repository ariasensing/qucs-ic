#ifndef DLGSELECTTECH_H
#define DLGSELECTTECH_H

#include <QDialog>


namespace Ui {
class dlgSelectTech;
}

class dlgSelectTech : public QDialog {
  Q_OBJECT

public:
  explicit dlgSelectTech(QWidget* parent = nullptr);
  ~dlgSelectTech();
  QString     getSelectedTech() {return m_selected;}
private:
  QString m_selected;
  Ui::dlgSelectTech* ui;

public slots:
  void  OkPressed();
  void  CancelPressed();

};

#endif // DLGSELECTTECH_H
