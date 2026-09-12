#ifndef CROSSHAIROVERLAY_H
#define CROSSHAIROVERLAY_H

#include <QObject>

#include <QWidget>
#include <QPointF>
#include <QEvent>
#include <QPainter>
#include <QPen>

class CrosshairOverlay : public QWidget {
  Q_OBJECT

public:
  explicit CrosshairOverlay(QWidget* targetWidget)
      : QWidget(targetWidget), m_targetWidget(targetWidget), m_hasMousePos(false)
  {
    // 1. Make the overlay transparent to clicks and clear the native window background
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    // 2. Match initial sizing and track target geometry changes
    if (m_targetWidget) {
      setGeometry(m_targetWidget->rect());
      m_targetWidget->installEventFilter(this);
    }
  }

  enum overlay_type {
    CHO_CROSS = 0x01,
    CHO_HORIZONTAL_LINE = 0x02,
    CHO_VERTICAL_LINE = 0x04,
    CHO_CIRCLE = 0x08} ;



  void updatePosition(const QPointF& localPos, overlay_type type) {
    m_mousePos = localPos;
    m_hasMousePos = true;
    m_type = type;
    update(); // Request a redraw
  }

  void clearCrosshair() {
    m_hasMousePos = false;
    update();
  }


protected:
  // Sync geometry when the target widget moves or resizes
  bool eventFilter(QObject* watched, QEvent* event) override {
    if (watched == m_targetWidget &&
        (event->type() == QEvent::Resize || event->type() == QEvent::Move)) {
      setGeometry(m_targetWidget->rect());
      update();
    }
    return QWidget::eventFilter(watched, event);
  }

  void paintEvent(QPaintEvent* event) override {
    Q_UNUSED(event);
    if (!m_hasMousePos) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

           // Customize the line style (Neon Green, 2px thick)

    int x = static_cast<int>(m_mousePos.x());
    int y = static_cast<int>(m_mousePos.y());

    if (m_type & CHO_CROSS)
    {
      QPen pen(QColor(0, 255, 0), 2, Qt::SolidLine);
      painter.setPen(pen);

      painter.drawLine(x, y - 6 , x, y + 6);
      painter.drawLine(x- 6, y, x+6, y);

    }
    painter.end();
  }

private:
  QWidget* m_targetWidget;
  QPointF m_mousePos;
  bool m_hasMousePos;
  overlay_type m_type = CHO_CROSS;
};

#endif // CROSSHAIROVERLAY_H
