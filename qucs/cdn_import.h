#ifndef CDN_IMPORT_H
#define CDN_IMPORT_H
#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "layLayerProperties.h"
#include "layLayoutView.h"

namespace cadence_import {

// ------------------------------------------------------------
// Nested Skill AST node
// ------------------------------------------------------------
struct SkillNode
{
  enum Type { Atom, List } type = Atom;

  std::string atom;
  std::vector<SkillNode> list;

  bool isAtom() const { return type == Atom; }
  bool isList() const { return type == List; }

  const std::string &str() const { return atom; }

  size_t size() const { return list.size(); }
  const SkillNode &operator[](size_t i) const { return list.at(i); }
};

// ------------------------------------------------------------
// Data structures produced by the parser
// ------------------------------------------------------------
struct DisplayPacket
{
  int          stipple    = 1;          // KLayout dither index (1 = hollow)
  int          lineStyle  = 0;
  unsigned int fillColor  = 0x808080;
  unsigned int frameColor = 0x808080;
  int          width      = 1;
  bool         xfill      = false;
};

struct LayerEntry
{
  std::string name;
  std::string purpose;
  int         layer    = -1;
  int         datatype = 0;
  bool        visible  = true;
  bool        valid    = true;
  DisplayPacket display;
};

struct DrfData
{
  std::map<std::string, unsigned int> colors;
  std::map<std::string, int>          stipples;
  std::map<std::string, int>          lineStyles;
  std::map<std::string, int>          widths;
  std::map<std::string, DisplayPacket> packets;
};

struct ImportResult
{
  std::vector<lay::LayerPropertiesNode> nodes;
};

// ------------------------------------------------------------
// Public API
// ------------------------------------------------------------

/**
 * Parse a Cadence .drf file (colors, stipples, line styles, packets).
 * If a LayoutView is supplied, custom stipples and line styles are registered.
 */
DrfData parseDrf(const std::string &path, lay::LayoutView *view = nullptr);

/**
 * Parse a Cadence .layermap / .map file.
 */
std::vector<LayerEntry> parseLayerMap(const std::string &path);

/**
 * High-level import: combine .layermap + .drf → list of LayerPropertiesNode.
 */
ImportResult importCadence(const std::string &layermapPath,
                           const std::string &drfPath,
                           lay::LayoutView *view = nullptr);


} // namespace cadence_import
#endif // CDN_IMPORT_H
