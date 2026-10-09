#include "custom_card.h"
#include "icons.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace esphome::custom_card {

using tessera::Font;
namespace ui = tessera::ui;

// ---- What a template sensor describes ----
//
// The sensor's attribute `elements` is a list of texts, one part each, its fields split by "|":
//
//   value|<value>|<unit>|<label>|<colour>|<icon>
//   bar|<value>|<unit>|<label>|<colour>|<icon>|<min>|<max>
//   text|<words>|||<colour>|<icon>
//
// Fields at the end may be left out. A colour is one of the key colours of theme.h (red, orange, yellow, green, mint,
// blue, purple, pink, gray), the same in light and dark; an icon is a name of Tessera's set (icons.h). Values that
// follow each other stand side by side, four at the most; a bar and a text take a line of their own.

static constexpr int MAX_PARTS = 6;
static constexpr int MAX_IN_ROW = 4;

enum class Kind : uint8_t { VALUE, BAR, TEXT };

struct Part {
  Kind kind = Kind::VALUE;
  std::string value, unit, label;
  uint32_t colour = 0;  // a key colour of theme.h, 0 for the card's own paint
  uint32_t icon = 0;    // a codepoint of Tessera's set, 0 for none
  float min = 0, max = 100;
};

static std::string trim(const std::string &text) {
  const size_t start = text.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) return "";
  return text.substr(start, text.find_last_not_of(" \t\r\n") - start + 1);
}

static std::vector<std::string> fields_of(const std::string &line) {
  std::vector<std::string> out;
  size_t start = 0;
  while (true) {
    const size_t bar = line.find('|', start);
    out.push_back(trim(line.substr(start, bar == std::string::npos ? std::string::npos : bar - start)));
    if (bar == std::string::npos) return out;
    start = bar + 1;
  }
}

// An icon of Tessera's set by its name, with or without "mdi:"; 0 for one the screen's font does not hold.
static uint32_t icon_of(std::string name) {
  if (name.rfind("mdi:", 0) == 0) name = name.substr(4);
  if (name.empty()) return 0;
  const IconName *first = std::begin(ICON_NAMES), *last = std::end(ICON_NAMES);
  const IconName *at = std::lower_bound(first, last, name.c_str(),
                                        [](const IconName &icon, const char *key) { return strcmp(icon.name, key) < 0; });
  return at != last && name == at->name ? at->codepoint : 0;
}

// "21.4", "21,4" or " 820 " as a number; false for words (unknown, unavailable).
static bool number_of(std::string text, float *out) {
  std::replace(text.begin(), text.end(), ',', '.');
  text = trim(text);
  if (text.empty()) return false;
  char *end = nullptr;
  const float value = strtof(text.c_str(), &end);
  if (end == text.c_str() || *end != '\0' || !std::isfinite(value)) return false;
  *out = value;
  return true;
}

static bool parse(const std::string &line, Part *part) {
  const std::vector<std::string> f = fields_of(line);
  auto at = [&](size_t i) { return i < f.size() ? f[i] : std::string(); };
  if (f[0] == "value") part->kind = Kind::VALUE;
  else if (f[0] == "bar") part->kind = Kind::BAR;
  else if (f[0] == "text") part->kind = Kind::TEXT;
  else return false;
  part->value = at(1);
  part->unit = at(2);
  part->label = at(3);
  part->colour = theme::key_swatch(at(4));
  part->icon = icon_of(at(5));
  float n;
  if (number_of(at(6), &n)) part->min = n;
  if (number_of(at(7), &n)) part->max = n;
  return true;
}

// ---- The tile ----

