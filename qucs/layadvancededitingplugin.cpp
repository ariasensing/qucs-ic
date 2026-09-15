// layAdvancedEditingPlugin.cc
#include "layadvancededitingplugin.h"
#include "layLayoutViewBase.h"
#include "edtShapeService.h"
#include "tlLog.h"   // optional, for tl::info / tl::warn
#include "shapedrawer.h"

QMap<lay::LayoutViewBase*, layAdvancedEditingPlugin*> layAdvancedEditingPlugin::m_mapped_plugins = QMap<lay::LayoutViewBase*, layAdvancedEditingPlugin*> ();

layAdvancedEditingPlugin::layAdvancedEditingPlugin(db::Manager * manager, lay::Dispatcher * /*dispatcher*/, lay::LayoutViewBase *view)
    :
      edt::Service(manager, view, db::ShapeIterator::All),
      mp_view(view),
      m_is_idle(true),
      m_shapeDrawer(nullptr)
{
  if (mp_view!=nullptr)
    m_mapped_plugins[mp_view] = this;

  if ((mp_view!=nullptr)&&(mp_view->widget()!=nullptr))
    mp_view->widget()->setCursor(Qt::BlankCursor);

  init_mode();


}

/**
 * @brief layAdvancedEditingPlugin::get_plugin_from_view
 * @param view
 * @return
 */
layAdvancedEditingPlugin* layAdvancedEditingPlugin::get_plugin_from_view(lay::LayoutViewBase* view)
{
  QMap<lay::LayoutViewBase*, layAdvancedEditingPlugin*>::iterator it = m_mapped_plugins.find(view);
  if (it == m_mapped_plugins.end()) return nullptr;
  return it.value();
}
/**
 * @brief layAdvancedEditingPlugin::~layAdvancedEditingPlugin
 */
layAdvancedEditingPlugin::~layAdvancedEditingPlugin()
{
  m_mapped_plugins.remove(mp_view);
  clear_mouse_cursors();
  clear_partial_selection();
}
/**
 * @brief layAdvancedEditingPlugin::activated
 */
void layAdvancedEditingPlugin::activated()
{
  // Example: change cursor when the tool is selected
  if (mp_view) {
    set_cursor(lay::Cursor::cross);
  }
  tl::info << "layAdvancedEditingPlugin activated";
}
/**
 * @brief layAdvancedEditingPlugin::deactivated
 */
void layAdvancedEditingPlugin::deactivated()
{
  if (mp_view) {
    set_cursor(lay::Cursor::arrow);
  }
  tl::info << "layAdvancedEditingPlugin deactivated";
  m_dragging = false;
}

/**
 * @brief MagneticCursorPlugin::drag_cancel
 */
void layAdvancedEditingPlugin::drag_cancel()
{
}
/**
 * @brief layAdvancedEditingPlugin::update
 */
void layAdvancedEditingPlugin::update()
{
  // Called when the view (layers, cell, etc.) changes
}
/**
 * @brief layAdvancedEditingPlugin::enter_event
 * @param prio
 * @return
 */
bool layAdvancedEditingPlugin::enter_event (bool prio)
{
  if (!mp_view) return false;

  if ((mp_view->widget()!=nullptr)&&(!mp_view->widget()->hasFocus()))
  {
    mp_view->widget()->setFocusPolicy(Qt::StrongFocus);
    mp_view->widget()->setFocus();
  }
  return true;
}
/**
 * @brief layAdvancedEditingPlugin::leave_event
 * @return
 */
bool layAdvancedEditingPlugin::leave_event(bool /*prio*/)
{
  clear_mouse_cursors();
  return false;
}

/**
 * @brief layAdvancedEditingPlugin::mouse_move_event
 * @param p
 * @param prio
 * @return
 */

bool layAdvancedEditingPlugin::mouse_move_event(const db::DPoint &p, unsigned int buttons, bool prio)
{
  if (!mp_view) return false;

  m_last_snapped = snap_to_grid(p);

  if (m_magnetic)
    m_last_snapped = snap2(m_last_snapped);

  emit  update_mouse_position(m_last_snapped);

  if (mode_mouse_move_event)
    return (this->*mode_mouse_move_event)(m_last_snapped,buttons,prio);

  return false;
}
/**
 * @brief layAdvancedEditingPlugin::mouse_press_event
 * @param p
 * @param buttons
 * @param prio
 * @return
 */
