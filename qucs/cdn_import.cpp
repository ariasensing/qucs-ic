#include "cdn_import.h"

#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

#include "tlString.h"
#include "tlLog.h"
#include "layLayoutView.h"
#include "dbLayout.h"
#include "dbCell.h"
// CadenceLayerImport.cpp

namespace cadence_import {

// =====================================================================
// Skill Parser (nested)
// =====================================================================

class SkillParser
{
public:
  explicit SkillParser(const std::string &src)
      : m_src(src), m_pos(0), m_len(src.size())
  {}

  std::vector<SkillNode> parse()
  {
    std::vector<SkillNode> result;
    skipWhitespaceAndComments();

    while (m_pos < m_len) {
      result.push_back(parseForm());
      skipWhitespaceAndComments();
    }
    return result;
  }

private:
  const std::string &m_src;
  size_t m_pos;
  size_t m_len;

  void skipWhitespaceAndComments()
  {
    while (m_pos < m_len) {
      char c = m_src[m_pos];
      if (std::isspace(static_cast<unsigned char>(c))) {
        ++m_pos;
        continue;
      }
      if (c == ';') {                         // Cadence comment
        while (m_pos < m_len && m_src[m_pos] != '\n')
          ++m_pos;
        continue;
      }
      break;
    }
  }

  SkillNode parseForm()
  {
    skipWhitespaceAndComments();
    if (m_pos >= m_len)
      throw std::runtime_error("Unexpected end of file while parsing Skill");

    if (m_src[m_pos] == '(')
      return parseList();

    return parseAtom();
  }

  SkillNode parseList()
  {
    ++m_pos; // consume '('

    SkillNode node;
    node.type = SkillNode::List;

    while (true) {
      skipWhitespaceAndComments();
      if (m_pos >= m_len)
        throw std::runtime_error("Unclosed parenthesis in Skill file");

      if (m_src[m_pos] == ')') {
        ++m_pos; // consume ')'
        break;
      }
      node.list.push_back(parseForm());
    }
    return node;
  }