class CardTile : public tessera::Tile {
 public:
  explicit CardTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    width_ = c.width;
    height_ = c.height;
    name_ = c.name ? c.name : "";
    head_ = ui::label(c.parent, Font::TITLE, theme::INK);
    note_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    lv_obj_set_style_text_align(note_, LV_TEXT_ALIGN_CENTER, 0);
    for (Slot &s : slots_) {
      s.icon = ui::label(c.parent, Font::ICON_SMALL, theme::MUTED);
      s.label = ui::label(c.parent, Font::BODY, theme::MUTED);
      s.value = ui::label(c.parent, Font::VALUE, theme::INK);
      s.unit = ui::label(c.parent, Font::BODY, theme::MUTED);
      s.track = ui::block(c.parent, theme::TRACK);
      s.fill = ui::block(s.track, theme::ACCENT);
    }
    hide_all();
  }

  void on_state(JsonObjectConst data) override {
    parts_.clear();
    note_text_.clear();
    shown_name_ = !name_.empty() ? name_ : std::string(data["name"] | "");
    const std::string wait = data["wait"] | "";
    JsonVariantConst elements = data["attributes"]["elements"];
    bool described = false;
    if (elements.is<JsonArrayConst>()) {
      described = true;
      for (JsonVariantConst line : elements.as<JsonArrayConst>()) {
        Part part;
        if (parts_.size() < MAX_PARTS && line.is<const char *>() && parse(line.as<const char *>(), &part))
          parts_.push_back(part);
      }
    } else if (elements.is<const char *>()) {
      described = true;
      Part part;
      if (parse(elements.as<const char *>(), &part)) parts_.push_back(part);
    }
    const std::string state = data["state"] | "";
    if (!wait.empty()) {
      parts_.clear();
      note_text_ = plugin_->text(wait == "wrong_entity" ? "wrong_entity" : "waiting");
    } else if (described && parts_.empty()) {
      note_text_ = plugin_->text("no_elements");
    } else if (!described && state.empty()) {
      note_text_ = plugin_->text("choose");
    } else if (!described) {
      // A sensor without `elements`: its own state and unit, as a plain value.
      Part part;
      part.value = state;
      part.unit = data["attributes"]["unit_of_measurement"] | "";
      parts_.push_back(part);
    }
    layout();
    paint();
  }

  void on_theme() override { paint(); }

 private:
  struct Slot {
    lv_obj_t *icon{}, *label{}, *value{}, *unit{}, *track{}, *fill{};
  };
  struct Line {
    Kind kind;
    int first, count;
  };

  static void show(lv_obj_t *obj, bool shown) {
    if (shown) lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
  }

  void hide_all() {
    show(head_, false);
    show(note_, false);
    for (Slot &s : slots_)
      for (lv_obj_t *obj : {s.icon, s.label, s.value, s.unit, s.track}) show(obj, false);
  }

  // Values in a row, bars and texts alone.
  std::vector<Line> lines() const {
    std::vector<Line> out;
    for (int i = 0; i < (int) parts_.size(); ++i) {
      const Kind kind = parts_[i].kind;
      if (kind == Kind::VALUE && !out.empty() && out.back().kind == Kind::VALUE && out.back().count < MAX_IN_ROW)
        ++out.back().count;
      else
        out.push_back({kind, i, 1});
    }
    return out;
  }

  int row_height() const { return std::max(ui::line_height(Font::BODY), ui::line_height(Font::ICON_SMALL)); }
  int bar_thickness() const { return ui::px(ui::large() ? 10 : 6); }
  int gap() const { return ui::px(ui::large() ? 8 : 4); }
  int column_gap() const { return ui::px(ui::large() ? 16 : 8); }
  int column_width(int count) const { return (width_ - (count - 1) * column_gap()) / count; }
  bool has_words(const Line &line) const {
    for (int i = line.first; i < line.first + line.count; ++i)
      if (!parts_[i].label.empty() || parts_[i].icon) return true;
    return false;
  }

  int line_height(const Line &line, Font face, bool labels) const {
    switch (line.kind) {
      case Kind::VALUE: return (labels && has_words(line) ? row_height() : 0) + ui::line_height(face);
      case Kind::BAR: return ui::line_height(Font::BODY_LARGE) + ui::px(4) + bar_thickness();
      default: return std::max(ui::line_height(Font::BODY_LARGE), ui::line_height(Font::ICON_SMALL));
    }
  }

  int unit_width(const Part &part, Font unit_face) const {
    return part.unit.empty() ? 0 : ui::px(3) + ui::text_width(part.unit, unit_face);
  }

  // A value's icon in front of it, when there is no row for the words above it: the tile's icon size where the
  // value is that tall, else the small one.
  static Font inline_icon(Font face) {
    return ui::line_height(Font::ICON) <= ui::line_height(face) ? Font::ICON : Font::ICON_SMALL;
  }
  int icon_room(const Part &part, Font face) const {
    return part.icon ? ui::text_width(ui::icon(part.icon), inline_icon(face)) + ui::px(4) : 0;
  }

  bool fits(const std::vector<Line> &all, Font face, bool head, bool labels) const {
    int used = head ? ui::line_height(Font::TITLE) + gap() : 0;
    for (const Line &line : all) {
      if (line.kind == Kind::VALUE) {
        const int column = column_width(line.count);
        for (int i = line.first; i < line.first + line.count; ++i)
          if ((labels ? 0 : icon_room(parts_[i], face)) + ui::text_width(parts_[i].value, face) +
                  unit_width(parts_[i], Font::BODY) > column)
            return false;
      }
      used += line_height(line, face, labels);
    }
    used += gap() * ((int) all.size() - 1);
    return used <= height_;
  }

  // Lay the parts out: the largest value font that fits; words give way before numbers, the name last.
  void layout() {
    hide_all();
    if (!note_text_.empty()) {
      const int h = ui::line_height(Font::BODY);
      lv_obj_set_size(note_, width_, h);
      lv_obj_set_pos(note_, 0, (height_ - h) / 2);
      ui::set_text(note_, note_text_);
      show(note_, true);
      return;
    }
    const std::vector<Line> all = lines();
    const bool named = !shown_name_.empty();
    struct Try { bool head, labels; };
    const Try tries[] = {{named, true}, {named, false}, {false, false}};
    const Font large[] = {Font::VALUE, Font::HEADLINE, Font::TITLE};
    const Font every[] = {Font::VALUE, Font::HEADLINE, Font::TITLE, Font::BODY_LARGE, Font::BODY};
    Font face = Font::BODY;
    Try chosen = tries[2];
    bool found = false;
    for (int pass = 0; pass < 2 && !found; ++pass) {
      for (const Try &t : tries) {
        const Font *first = pass ? std::begin(every) : std::begin(large);
        const Font *last = pass ? std::end(every) : std::end(large);
        for (const Font *f = first; f != last && !found; ++f)
          if (fits(all, *f, t.head, t.labels)) {
            face = *f;
            chosen = t;
            found = true;
          }
        if (found) break;
      }
    }

    int y = 0;
    if (chosen.head) {
      const int h = ui::line_height(Font::TITLE);
      lv_obj_set_size(head_, width_, h);
      lv_obj_set_pos(head_, 0, 0);
      ui::set_text(head_, shown_name_);
      show(head_, true);
      y = h + gap();
    }
    int content = 0;
    for (const Line &line : all) content += line_height(line, face, chosen.labels);
    const int spare = std::max(0, height_ - y - content);
    const int n = (int) all.size();
    const int between = n > 1 ? std::max(gap(), spare / (n + 1)) : 0;
    y += std::max(0, (spare - between * (n - 1)) / 2);
    for (const Line &line : all) {
      place(line, face, chosen.labels, y);
      y += line_height(line, face, chosen.labels) + between;
    }
  }

  // An icon and words beside it, centred on a row of height `h` at (x, y), at most `room` wide.
  int place_words(Slot &s, const Part &part, int x, int y, int h, int room, Font words_face, lv_obj_t *words,
                  const std::string &text) {
    int left = x;
    if (part.icon) {
      const std::string glyph = ui::icon(part.icon);
      const int w = ui::text_width(glyph, Font::ICON_SMALL), ih = ui::line_height(Font::ICON_SMALL);
      ui::set_font(s.icon, Font::ICON_SMALL);
      ui::set_text(s.icon, glyph);
      lv_obj_set_size(s.icon, w, ih);
      lv_obj_set_pos(s.icon, x, y + (h - ih) / 2);
      show(s.icon, true);
      left += w + ui::px(4);
    }
    if (!text.empty() && x + room - left > 0) {
      const int wh = ui::line_height(words_face);
      ui::set_font(words, words_face);
      ui::set_text(words, text);
      lv_obj_set_size(words, x + room - left, wh);
      lv_obj_set_pos(words, left, y + (h - wh) / 2);
      show(words, true);
    }
    return left;
  }

  // A value and its unit after it, on the value's baseline.
  void place_value(Slot &s, const Part &part, Font face, Font unit_face, int x, int y, int room) {
    const int uw = unit_width(part, unit_face);
    const int vw = std::min(ui::text_width(part.value, face), std::max(1, room - uw));
    const int vh = ui::line_height(face);
    ui::set_font(s.value, face);
    ui::set_text(s.value, part.value);
    lv_obj_set_size(s.value, vw, vh);
    lv_obj_set_pos(s.value, x, y);
    show(s.value, true);
    if (!part.unit.empty()) {
      const lv_font_t *big = ui::font(face), *small = ui::font(unit_face);
      const int uh = ui::line_height(unit_face);
      const int baseline = y + vh - (big ? big->base_line : 0);
      ui::set_font(s.unit, unit_face);
      ui::set_text(s.unit, part.unit);
      lv_obj_set_size(s.unit, uw - ui::px(3), uh);
      lv_obj_set_pos(s.unit, x + vw + ui::px(3), baseline - uh + (small ? small->base_line : 0));
      show(s.unit, true);
    }
  }

  void place(const Line &line, Font face, bool labels, int y) {
    if (line.kind == Kind::VALUE) {
      const int column = column_width(line.count);
      const bool words = labels && has_words(line);
      for (int k = 0; k < line.count; ++k) {
        const Part &part = parts_[line.first + k];
        Slot &s = slots_[line.first + k];
        const int x = k * (column + column_gap());
        if (words) {
          place_words(s, part, x, y, row_height(), column, Font::BODY, s.label, part.label);
          place_value(s, part, face, Font::BODY, x, y + row_height(), column);
        } else {
          // No row for the words: the icon stands before the number, centred on it, and says what it is.
          const int room = icon_room(part, face);
          if (room) {
            const Font icon_face = inline_icon(face);
            const int ih = ui::line_height(icon_face);
            ui::set_font(s.icon, icon_face);
            ui::set_text(s.icon, ui::icon(part.icon));
            lv_obj_set_size(s.icon, room - ui::px(4), ih);
            lv_obj_set_pos(s.icon, x, y + (ui::line_height(face) - ih) / 2);
            show(s.icon, true);
          }
          place_value(s, part, face, Font::BODY, x + room, y, column - room);
        }
      }
      return;
    }
    const Part &part = parts_[line.first];
    Slot &s = slots_[line.first];
    if (line.kind == Kind::TEXT) {
      place_words(s, part, 0, y, line_height(line, face, labels), width_, Font::BODY_LARGE, s.value, part.value);
      return;
    }
    // A bar: its name at the left, the value at the right, the bar under them.
    const int h = ui::line_height(Font::BODY_LARGE);
    const int uw = unit_width(part, Font::BODY);
    const int vw = ui::text_width(part.value, Font::BODY_LARGE);
    place_value(s, part, Font::BODY_LARGE, Font::BODY, width_ - vw - uw, y, vw + uw);
    place_words(s, part, 0, y, h, std::max(0, width_ - vw - uw - ui::px(8)), Font::BODY, s.label, part.label);
    const int thick = bar_thickness(), top = y + h + ui::px(4);
    lv_obj_set_size(s.track, width_, thick);
    lv_obj_set_pos(s.track, 0, top);
    lv_obj_set_style_radius(s.track, thick / 2, 0);
    lv_obj_set_style_radius(s.fill, thick / 2, 0);
    show(s.track, true);
    float value;
    float share = 0;
    if (number_of(part.value, &value) && part.max > part.min)
      share = std::min(1.0f, std::max(0.0f, (value - part.min) / (part.max - part.min)));
    const int filled = share > 0 ? std::max(thick, (int) std::lround(share * width_)) : 0;
    lv_obj_set_size(s.fill, filled, thick);
    lv_obj_set_pos(s.fill, 0, 0);
    show(s.fill, filled > 0);
  }

  // A part's colour for words and icons: the key colour itself, except a light one (yellow) in the light look, which
  // takes some ink so it reads on a light card. The bar keeps the key colour.
  static lv_color_t ink_of(uint32_t own) {
    const uint32_t r = own >> 16 & 0xFF, g = own >> 8 & 0xFF, b = own & 0xFF;
    const bool light = r * 299 + g * 587 + b * 114 > 150000;
    return theme::rgb(light && !theme::dark ? theme::mix(own, theme::hex(theme::INK), 150) : own);
  }

  // The colours: a part's own colour paints its value, its icon and its bar, the rest follows the look.
  void paint() {
    ui::set_color(head_, theme::INK);
    ui::set_color(note_, theme::MUTED);
    for (size_t i = 0; i < MAX_PARTS; ++i) {
      Slot &s = slots_[i];
      const uint32_t own = i < parts_.size() ? parts_[i].colour : 0;
      ui::set_color(s.label, theme::MUTED);
      ui::set_color(s.unit, theme::MUTED);
      lv_obj_set_style_bg_color(s.track, ui::color(theme::TRACK), 0);
      if (own) {
        lv_obj_set_style_text_color(s.value, ink_of(own), 0);
        lv_obj_set_style_text_color(s.icon, ink_of(own), 0);
        lv_obj_set_style_bg_color(s.fill, theme::rgb(own), 0);
      } else {
        ui::set_color(s.value, theme::INK);
        ui::set_color(s.icon, theme::MUTED);
        lv_obj_set_style_bg_color(s.fill, ui::color(theme::ACCENT), 0);
      }
    }
  }

  const tessera::Plugin *plugin_;
  int width_ = 0, height_ = 0;
  std::string name_, shown_name_, note_text_;
  std::vector<Part> parts_;
  Slot slots_[MAX_PARTS];
  lv_obj_t *head_{}, *note_{};
};

void CustomCard::setup() {
  add_tile("card", [this]() { return new CardTile(this); });
}

}  // namespace esphome::custom_card