bool layAdvancedEditingPlugin::mouse_press_event(const db::DPoint &/*p*/, unsigned int buttons, bool prio)
{
  if (mode_mouse_press_event)
   return (this->*mode_mouse_press_event)(m_last_snapped,buttons,prio);

  return false;
}
/**
 * @brief layAdvancedEditingPlugin::mouse_click_event
 * @return
 */
bool layAdvancedEditingPlugin::mouse_click_event(const db::DPoint &/* p*/, unsigned int buttons, bool prio)
{

  if (mode_mouse_click_event)
    return (this->*mode_mouse_click_event)(m_last_snapped,buttons,prio);

  return false;
}
/**
 * @brief layAdvancedEditingPlugin::mouse_double_click_event
 * @return
 */
bool layAdvancedEditingPlugin::mouse_double_click_event(const db::DPoint &/* p*/, unsigned int buttons, bool prio)
{
  if (mode_mouse_double_click_event)
    return (this->*mode_mouse_double_click_event)(m_last_snapped, buttons,prio);

  return false;
}
/**
 * @brief layAdvancedEditingPlugin::mouse_release_event
 * @return
 */
bool layAdvancedEditingPlugin::mouse_release_event(const db::DPoint &/*p*/, unsigned int buttons, bool prio)
{
  if (mode_mouse_release_event)
    return (this->*mode_mouse_release_event)(m_last_snapped,buttons,prio);

  return false;
}
/**
 * @brief layAdvancedEditingPlugin::wheel_event
 * @return
 */
bool layAdvancedEditingPlugin::wheel_event(int /*delta*/, bool /*horizontal*/,
                           const db::DPoint & /*p*/, unsigned int /*buttons*/, bool /*prio*/)
{
  return false;
}
/**
 * @brief layAdvancedEditingPlugin::configure
 * @param name
 * @param value
 * @return
 */
bool layAdvancedEditingPlugin::configure(const std::string &name, const std::string &value)
{
  // Handle configuration keys you registered in the factory
  // return true if you consumed the key
  (void)name;
  (void)value;
  return false;
}
/**
 * @brief layAdvancedEditingPlugin::config_finalize
 */
void layAdvancedEditingPlugin::config_finalize()
{
  // Called after a batch of configure() calls
}
/**
 * @brief layAdvancedEditingPlugin::menu_activated
 * @param symbol
 */
void layAdvancedEditingPlugin::menu_activated(const std::string &symbol)
{
  // Handle menu symbols that target this plugin instance
  (void)symbol;
}

/**
 * @brief layAdvancedEditingPlugin::micron_to_pixel
 * @param micron_pos
 * @return
 */
QPointF layAdvancedEditingPlugin::micron_to_pixel(const db::DPoint& micron_pos)
{
  return micron_to_pixel(mp_view, micron_pos);
}
/**
 * @brief layAdvancedEditingPlugin::terminate_action Terminate whatever action was on-going
 */
void        layAdvancedEditingPlugin::terminate_action()
{

  // Terminate current operation and start common (selection) mode

  if (mode_abort)
    if (m_current_operation!=Unknown)
      (this->*mode_abort)();

  m_current_operation = Unknown;
  selection_start();
}

/**
 * @brief layAdvancedEditingPlugin::zoom_on_new_position
 * @param x
 * @param y
 */
void   layAdvancedEditingPlugin::zoom_on_new_position(double xc, double yc)
{
  if (!mp_view) return;
  double w = mp_view->box().width();
  double h = mp_view->box().height();

  mp_view->zoom_box(db::DBox(xc-w/2, yc-h/2, xc+w/2, yc+h/2));

}

/**
 * @brief layAdvancedEditingPlugin::set_new_coords
 * @param x
 * @param y
 */
void   layAdvancedEditingPlugin::set_new_coords(double , double )
{

}
/**
 * @brief layAdvancedEditingPlugin::set_new_value
 * @param x
 */
