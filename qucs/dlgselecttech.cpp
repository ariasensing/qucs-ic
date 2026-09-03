#include "dlgselecttech.h"
#include "ui_dlgselecttech.h"
#include "ictech.h"

dlgSelectTech::dlgSelectTech(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgSelectTech) ,
      m_selected("")
{
  ui->setupUi(this);

  // Fill the combobox
  QStringList available_techs = tech::getAvailableTechs();
  ui->cbTechList->addItem(tr("[NONE]"));
  for (const auto& tname : std::as_const(available_techs))
    ui->cbTechList->addItem(tname);

  //
  QObject::connect(ui->btnOk,     &QPushButton::clicked, this, &dlgSelectTech::OkPressed);
  QObject::connect(ui->btnCancel, &QPushButton::clicked, this, &dlgSelectTech::CancelPressed);

}

dlgSelectTech::~dlgSelectTech() {
  delete ui;
}

/**
 * @brief dlgSelectTech::OkPressed
 */
void  dlgSelectTech::OkPressed()
{
  m_selected = ui->cbTechList->currentIndex()==0 ? "" : ui->cbTechList->currentText();
  QDialog::accept();
}

void  dlgSelectTech::CancelPressed()
{
  m_selected = "";
  QDialog::reject();
}

