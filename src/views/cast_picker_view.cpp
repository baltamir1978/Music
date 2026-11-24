#include "views/cast_picker_view.hpp"

#include "assets/font_assets.hpp"
#include "cast/dlna_caster.h"

namespace music {
namespace {

// Los del aviso de volumen y el de «Hold ESC to exit».
constexpr std::uint32_t kPanelColor = 0x474747;
constexpr std::uint32_t kTextColor = 0xFFFFFF;
constexpr std::uint32_t kDimColor = 0xB0B0B0;
constexpr std::uint32_t kAccentColor = 0xFED40D;
constexpr int kWidth = 284;
constexpr int kHeight = 140;
// Sin teles al cabo de este rato, la lista explica qué puede pasar.
constexpr std::uint32_t kNoTvHintMs = 8000;

}  // namespace

CastPickerView::~CastPickerView() { close(); }

void CastPickerView::open(lv_obj_t* parent, platform::DlnaCaster& caster, const std::string& preselect_udn)
{
    close();
    _caster = &caster;
    _preselect = preselect_udn;
    _selection = 0;
    _renderers.clear();
    _version = caster.renderers_version() + 1; // que se pinte
    _opened_at = lv_tick_get();
    _empty_hint = false;

    _root = lv_obj_create(parent);
    lv_obj_remove_style_all(_root);
    lv_obj_set_size(_root, kWidth, kHeight);
    lv_obj_center(_root);
    lv_obj_set_style_bg_color(_root, lv_color_hex(kPanelColor), 0);
    lv_obj_set_style_bg_opa(_root, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(_root, 14, 0);
    lv_obj_set_style_pad_all(_root, 10, 0);
    lv_obj_remove_flag(_root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(_root, LV_OBJ_FLAG_CLICKABLE);

    _title = lv_label_create(_root);
    lv_obj_set_style_text_font(_title, font(FontFamily::Sans, FontSize::Px14), 0);
    lv_obj_set_style_text_color(_title, lv_color_hex(kTextColor), 0);
    lv_obj_align(_title, LV_ALIGN_TOP_LEFT, 0, 0);

    for (std::size_t i = 0; i < _rows.size(); ++i) {
        auto* row = lv_label_create(_root);
        lv_obj_set_style_text_font(row, font(FontFamily::Sans, FontSize::Px12), 0);
        lv_label_set_long_mode(row, LV_LABEL_LONG_DOT);
        lv_obj_set_size(row, LV_PCT(100), 20);
        lv_obj_set_style_pad_hor(row, 6, 0);
        lv_obj_set_style_pad_top(row, 2, 0);
        lv_obj_set_style_radius(row, 6, 0);
        lv_obj_align(row, LV_ALIGN_TOP_LEFT, 0, 22 + static_cast<int>(i) * 21);
        _rows[i] = row;
    }

    _hint = lv_label_create(_root);
    lv_obj_set_style_text_font(_hint, font(FontFamily::Sans, FontSize::Px12), 0);
    lv_obj_set_style_text_color(_hint, lv_color_hex(kDimColor), 0);
    lv_label_set_long_mode(_hint, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(_hint, LV_PCT(100));
    lv_obj_align(_hint, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_move_foreground(_root);
    update();
}

void CastPickerView::close()
{
    if (_root && lv_obj_is_valid(_root)) {
        lv_obj_delete(_root);
    }
    _root = nullptr;
    _title = nullptr;
    _hint = nullptr;
    _rows.fill(nullptr);
    _caster = nullptr;
}

void CastPickerView::move(int delta)
{
    if (_renderers.empty()) {
        return;
    }
    const int next = static_cast<int>(_selection) + delta;
    if (next < 0 || next >= static_cast<int>(_renderers.size())) {
        return;
    }
    _selection = static_cast<std::size_t>(next);
    redraw();
}

bool CastPickerView::selected(dlna::Renderer& renderer) const
{
    if (_selection >= _renderers.size()) {
        return false;
    }
    renderer = _renderers[_selection];
    return true;
}

void CastPickerView::update()
{
    if (!_root || !_caster) {
        return;
    }
    const bool empty_hint = _renderers.empty() && lv_tick_elaps(_opened_at) >= kNoTvHintMs;
    if (_caster->renderers_version() == _version && empty_hint == _empty_hint) {
        return;
    }
    const bool first_fill = _renderers.empty();
    _version = _caster->renderers_version();
    _renderers = _caster->renderers();
    _empty_hint = empty_hint;
    if (first_fill) {
        // La última tele usada sale ya señalada.
        for (std::size_t i = 0; i < _renderers.size(); ++i) {
            if (_renderers[i].udn == _preselect) {
                _selection = i;
            }
        }
    }
    redraw();
}

void CastPickerView::redraw()
{
    if (!_root) {
        return;
    }
    lv_label_set_text(_title, _renderers.empty() ? "Looking for TVs..." : "Play on TV");
    const std::size_t first = _selection < kRows ? 0 : _selection + 1 - kRows;
    for (std::size_t i = 0; i < _rows.size(); ++i) {
        auto* row = _rows[i];
        const std::size_t index = first + i;
        if (index >= _renderers.size()) {
            lv_obj_add_flag(row, LV_OBJ_FLAG_HIDDEN);
            continue;
        }
        lv_obj_remove_flag(row, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(row, _renderers[index].name.c_str());
        const bool selected = index == _selection;
        lv_obj_set_style_bg_color(row, lv_color_hex(kAccentColor), 0);
        lv_obj_set_style_bg_opa(row, selected ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
        lv_obj_set_style_text_color(row, lv_color_hex(selected ? 0x000000 : kTextColor), 0);
    }
    if (_empty_hint) {
        lv_label_set_text(_hint, "None yet. Is the TV on and on this Wi-Fi?");
    } else {
        lv_label_set_text(_hint, _renderers.empty() ? "ESC cancel" : "ENTER play   ESC cancel");
    }
}

}  // namespace music
