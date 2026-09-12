#include "mouseselectiondrawer.h"

MouseSelectionDrawer::MouseSelectionDrawer(lay::LayoutViewBase *view, QObject* parent) :
  ShapeDrawer(view, parent)
{
  mp_cursor = new lay::Marker(view, view->active_cellview_index(),4);
  // nice cross-hair style
  mp_cursor->set_frame_color(0x00ff00);
  mp_cursor->set_line_width(1);
  mp_cursor->set_vertex_size(7);
  mp_cursor->set_dither_pattern(1);   // hollow
  mp_cursor->set_line_style(0);

}



void MouseSelectionDrawer::set_new_mouse_position(const db::DPoint& p)
{
  // Calculate the snapped position
  ShapeDrawer::set_new_mouse_position(p);

  db::DPoint s = current_snapped_pos();

         // Desired visual size in pixels (e.g. 9×9 px cross / box)
  const double size_px = 9.0;

         // Convert pixels → microns at the current zoom
  double size_um = pixels_to_micron(size_px);
  if (size_um < 1e-6)
    size_um = 1e-6;

  db::DBox box(s.x() - size_um/2.0,
               s.y() - size_um/2.0,
               s.x() + size_um/2.0,
               s.y() + size_um/2.0);
  db::DCplxTrans unit;
  mp_cursor->set(box, unit);


}




void MouseSelectionDrawer::start_dragging(const db::DPoint&   )
{

}

