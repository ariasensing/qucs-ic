#include "technologyeditor.h"
#include "ui_technologyeditor.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QCheckBox>
#include <QPainter>
#include "editLayer.h"

/**
 * @brief TechnologyEditor::loadLayers
 * @return
 */
void TechnologyEditor::loadLayers()
{
  QString newlypfile = ui->leLypFile->text();
  if (newlypfile.isEmpty())
    newlypfile = QFileDialog::getOpenFileName(this, "Open Layer Property File","",
                                              tr("KLayout Property File (*.lyp);; All files (*.*)" ) );

  if (newlypfile.isEmpty()) return;

  if (m_editedTech==nullptr) return;
  m_editedTech->import_klayout_layerdefs(newlypfile);
  // Clear prev layers
  m_layoutView->clear_layers();
  m_layoutView->load_layer_props(newlypfile.toStdString());


  listLayers();
}

/**
 * @brief TechnologyEditor::listLayers
 */
void  TechnologyEditor::listLayers()
{
  int current_row = ui->tblLayers->currentRow();
  int current_col = ui->tblLayers->currentColumn();
  ui->tblLayers->clear();
  QStringList columns({tr("Layer num"), tr("Datatype"), tr("Name"), tr("Stipple"), tr("Visible")});
  ui->tblLayers->setColumnCount(columns.count());
  ui->tblLayers->setHorizontalHeaderLabels(columns);

  ui->tblLayers->setCurrentCell(current_row, current_col);
  // Probably there is a smarter way to know how many layers

  unsigned int row = 0;
  for (lay::LayerPropertiesConstIterator it =  m_layoutView->begin_layers(); it != m_layoutView->end_layers(); ++it)
    row++;
  ui->tblLayers->setRowCount( row);

  row = 0;
  for (lay::LayerPropertiesConstIterator it =  m_layoutView->begin_layers(); it != m_layoutView->end_layers(); ++it)
  {

    const lay::LayerPropertiesNode &node = *it;
    lay::ParsedLayerSource source = node.source(true);

    // Retrieve layernum / purpose
    int layer_id = source.layer();
    int purpose_id = source.datatype();
    QString name = QString::fromStdString(node.name());

    QTableWidgetItem* newItem = new QTableWidgetItem();
    newItem->setText(QString::number(layer_id));
    newItem->setFlags(newItem->flags() & ~Qt::ItemIsEditable);
    ui->tblLayers->setItem(row,0, newItem);

    newItem = new QTableWidgetItem();
    newItem->setText(QString::number(purpose_id));
    newItem->setFlags(newItem->flags() & ~Qt::ItemIsEditable);
    ui->tblLayers->setItem(row,1, newItem);

    newItem = new QTableWidgetItem();
    newItem->setText(name);
    newItem->setFlags(newItem->flags() & ~Qt::ItemIsEditable);
    ui->tblLayers->setItem(row,2, newItem);

    // Design stipple
    // Prefer the *effective* values (after inheritance from parents)
    unsigned int fill  = node.eff_fill_color(true);     // 0xAARRGGBB or 0xRRGGBB
    int dither         = node.eff_dither_pattern(true); // always a valid index
    unsigned int frame = node.eff_frame_color(true);

    QTableWidgetItem *previewItem = new QTableWidgetItem;
    previewItem->setData(Qt::DecorationRole,
                         makeLayerPreview(fill,
                                          frame,
                                          dither,
                                          m_layoutView));   // view can be nullptr

    previewItem->setFlags(previewItem->flags() & ~Qt::ItemIsEditable);
    ui->tblLayers->setItem(row, 3, previewItem);

    // Optional: also show the numeric index / color as tooltip
    previewItem->setToolTip(QString("dither=%1  fill=#%2")
                                .arg(dither)
                                .arg(fill, 6, 16, QChar('0')));

    QCheckBox *visible = new QCheckBox();
    visible->setStyleSheet("margin-left:50%; margin-right:50%;");
    visible->setChecked(node.visible(false));
    ui->tblLayers->setCellWidget(row, 4, visible);
    visible->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    visible->setFocusPolicy( Qt::NoFocus);
    row++;
  }
}
/**
 * @brief TechnologyEditor::update_layer_row
 * @param row
 * @param props
 */
void    TechnologyEditor::update_layer_row(int row, const lay::LayerPropertiesNode &props)
{

}

/**
 * @brief TechnologyEditor::makeLayerPreview
 * @param fillColor
 * @param ditherIndex
 * @param view
 * @param size
 * @return
 */

QPixmap TechnologyEditor::makeLayerPreview(unsigned int fillColor, int frameColor, int ditherIndex,
                         lay::LayoutView *view,
                         int size)
{
  QPixmap pix(size, size);
  pix.fill(Qt::transparent);

  QPainter p(&pix);
  p.setRenderHint(QPainter::Antialiasing, false);

         // Background (optional)
  p.fillRect(0, 0, size, size, QColor(40, 40, 40));

         // Convert KLayout color (usually 0xRRGGBB or 0xAARRGGBB)
  QColor fill(QColor::fromRgba(fillColor | 0xff000000));  // force opaque if needed

  if (ditherIndex <= 1) {
    // solid or empty
    if (ditherIndex == 0)
      p.fillRect(2, 2, size-4, size-4, fill);
  }
  else {
    // Try to get a real pattern
    QBitmap mask;
    if (view) {
      // You can parse view->get_stipple(ditherIndex) into a QBitmap
      // (simple 8x8 or 16x16 patterns are common)
      // For a quick solution many people just use a few hard-coded brushes
      // or draw a hatch themselves.
    }

           // Simple fallback hatches (good enough for a table)
    QBrush brush(fill);
    switch (ditherIndex % 8) {
    case 2: brush.setStyle(Qt::Dense4Pattern); break;
    case 3: brush.setStyle(Qt::Dense5Pattern); break;
    case 4: brush.setStyle(Qt::HorPattern);    break;
    case 5: brush.setStyle(Qt::VerPattern);    break;
    case 6: brush.setStyle(Qt::CrossPattern);  break;
    case 7: brush.setStyle(Qt::BDiagPattern);  break;
    default: brush.setStyle(Qt::FDiagPattern); break;
    }
    p.fillRect(2, 2, size-4, size-4, brush);
  }

         // Optional thin frame
  p.setPen(QPen(QColor(QColor::fromRgba(frameColor | 0xff000000)), 1));
  p.drawRect(1, 1, size-3, size-3);

  return pix;
}
/**
 * @brief TechnologyEditor::saveLayers
 */

