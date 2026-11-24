#pragma once

#include "core/music_types.hpp"
#include "cast/dlna.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace music {

enum class PlaybackState : std::uint8_t {
    Stopped = 0,
    Playing,
    Paused,
    Error,
};

enum class PlaybackMode : std::uint8_t {
    Sequential = 0,
    Shuffle,
    RepeatOne,
};

struct PlaybackSnapshot {
    PlaybackState state = PlaybackState::Stopped;
    PlaybackMode mode = PlaybackMode::Shuffle;
    std::int64_t track_id = 0;
    std::filesystem::path path;
    std::string title;
    std::string artist;
    std::int64_t duration_ms = 0;
    std::int64_t position_ms = 0;
    std::array<float, 6> spectrum{};
    std::string error;
    // Nombre de la tele si la música está sonando en ella (DLNA); vacío si suena aquí.
    std::string tv_name;
    std::uint64_t revision = 0;

    bool hasTrack() const noexcept { return track_id != 0 && !path.empty(); }
};

}  // namespace music

namespace platform {
class DlnaCaster;
}

namespace music {

class PlaybackModel {
public:
    PlaybackModel();
    ~PlaybackModel();

    PlaybackModel(const PlaybackModel&) = delete;
    PlaybackModel& operator=(const PlaybackModel&) = delete;

    void setQueue(std::vector<Track> tracks);
    bool play(const Track& track);
    bool toggle(const Track& track);
    bool toggleCurrent();
    bool previous();
    bool next();
    void cycleMode();
    void pause();
    void stop();
    void update(float delta_seconds);
    PlaybackSnapshot snapshot() const;

    /** Lo que busca teles y les manda música; se crea la primera vez que se pide. */
    platform::DlnaCaster& caster();
    /**
     * Desde ahora la música suena en `tv`: lo que estuviera sonando sigue allí desde donde
     * iba, y las canciones siguientes también van a la tele.
     */
    void castTo(const dlna::Renderer& tv);
    /** Vuelve a sonar aquí, desde donde iba en la tele. */
    void stopCasting();
    bool casting() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

}  // namespace music
