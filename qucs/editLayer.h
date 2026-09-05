#ifndef EDITLAYER_H
#define EDITLAYER_H
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QInputDialog>

#include "layLayoutView.h"
#include "layLayerProperties.h"

class LayerDialog : public QDialog
{
  Q_OBJECT
public:
  // Constructor for NEW layer
  explicit LayerDialog(lay::LayoutView *view, QWidget *parent = nullptr)
      : LayerDialog(view, lay::LayerProperties(), false, parent)
  {}

         // Constructor for EDIT layer
  explicit LayerDialog(lay::LayoutView *view,
                       const lay::LayerProperties &initial,
                       QWidget *parent = nullptr)
      : LayerDialog(view, initial, true, parent)
  {}

         // Results
  int     layer()         const { return m_layerSpin->value(); }
  int     datatype()      const { return m_datatypeSpin->value(); }
  QString name()          const { return m_nameEdit->text().trimmed(); }
  QColor  fillColor()     const { return m_fillColor; }
  int     ditherPattern() const { return m_stippleCombo->currentData().toInt(); }
  int     lineStyle()     const { return m_lineStyleCombo->currentData().toInt(); }

  bool isEditMode() const { return m_editMode; }

private:
  // Private unified constructor
  LayerDialog(lay::LayoutView *view,
              const lay::LayerProperties &initial,
              bool editMode,
              QWidget *parent)
      : QDialog(parent), m_view(view), m_editMode(editMode)
  {
    setWindowTitle(editMode ? tr("Edit Layer") : tr("New Layer"));
    resize(400, 340);

    auto *form = new QFormLayout(this);

           // ----- Layer / Datatype -----
    m_layerSpin = new QSpinBox;
    m_layerSpin->setRange(0, 65535);
    m_layerSpin->setValue(1);

    m_datatypeSpin = new QSpinBox;
    m_datatypeSpin->setRange(0, 65535);
    m_datatypeSpin->setValue(0);

    auto *ldLayout = new QHBoxLayout;
    ldLayout->addWidget(m_layerSpin);
    ldLayout->addWidget(new QLabel("/"));
    ldLayout->addWidget(m_datatypeSpin);
    form->addRow(tr("Layer / Datatype:"), ldLayout);

           // In edit mode → make them read-only
    if (m_editMode) {
      m_layerSpin->setReadOnly(true);
      m_datatypeSpin->setReadOnly(true);
      // Optional visual feedback
      m_layerSpin->setStyleSheet("QSpinBox { background: #3a3a3a; }");
      m_datatypeSpin->setStyleSheet("QSpinBox { background: #3a3a3a; }");
    }

           // ----- Name -----
    m_nameEdit = new QLineEdit;
    form->addRow(tr("Name:"), m_nameEdit);

           // ----- Color -----
    m_colorBtn = new QPushButton;
    m_colorBtn->setFixedSize(48, 24);
    m_fillColor = QColor(0, 128, 255);
    updateColorButton();
    connect(m_colorBtn, &QPushButton::clicked, this, &LayerDialog::chooseColor);
    form->addRow(tr("Color:"), m_colorBtn);

           // ----- Stipple -----
    m_stippleCombo = new QComboBox;
    populateStipples();
    m_stipplePreview = new QLabel;
    m_stipplePreview->setFixedSize(32, 32);
    m_stipplePreview->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    auto *stippleLayout = new QHBoxLayout;
    stippleLayout->addWidget(m_stippleCombo, 1);
    stippleLayout->addWidget(m_stipplePreview);
    form->addRow(tr("Stipple:"), stippleLayout);

    connect(m_stippleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LayerDialog::updateStipplePreview);

    m_editStippleBtn = new QPushButton(tr("Custom Stipple…"));
    connect(m_editStippleBtn, &QPushButton::clicked, this, &LayerDialog::editCustomStipple);
    form->addRow(QString(), m_editStippleBtn);

           // ----- Line Style -----
    m_lineStyleCombo = new QComboBox;
    populateLineStyles();
    m_lineStylePreview = new QLabel;
    m_lineStylePreview->setFixedSize(64, 24);
    m_lineStylePreview->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    auto *lineStyleLayout = new QHBoxLayout;
    lineStyleLayout->addWidget(m_lineStyleCombo, 1);
    lineStyleLayout->addWidget(m_lineStylePreview);
    form->addRow(tr("Line Style:"), lineStyleLayout);

    connect(m_lineStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LayerDialog::updateLineStylePreview);

    m_editLineStyleBtn = new QPushButton(tr("Custom Line Style…"));
    connect(m_editLineStyleBtn, &QPushButton::clicked, this, &LayerDialog::editCustomLineStyle);
    form->addRow(QString(), m_editLineStyleBtn);

           // ----- Buttons -----
    auto *buttons = new QHBoxLayout;
    buttons->addStretch();
    auto *okBtn     = new QPushButton(tr("OK"));
    auto *cancelBtn = new QPushButton(tr("Cancel"));
    buttons->addWidget(okBtn);
    buttons->addWidget(cancelBtn);
    form->addRow(buttons);

    connect(okBtn,     &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

           // ----- Load initial values when editing -----
    if (m_editMode) {
      loadFromProperties(initial);
    }

    updateStipplePreview(m_stippleCombo->currentIndex());
    updateLineStylePreview(m_lineStyleCombo->currentIndex());
  }

private slots:
  void chooseColor() {
    QColor c = QColorDialog::getColor(m_fillColor, this, tr("Select Layer Color"));
    if (c.isValid()) {
      m_fillColor = c;
      updateColorButton();
      updateStipplePreview(m_stippleCombo->currentIndex());
      updateLineStylePreview(m_lineStyleCombo->currentIndex());
    }
  }

  void updateStipplePreview(int) {
    m_stipplePreview->setPixmap(makeStipplePixmap(ditherPattern(), m_fillColor, 32));
  }

  void updateLineStylePreview(int) {
    m_lineStylePreview->setPixmap(makeLineStylePixmap(lineStyle(), m_fillColor, 64, 24));
  }

  void editCustomStipple() {
    bool ok = false;
    int idx = QInputDialog::getInt(this, tr("Custom Stipple"),
                                   tr("Custom dither index:"), 32, 16, 255, 1, &ok);
    if (ok) {
      m_stippleCombo->addItem(tr("Custom %1").arg(idx), idx);
      m_stippleCombo->setCurrentIndex(m_stippleCombo->count() - 1);
    }
  }

  void editCustomLineStyle() {
    bool ok = false;
    int idx = QInputDialog::getInt(this, tr("Custom Line Style"),
                                   tr("Custom line style index:"), 16, 8, 255, 1, &ok);
    if (ok) {
      m_lineStyleCombo->addItem(tr("Custom %1").arg(idx), idx);
      m_lineStyleCombo->setCurrentIndex(m_lineStyleCombo->count() - 1);
    }
  }

private:
  void loadFromProperties(const lay::LayerProperties &lp)
  {
    // Layer / Datatype (from source or explicit fields)
    // Prefer the effective/real values
    int l = lp.source(true).layer();
    int d = lp.source(true).datatype();

           // Fallback if source is not numeric
    if (l < 0) l = 0;
    if (d < 0) d = 0;

    m_layerSpin->setValue(l);
    m_datatypeSpin->setValue(d);

           // Name
    m_nameEdit->setText(QString::fromStdString(lp.name()));

           // Color
    unsigned int fc = lp.eff_fill_color(true);
    m_fillColor = QColor::fromRgba(fc | 0xff000000);
    updateColorButton();

           // Stipple
    int dither = static_cast<int>(lp.eff_dither_pattern(true));
    setComboToValue(m_stippleCombo, dither);

           // Line style
    int ls = static_cast<int>(lp.eff_line_style(true));
    setComboToValue(m_lineStyleCombo, ls);
  }

  void setComboToValue(QComboBox *combo, int value)
  {
    int idx = combo->findData(value);
    if (idx >= 0) {
      combo->setCurrentIndex(idx);
    } else {
      // Value not in the predefined list → add it as custom
      combo->addItem(tr("Custom %1").arg(value), value);
      combo->setCurrentIndex(combo->count() - 1);
    }
  }

  void updateColorButton() {
    QPixmap px(40, 20);
    px.fill(m_fillColor);
    m_colorBtn->setIcon(QIcon(px));
    m_colorBtn->setText(m_fillColor.name());
  }

  void populateStipples() {
    const struct { const char *name; int idx; } items[] = {
        { "Solid", 0 }, { "Hollow", 1 }, { "Dense 1", 2 }, { "Dense 2", 3 },
        { "Horizontal", 4 }, { "Vertical", 5 }, { "Cross", 6 },
        { "Diag /", 7 }, { "Diag \\", 8 }, { "Grid", 9 }, { "Dots", 10 }
    };
    for (auto &i : items)
      m_stippleCombo->addItem(tr(i.name), i.idx);
  }

  void populateLineStyles() {
    const struct { const char *name; int idx; } items[] = {
        { "Solid", 0 }, { "Dashed", 1 }, { "Dotted", 2 },
        { "Dash-Dot", 3 }, { "Dash-Dot-Dot", 4 },
        { "Long Dash", 5 }, { "Short Dash", 6 }
    };
    for (auto &i : items)
      m_lineStyleCombo->addItem(tr(i.name), i.idx);
  }

         // --- Preview helpers (same as before) ---
  static QPixmap makeStipplePixmap(int dither, const QColor &color, int size) {
    QPixmap pix(size, size);
    pix.fill(QColor(30, 30, 30));
    QPainter p(&pix);
    QBrush brush(color);
    switch (dither) {
    case 0:  brush.setStyle(Qt::SolidPattern);  break;
    case 1:  brush.setStyle(Qt::NoBrush);       break;
    case 2: case 3: brush.setStyle(Qt::Dense4Pattern); break;
    case 4:  brush.setStyle(Qt::HorPattern);    break;
    case 5:  brush.setStyle(Qt::VerPattern);    break;
    case 6:  brush.setStyle(Qt::CrossPattern);  break;
    case 7:  brush.setStyle(Qt::FDiagPattern);  break;
    case 8:  brush.setStyle(Qt::BDiagPattern);  break;
    default: brush.setStyle(Qt::Dense5Pattern); break;
    }
    p.fillRect(2, 2, size-4, size-4, brush);
    p.setPen(Qt::white);
    p.drawRect(1, 1, size-3, size-3);
    return pix;
  }

  static QPixmap makeLineStylePixmap(int style, const QColor &color, int w, int h) {
    QPixmap pix(w, h);
    pix.fill(QColor(30, 30, 30));
    QPainter p(&pix);
    Qt::PenStyle ps = Qt::SolidLine;
    switch (style) {
    case 1: ps = Qt::DashLine;       break;
    case 2: ps = Qt::DotLine;        break;
    case 3: ps = Qt::DashDotLine;    break;
    case 4: ps = Qt::DashDotDotLine; break;
    default: break;
    }
    p.setPen(QPen(color, 2, ps));
    p.drawLine(4, h/2, w-4, h/2);
    return pix;
  }

  lay::LayoutView *m_view = nullptr;
  bool m_editMode = false;

  QSpinBox    *m_layerSpin;
  QSpinBox    *m_datatypeSpin;
  QLineEdit   *m_nameEdit;
  QPushButton *m_colorBtn;
  QComboBox   *m_stippleCombo;
  QLabel      *m_stipplePreview;
  QPushButton *m_editStippleBtn;
  QComboBox   *m_lineStyleCombo;
  QLabel      *m_lineStylePreview;
  QPushButton *m_editLineStyleBtn;
  QColor       m_fillColor;
};
#endif // EDITLAYER_H
