// layAdvancedEditingPlugin.cc
#include "layadvancededitingplugin.h"
#include "layLayoutViewBase.h"
#include "edtShapeService.h"
#include "tlLog.h"   // optional, for tl::info / tl::warn
#include "shapedrawer.h"


/**
 * @brief layAdvancedEditingPlugin::selection_start
 */
void        layAdvancedEditingPlugin::selection_start()
{
  if (m_current_operation==Selection)
    return;

  if (m_current_operation!=Unknown)
    terminate_action();

  m_current_operation = Selection;

  mode_start                    = &layAdvancedEditingPlugin::selection_start;
  mode_complete                 = &layAdvancedEditingPlugin::selection_complete;
  mode_abort                    = &layAdvancedEditingPlugin::selection_abort;
  mode_mouse_move_event         = &layAdvancedEditingPlugin::selection_mouse_move;
  mode_mouse_click_event        = &layAdvancedEditingPlugin::selection_mouse_click;
  mode_mouse_double_click_event = &layAdvancedEditingPlugin::selection_mouse_double_click;
  mode_mouse_press_event        = &layAdvancedEditingPlugin::selection_mouse_press;
  mode_drag_cancel              = &layAdvancedEditingPlugin::selection_drag_cancel;
  mode_key_event                = &layAdvancedEditingPlugin::selection_key_event;
  mode_mouse_release_event      = &layAdvancedEditingPlugin::selection_mouse_release;

  clear_mouse_cursors();

  if (mp_view) {
    set_cursor(lay::Cursor::cross);
  }

}

/**
 * @brief layAdvancedEditingPlugin::selection_terminate
 */

void        layAdvancedEditingPlugin::selection_complete()
{
  // Keep the selection. Don't do anything since we stay in the same mode
}

/**
 * @brief layAdvancedEditingPlugin::selection_abort
 */
void        layAdvancedEditingPlugin::selection_abort()
{
  clear_partial_selection();
}
/**
 * @brief layAdvancedEditingPlugin::selection_mouse_click
 * @param p
 * @param buttons
 */
bool        layAdvancedEditingPlugin::selection_mouse_click(const db::DPoint &p, unsigned int buttons, bool /*prio*/ )
{
  bool add = (buttons & lay::ShiftButton) != 0;   // Shift = add to selection
  if (!add)
    clear_partial_selection();
  else
  {

  }
  return false;
}
/**
 * @brief layAdvancedEditingPlugin::selection_mouse_double_click
 * @param p
 * @param buttons
 */
bool        layAdvancedEditingPlugin::selection_mouse_double_click(const db::DPoint &p, unsigned int buttons, bool /*prio*/ )
{
  return false;
}

/**
 * @brief layAdvancedEditingPlugin::selection_mouse_press
 * @param p
 * @param buttons
 * @param prio
 * @return
 */
bool        layAdvancedEditingPlugin::selection_mouse_press(const db::DPoint &p, unsigned int buttons, bool prio)
{
  if (buttons & lay::LeftButton) {
    m_dragging   = true;
    m_drag_start = p;
    m_drag_box   = db::DBox(p, p);
    return true;
  }
  return false;
}

/**
 * @brief layAdvancedEditingPlugin::selection_mouse_release
 * @param p
 * @param buttons
 * @param prio
 * @return
 */
bool        layAdvancedEditingPlugin::selection_mouse_release(const db::DPoint &p, unsigned int buttons, bool prio)
{
    if (!m_dragging)
      return false;

    m_dragging = false;

    bool add = (buttons & lay::ShiftButton) != 0;   // Shift = add to selection

    db::DBox box(m_drag_start, p);
    if (box.empty() || box.width() < 1e-6 || box.height() < 1e-6) {
      // pure click
      select_at_point(p, add);
    } else {
      // rubber-box
      select_in_box(box, add);
    }

    update_cursor_markers();
    return true;
}

/**
 * @brief layAdvancedEditingPlugin::selection_drag_cancel
 */
void        layAdvancedEditingPlugin::selection_drag_cancel()
{

}

/**
 * @brief layAdvancedEditingPlugin::selection_mouse_move
 * @param p
 * @param buttons
 * @return
 */

bool        layAdvancedEditingPlugin::selection_mouse_move(const db::DPoint &p, unsigned int buttons, bool )
{
  // Check if left button is kept down
  if ((buttons & lay::LeftButton))
  {
    if (!m_dragging)
    {
      m_dragging   = true;
      m_drag_start = m_last_snapped;
      m_drag_box   = db::DBox(m_last_snapped, m_last_snapped);
    }
  }

  update_cursor_markers();

  return false;
}