void   layAdvancedEditingPlugin::set_new_value(double )
{

}


// Helper function
QPointF layAdvancedEditingPlugin::micron_to_pixel(lay::LayoutViewBase* view, const db::DPoint &micronPos)
{
  if (!view || !view->widget())
    return QPointF();

  db::DCplxTrans micron2pixel = view->viewport().trans();
  db::DPoint pixel = micron2pixel * micronPos;

  QWidget *canvas = view->widget();
  int x = static_cast<int>(std::round(pixel.x()));
  int y = canvas->height() - 1 - static_cast<int>(std::round(pixel.y()));

  return QPointF(x, y);
}

/**
 * @brief layAdvancedEditingPlugin::get_snapped_pos
 * @return
 */
db::DPoint& layAdvancedEditingPlugin::get_snapped_pos()
{
  return m_last_snapped;
}

/**
 * @brief layAdvancedEditingPlugin::snap_to_grid
 * @param p
 * @return
 */
db::DPoint layAdvancedEditingPlugin::snap_to_grid(const db::DPoint &p) const
{
  double g = grid_micron();

  if (g <= 0.0)
    return p;

  double x = std::round(p.x() / g) * g;
  double y = std::round(p.y() / g) * g;
  return db::DPoint(x, y);
}

/**
 * @brief layAdvancedEditingPlugin::grid_micron
 * @return
 */

double layAdvancedEditingPlugin::grid_micron() const
{
  if (mp_view->cellviews()==0) return 0.001;
  db::Layout* layout = &(mp_view->active_cellview()->layout());

  if (layout==nullptr) return 0.001;
  return layout->dbu();
}

/**
 * @brief layAdvancedEditingPlugin::clear_partial_selection
 */
void layAdvancedEditingPlugin::clear_partial_selection()
{
  m_partial_selection.clear();
  mp_view->clear_selection();
  clear_mouse_cursors();
}

/**
 * @brief layAdvancedEditingPlugin::select_at_point
 * @param p
 * @param add
 */
void layAdvancedEditingPlugin::select_at_point(const db::DPoint &p, bool add)
{
  if (!add)
    clear_partial_selection();

  db::DBox search_box(p, p);
  search_box.enlarge(db::vector(m_pick_tolerance_um,m_pick_tolerance_um));

  std::vector<PartialSelection> found;

  // Only the active cellview
  int cv_index = mp_view->active_cellview_index();
  if (cv_index < 0)
    return;

  const lay::CellView &cv = mp_view->active_cellview();

  db::Layout &layout = cv->layout();
  db::Cell &cell = *(cv.cell());                 // <-- current cell only
  double dbu = layout.dbu();
  db::CplxTrans to_micron(dbu);

  // Search box in database units
  db::Box db_box = db::Box(search_box.transformed(db::VCplxTrans(1.0 / dbu)));

  // Visible layers of this cellview
  for (lay::LayerPropertiesConstIterator l = mp_view->begin_layers();
       !l.at_end(); ++l)
  {
    if (l->cellview_index() != cv_index || !l->visible(true) || l->layer_index() < 0)
      continue;

    unsigned int layer = (unsigned int)l->layer_index();

    // Non-recursive iterator – only shapes in the current cell
    for (db::ShapeIterator si = cell.begin_overlapping(layer,db_box, db::ShapeIterator::All);
         !si.at_end(); ++si)
    {
      collect_partials_from_shape(*si, layer, to_micron, search_box,
                                  m_pick_tolerance_um,
                                  cv_index, cell.cell_index(), found);
    }
  }

  // For a pure click keep only the closest hit
  if (!found.empty() && !add) {
    std::sort(found.begin(), found.end(),
              [&p](const PartialSelection &a, const PartialSelection &b) {
                double da = a.is_vertex() ? a.vertex.distance(p)
                                          : a.edge.distance(p);
                double db = b.is_vertex() ? b.vertex.distance(p)
                                          : b.edge.distance(p);
                return da < db;
              });
    m_partial_selection.push_back(found.front());
  } else {
    m_partial_selection.insert(m_partial_selection.end(),
                               found.begin(), found.end());

  }
}

