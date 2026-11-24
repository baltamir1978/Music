#pragma once

#include "cast/dlna.h"

#include <lvgl.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace platform {
class DlnaCaster;
}

namespace music {

/**
 * Lista de teles para mandar la música (V). Va encima de todo (lv_layer_top), con el mismo
 * aspecto que el aviso de volumen, y se maneja con arriba/abajo, Enter y Esc. Mientras está
 * abierta, el caster sigue buscando teles y la lista se va llenando.
 */
class CastPickerView final {
public:
    CastPickerView() = default;
    ~CastPickerView();

    CastPickerView(const CastPickerView&) = delete;
    CastPickerView& operator=(const CastPickerView&) = delete;

    void open(lv_obj_t* parent, platform::DlnaCaster& caster, const std::string& preselect_udn);
    void close();
    bool isOpen() const noexcept { return _root != nullptr; }
    void move(int delta);
    /** La tele señalada; false si no hay ninguna. */
    bool selected(dlna::Renderer& renderer) const;
    void update();

private:
    static constexpr std::size_t kRows = 4;

    platform::DlnaCaster* _caster = nullptr;
    lv_obj_t* _root = nullptr;
    lv_obj_t* _title = nullptr;
    lv_obj_t* _hint = nullptr;
    std::array<lv_obj_t*, kRows> _rows{};
    std::vector<dlna::Renderer> _renderers;
    std::size_t _selection = 0;
    std::uint32_t _version = 0;
    std::uint32_t _opened_at = 0;
    std::string _preselect;
    bool _empty_hint = false;

    void redraw();
};

}  // namespace music