/**
 * @brief layAdvancedEditingPlugin::selection_key_event
 * @param key
 * @param buttons
 * @return
 */
bool        layAdvancedEditingPlugin::selection_key_event(unsigned int key, unsigned int /*buttons*/)
{
  if (key=='A'){}
    // Select all shapes
  //  select(mp_view->active_cellview()->layout().get

}

/**
 * @brief layAdvancedEditingPlugin::collect_partials_from_shape
 * @param iter
 * @param search_box
 * @param pick_tol_um
 * @param out
 */
void layAdvancedEditingPlugin::collect_partials_from_shape(
    const db::Shape &shape,
    unsigned int layer,
    const db::CplxTrans &to_micron,
    const db::DBox &search_box,
    double pick_tol_um,
    int cv_index,
    db::cell_index_type cell_index,
    std::vector<PartialSelection> &out)
{
  if (!(shape.is_polygon() || shape.is_path() || shape.is_box() ||
        shape.is_simple_polygon()))
    return;

  db::DPolygon poly = shape_to_dpolygon(shape, to_micron);

  std::vector<db::DEdge>  edges;
  std::vector<db::DPoint> vertices;
  extract_edges_and_vertices(poly, edges, vertices);

         // Build a top-level ObjectInstPath (no hierarchy)
  lay::ObjectInstPath path;
  path.set_cv_index(cv_index);
  path.set_topcell(cell_index);
  path.set_layer(layer);
  path.set_shape(shape);
  // Search for entire polygon

  db::DBox aabb = poly.box();
  if ((search_box.bottom()<=aabb.bottom())&&(search_box.top()>=aabb.top())&&
      (search_box.left()<=aabb.left())&&(search_box.right()>=aabb.right()))
  {
    PartialSelection ps;
    ps.path         = path;
    ps.edge_index   = -1;
    ps.vertex_index = -1;
    out.push_back(ps);
    // Put the object selected into the main selection
    select(path, lay::Editable::Add);
    return;
  }
  // path remains empty → object lives in the top cell

         // ----- vertices -----
  for (size_t i = 0; i < vertices.size(); ++i) {
    const db::DPoint &v = vertices[i];
    bool hit = false;
    if (pick_tol_um > 0.0)
      hit = (v.distance(search_box.center()) <= pick_tol_um);
      else
      hit = search_box.contains(v);

    if (hit) {
      PartialSelection ps;
      ps.path         = path;
      ps.edge_index   = -1;
      ps.vertex_index = int(i);
      ps.vertex       = v;
      out.push_back(ps);
    }
  }

         // ----- edges -----
  for (size_t i = 0; i < edges.size(); ++i) {
    const db::DEdge &e = edges[i];
    bool hit = false;
    if (pick_tol_um > 0.0)
      hit = (e.distance(search_box.center()) <= pick_tol_um);
      else
      hit = search_box.contains(e.p1()) && search_box.contains(e.p2());

    if (hit) {
      PartialSelection ps;
      ps.path         = path;
      ps.edge_index   = int(i);
      ps.vertex_index = -1;
      ps.edge         = e;
      out.push_back(ps);
    }
  }
}

/**
 * @brief layAdvancedEditingPlugin::visualize_partial_selection
 */
void layAdvancedEditingPlugin::visualize_partial_selection()
{
  for (const auto &ps : m_partial_selection) {
    if (ps.is_vertex()) {
      add_mouse_cursor(ps.vertex, false);          // strong point marker
    } else if (ps.is_edge()) {
      add_edge_marker(ps.edge, false);             // strong edge marker
    }
  }
}


/**
 * @brief layAdvancedEditingPlugin::update_cursor_markers
 */
void  layAdvancedEditingPlugin::update_cursor_markers()
{
  clear_mouse_cursors();
  add_mouse_cursor(m_last_snapped, false);
  visualize_partial_selection();
}

/**
 * @brief layAdvancedEditingPlugin::clear_hovering_list
 */
void  layAdvancedEditingPlugin::clear_hovering_list()
{

}

/**
 * @brief layAdvancedEditingPluing::update_hovering_list
 * @param pt
 */
void  layAdvancedEditingPlugin::update_hovering_list(const db::DPoint& pt)
{

}

