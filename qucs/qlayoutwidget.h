#ifndef QLAYOUTWIDGET_H
#define QLAYOUTWIDGET_H
#include "layLayoutView_qt.h"

class qLayoutWidget : public lay::LayoutViewWidget {
public:
  qLayoutWidget(db::Manager *mgr, bool editable, lay::Plugin *plugin_parent, QWidget *parent = 0,
                unsigned int options = (unsigned int) lay::LayoutView::LV_Normal);

  virtual bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // QLAYOUTWIDGET_H