  SkillNode parseAtom()
  {
    SkillNode node;
    node.type = SkillNode::Atom;

    char c = m_src[m_pos];

           // Quoted string
    if (c == '"' || c == '\'') {
      char quote = c;
      ++m_pos;
      while (m_pos < m_len && m_src[m_pos] != quote) {
        if (m_src[m_pos] == '\\' && m_pos + 1 < m_len) {
          node.atom += m_src[m_pos + 1];
          m_pos += 2;
        } else {
          node.atom += m_src[m_pos++];
        }
      }
      if (m_pos < m_len)
        ++m_pos; // closing quote
    }
    else {
      // Ordinary atom
      while (m_pos < m_len) {
        c = m_src[m_pos];
        if (std::isspace(static_cast<unsigned char>(c)) ||
            c == '(' || c == ')' || c == ';')
          break;
        node.atom += c;
        ++m_pos;
      }
    }
    return node;
  }
};

static std::vector<SkillNode> parseSkillFile(const std::string &path)
{
  std::ifstream f(path);
  if (!f)
    throw std::runtime_error("Cannot open file: " + path);

  std::string content((std::istreambuf_iterator<char>(f)),
                      std::istreambuf_iterator<char>());

  SkillParser parser(content);
  return parser.parse();
}

// =====================================================================
// Mapping helpers (built-in patterns only)
// =====================================================================

static int map_stipple_name(const std::string &name)
{
  // Common Cadence names → built-in KLayout dither indices
  // 0 = solid, 1 = hollow, 2+ = various patterns
  std::string n = name;
  std::transform(n.begin(), n.end(), n.begin(),
                 [](unsigned char c){ return std::tolower(c); });

  if (n == "solid")                          return 0;
  if (n == "hollow" || n == "clear" ||
      n == "empty"  || n == "none")          return 1;
  if (n.find("dot")   != std::string::npos)  return 2;
  if (n.find("dash")  != std::string::npos)  return 3;
  if (n.find("cross") != std::string::npos)  return 6;
  if (n.find("diag")  != std::string::npos ||
      n.find("slash") != std::string::npos)  return 7;
  if (n.find("grid")  != std::string::npos)  return 9;

         // default = hollow
  return 1;
}

static int map_line_style_name(const std::string & /*name*/)
{
  // Most Cadence styles map to solid
  return 0;
}

// =====================================================================
// DRF parser
// =====================================================================

DrfData parseDrf(const std::string &path, lay::LayoutView * /*view*/)
{
  DrfData drf;

  std::vector<SkillNode> forms;
  try {
    forms = parseSkillFile(path);
  }
  catch (const std::exception &ex) {
    tl::warn << "Failed to parse DRF " << path << ": " << ex.what();
    return drf;
  }

         // ---------- First pass: colors, line styles, stipples ----------
  for (const auto &form : forms)
  {
    if (!form.isList() || form.size() < 2)
      continue;

    const std::string &tag = form[0].str();

           // Colors
    if (tag == "drDefineColor" && form.size() >= 5)
    {
      try {
        int r = std::stoi(form[2].str());
        int g = std::stoi(form[3].str());
        int b = std::stoi(form[4].str());
        drf.colors[form[1].str()] =
            (static_cast<unsigned>(r) << 16) |
            (static_cast<unsigned>(g) <<  8) |
            static_cast<unsigned>(b);
      }
      catch (...) {}
    }
    // Line styles
    else if (tag == "drDefineLineStyle" && form.size() >= 2)
    {
      const std::string &name = form[1].str();
      int w = 1;
      if (form.size() >= 3) {
        try { w = std::stoi(form[2].str()); } catch (...) {}
      }
      drf.widths[name]     = w;
      drf.lineStyles[name] = map_line_style_name(name);
    }
    // Stipples – only keep the name → built-in index mapping
    else if (tag == "drDefineStipple" && form.size() >= 2)
    {
      const std::string &name = form[1].str();
      drf.stipples[name] = map_stipple_name(name);
    }
  }

         // ---------- Second pass: packets ----------
  for (const auto &form : forms)
  {
    if (!form.isList() || form.size() < 6)
      continue;
    if (form[0].str() != "drDefinePacket")
      continue;

    DisplayPacket pkt;
    const std::string packetName = form[1].str();

           // stipple
    auto sit = drf.stipples.find(form[2].str());
    pkt.stipple = (sit != drf.stipples.end()) ? sit->second : 1;

           // line style
    auto lit = drf.lineStyles.find(form[3].str());
    if (lit != drf.lineStyles.end())
      pkt.lineStyle = lit->second;

           // fill / frame colours
    auto cit = drf.colors.find(form[4].str());
    if (cit != drf.colors.end())
      pkt.fillColor = cit->second;

    cit = drf.colors.find(form[5].str());
    if (cit != drf.colors.end())
      pkt.frameColor = cit->second;

           // optional X-fill
    if (form.size() > 6 &&
        (form[6].str() == "X" || form[6].str() == "x"))
      pkt.xfill = true;

           // width
    auto wit = drf.widths.find(form[3].str());
    if (wit != drf.widths.end())
      pkt.width = wit->second;

    drf.packets[packetName] = pkt;
  }

  return drf;
}

// =====================================================================
// Layer-map parser
// =====================================================================

std::vector<LayerEntry> parseLayerMap(const std::string &path)
{
  std::vector<LayerEntry> entries;
  std::ifstream f(path);
  if (!f)
    return entries;

  std::string line;
  while (std::getline(f, line))
  {
    auto hash = line.find('#');
    if (hash != std::string::npos)
      line.resize(hash);

           // trim
    size_t a = line.find_first_not_of(" \t\r\n");
    if (a == std::string::npos)
      continue;
    size_t b = line.find_last_not_of(" \t\r\n");
    line = line.substr(a, b - a + 1);
    if (line.empty())
      continue;

    std::istringstream iss(line);
    LayerEntry e;
    if (!(iss >> e.name >> e.purpose >> e.layer))
      continue;
    if (!(iss >> e.datatype))
      e.datatype = 0;

    entries.push_back(e);
  }
  return entries;
}

// =====================================================================
// High-level import
// =====================================================================

ImportResult importCadence(const std::string &layermapPath,
                           const std::string &drfPath,
                           lay::LayoutView *view)
{
  ImportResult result;

  DrfData drf = parseDrf(drfPath, view);
  auto mapEntries = parseLayerMap(layermapPath);

  for (const auto &e : mapEntries)
  {
    lay::LayerPropertiesNode node;

    node.set_source(tl::sprintf("%d/%d", e.layer, e.datatype));

    std::string displayName = e.name + "." + e.purpose;
    node.set_name(displayName + " - " +
                  std::to_string(e.layer) + "/" +
                  std::to_string(e.datatype));

           // Look for a matching packet (several naming conventions)
    DisplayPacket pkt;
    auto it = drf.packets.find(displayName);
    if (it == drf.packets.end())
      it = drf.packets.find(e.name);
    if (it == drf.packets.end())
      it = drf.packets.find(e.name + "_" + e.purpose);

    if (it != drf.packets.end())
      pkt = it->second;

    node.set_fill_color(pkt.fillColor);
    node.set_frame_color(pkt.frameColor);
    node.set_dither_pattern(pkt.stipple);
    node.set_line_style(pkt.lineStyle);
    node.set_width(pkt.width);
    node.set_xfill(pkt.xfill);
    node.set_visible(true);
    node.set_valid(true);

    result.nodes.push_back(node);
  }

  return result;
}

void applyCadenceToView(lay::LayoutView *view,
                        const std::string &layermapPath,
                        const std::string &drfPath,
                        bool clearExisting)
{
  if (!view)
    return;

  auto imported = importCadence(layermapPath, drfPath, view);

  if (clearExisting)
    view->clear_layers();

  for (const auto &node : imported.nodes)
    view->insert_layer(view->end_layers(), node);

  view->add_missing_layers();
  view->update_content();
}

} // namespace cadence_import