/**
 * @brief layAdvancedEditingPlugin::select_in_box
 * @param box
 * @param add
 */
void layAdvancedEditingPlugin::select_in_box(const db::DBox &box, bool add)
{
  if (!add)
    clear_partial_selection();

  std::vector<PartialSelection> found;

  int cv_index = mp_view->active_cellview_index();
  if (cv_index < 0)
    return;

  const lay::CellView &cv = mp_view->cellview(cv_index);
  if (!cv.is_valid())
    return;

  db::Layout &layout = cv->layout();
  db::Cell &cell = *(cv.cell());                 // <-- current cell only
  double dbu = layout.dbu();
  db::CplxTrans to_micron(dbu);

  db::Box db_box = db::Box(box.transformed(db::VCplxTrans(1.0 / dbu)));

  for (lay::LayerPropertiesConstIterator l = mp_view->begin_layers();
       !l.at_end(); ++l)
  {
    if (l->cellview_index() != cv_index || !l->visible(true) || l->layer_index() < 0)
      continue;

    unsigned int layer = (unsigned int)l->layer_index();

    for (db::ShapeIterator si = cell.begin_overlapping(layer,db_box, db::ShapeIterator::All);
         !si.at_end(); ++si)
    {
      collect_partials_from_shape(*si, layer, to_micron, box,
                                  0.0 /* exact box */,
                                  cv_index, cell.cell_index(), found);
    }
  }

  m_partial_selection.insert(m_partial_selection.end(),
                             found.begin(), found.end());

}

// --------------------------------------------------------------------------
// Geometry helpers
// --------------------------------------------------------------------------

db::DPolygon
layAdvancedEditingPlugin::shape_to_dpolygon(const db::Shape &shape, const db::CplxTrans &tr)
{
  // 1. Get the shape as an integer polygon
  db::Polygon poly;
  if (shape.is_polygon() || shape.is_simple_polygon()) {
    shape.polygon(poly);
  }
  else if (shape.is_path()) {
    db::Path path;
    shape.path(path);
    poly = path.polygon();               // or path.simple_polygon()
  }
  else if (shape.is_box()) {
    poly = db::Polygon(shape.box());
  }
  else {
    return db::DPolygon();               // unsupported shape type
  }

  // 3. Apply transform
  return poly.transformed(tr);
}
/**
 * @brief layAdvancedEditingPlugin::extract_edges_and_vertices
 * @param poly
 * @param edges
 * @param vertices
 */
void layAdvancedEditingPlugin::extract_edges_and_vertices(
    const db::DPolygon &poly,
    std::vector<db::DEdge>  &edges,
    std::vector<db::DPoint> &vertices)
{
  edges.clear();
  vertices.clear();

  // hull
  for (auto e = poly.begin_edge(); !e.at_end(); ++e) {
    edges.push_back(*e);
    vertices.push_back((*e).p1());
  }
  // holes (if any)
  for (unsigned h = 0; h < poly.holes(); ++h) {
    for (auto e = poly.begin_edge(h); !e.at_end(); ++e) {
      edges.push_back(*e);
      vertices.push_back((*e).p1());
    }
  }
}



/**
 * @brief layAdvancedEditingPlugin::key_event_x This is not actually an event handler since it is called by
 * the icLyaout QDialog directly
 * @param key
 * @param buttons
 * @return
 */
bool layAdvancedEditingPlugin::key_event (unsigned int key, unsigned int buttons)
{
  /*
   * The zoom shortcuts are preserved across each mode. So they are handled here
   * */
  if (key == 'F')
  {
    if (mp_view!=nullptr)
      mp_view->zoom_fit();
    // Full zoom
    return true;
  }
  if (key == '+')
  {
    if (mp_view!=nullptr)
      mp_view->zoom_in();
    // Full zoom
    return true;
  }
  if (key == '-')
  {
    if (mp_view!=nullptr)
      mp_view->zoom_out();
    // Full zoom
    return true;
  }


  //
  if (mode_key_event)
    return (this->*mode_key_event)(key,buttons);

  return false;
}

/**
 * @brief layAdvancedEditingPlugin::init_mode
 */
void  layAdvancedEditingPlugin::init_mode()
{
  selection_start();
}