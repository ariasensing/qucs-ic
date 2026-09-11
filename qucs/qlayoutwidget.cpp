#include "qlayoutwidget.h"
#include <QEvent>
#include <QMouseEvent>

qLayoutWidget::qLayoutWidget(db::Manager *mgr, bool editable, lay::Plugin *plugin_parent,
                             QWidget *parent, unsigned int options) :
    lay::LayoutViewWidget(mgr, editable, plugin_parent, parent, options)
{

}


 bool qLayoutWidget::eventFilter(QObject *obj, QEvent *event)
{

   if (event->type() == QEvent::MouseMove ||
       event->type() == QEvent::MouseButtonPress ||
       event->type() == QEvent::MouseButtonRelease)
   {
     QMouseEvent *me = static_cast<QMouseEvent*>(event);

            // Map to the canvas coordinate system if necessary
     QPointF pos = me->position();
     if (obj != this) {
       pos = mapFromGlobal(me->globalPosition());
     }
   }

  return lay::LayoutViewWidget::eventFilter(obj,event);
 }