void TechnologyEditor::saveLayers()
{
  QString newlypfile = ui->leLypFile->text();
  if (newlypfile.isEmpty())
    newlypfile = QFileDialog::getSaveFileName(this, "Save Layer Property File","",
                                              tr("KLayout Property File (*.lyp);; All files (*.*)" ) );
  if (newlypfile.isEmpty()) return;
  m_layoutView->save_layer_props(newlypfile.toStdString());

}

/**
 * @brief TechnologyEditor::createLayer
 */
void      TechnologyEditor::createLayer()
{
  LayerDialog dlg(m_layoutView, this);
  if (dlg.exec() == QDialog::Accepted) {
    // use dlg.layer(), dlg.datatype(), dlg.name(), ...
    unsigned int id = dlg.layer();
    unsigned int dt = dlg.datatype();

    lay::LayerProperties props;
    props.set_name(dlg.name().toStdString());
    props.set_fill_color(dlg.fillColor().rgb());
    props.set_frame_color(dlg.fillColor().rgb());
    props.set_dither_pattern(dlg.ditherPattern());
    props.set_line_style(dlg.lineStyle());

    // Check for existing one
    for (lay::LayerPropertiesConstIterator it =  m_layoutView->begin_layers(); it != m_layoutView->end_layers(); ++it)
    {

      const lay::LayerPropertiesNode &node = *it;
      lay::ParsedLayerSource source = node.source(true);

             // Retrieve layernum / purpose
      int layer_id = source.layer();
      int purpose_id = source.datatype();

      if ((layer_id == id)&&(dt==purpose_id))
      {
        if (QMessageBox::question(this,tr("Confirm"),tr("The layer and datatype already exists. \n Do you want to ovveride?"))
            ==QMessageBox::No)
          return;
        else
        {
          m_layoutView->set_properties(it, props);
          m_layoutView->update_content();
          listLayers();
          return;
        }
      }
    }

    // New
    m_layoutView->insert_layer(m_layoutView->begin_layers(), props);
    listLayers();
  }
}

/**
 * @brief TechnologyEditor::removeLayer
 */
void      TechnologyEditor::removeLayer()
{
  // Get current index
  int row = ui->tblLayers->currentRow();
  bool bok;
  unsigned int id=  ui->tblLayers->item(row,0)->text().toInt(&bok);
  if (!bok) return;
  unsigned int dt=  ui->tblLayers->item(row,1)->text().toInt(&bok);
  if (!bok) return;

         // Check for existing one
  for (lay::LayerPropertiesConstIterator it =  m_layoutView->begin_layers(); it != m_layoutView->end_layers(); ++it)
  {

    const lay::LayerPropertiesNode &node = *it;
    lay::ParsedLayerSource source = node.source(true);

           // Retrieve layernum / purpose
    int layer_id = source.layer();
    int purpose_id = source.datatype();

    if ((layer_id == id)&&(dt==purpose_id))
    {
      m_layoutView->delete_layer(it);
      listLayers();
      return;
    }
  }

}
/**
 * @brief TechnologyEditor::editLayer
 */
void TechnologyEditor::editLayer(const QModelIndex& model)
{
  // Get current index
  int row = ui->tblLayers->currentRow();
  bool bok;
  unsigned int id=  ui->tblLayers->item(row,0)->text().toInt(&bok);
  if (!bok) return;
  unsigned int dt=  ui->tblLayers->item(row,1)->text().toInt(&bok);
  if (!bok) return;

         // Check for existing one
  for (lay::LayerPropertiesConstIterator it =  m_layoutView->begin_layers(); it != m_layoutView->end_layers(); ++it)
  {

    const lay::LayerPropertiesNode &node = *it;
    lay::ParsedLayerSource source = node.source(true);

           // Retrieve layernum / purpose
    int layer_id = source.layer();
    int purpose_id = source.datatype();

    if ((layer_id == id)&&(dt==purpose_id))
    {
      LayerDialog dlg(m_layoutView, *it, this);   // edit mode
      if (dlg.exec() == QDialog::Accepted) {
        // layer() and datatype() stay the same
        // apply the other values back to the node / view
        lay::LayerProperties props = (*it);   // copy
        props.set_name(dlg.name().toStdString());
        props.set_fill_color(dlg.fillColor().rgb());
        props.set_frame_color(dlg.fillColor().rgb());
        props.set_dither_pattern(dlg.ditherPattern());
        props.set_line_style(dlg.lineStyle());

        m_layoutView->set_properties(it, props);
        m_layoutView->update_content();
        listLayers();
        }
    }
  }
}
