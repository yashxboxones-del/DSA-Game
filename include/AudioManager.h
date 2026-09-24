#pragma once

// =============================================================
// AudioManager.h
// Header file for the game audio subsystem.
//
// Wraps SFML 3 Audio (SoundBuffer, Sound, Music).
// Follows strict college-level clean design and safety:
//   - Graceful fallback: If sound files are missing or unreadable,
//     the game still runs perfectly with zero crashes.
//   - Decoupled playback: Events trigger sounds through this manager.
//   - Loopable music streams for Menu, Level 1, and Level 2.
// =============================================================

#include <SFML/Audio.hpp>
#include <string>
#include <optional>
#include <iostream>

enum class SoundType
{
    JUMP,
    COIN,
    STOMP,
    DAMAGE,
    CHECKPOINT,
    LEVEL_COMPLETE,
    GAME_OVER,
    VICTORY,
    MENU_SELECT
};

enum class MusicTrack
{
    NONE,
    MENU,
    LEVEL_1,
    LEVEL_2
};

class AudioManager
{
public:
    AudioManager();
    ~AudioManager() = default;

    // Initialize and load all sound buffers and verify music paths
    void init();

    // Sound effect playback
    void playSound(SoundType type);

    // Music streaming playback
    void playMusic(MusicTrack track);
    void pauseMusic();
    void resumeMusic();
    void stopMusic();

    // Volume controls (0.0f to 100.0f)
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);
    float getMusicVolume() const;
    float getSfxVolume() const;

private:
    // Helper to safely load an individual sound buffer
    bool loadSound(sf::SoundBuffer& buffer, std::optional<sf::Sound>& sound, const std::string& filepath);

private:
    // Volumes
    float m_musicVolume;
    float m_sfxVolume;

    // Music stream (SFML 3 streams music directly from disk)
    sf::Music m_music;
    MusicTrack m_currentTrack;
    bool m_isMusicPaused;

    // Sound buffers (in-memory audio data)
    sf::SoundBuffer m_jumpBuffer;
    sf::SoundBuffer m_coinBuffer;
    sf::SoundBuffer m_stompBuffer;
    sf::SoundBuffer m_damageBuffer;
    sf::SoundBuffer m_checkpointBuffer;
    sf::SoundBuffer m_levelCompleteBuffer;
    sf::SoundBuffer m_gameOverBuffer;
    sf::SoundBuffer m_victoryBuffer;
    sf::SoundBuffer m_menuSelectBuffer;

    // Sounds (instances that play the buffers)
    // Note: SFML 3 sf::Sound requires a buffer reference at construction,
    // so std::optional is used until buffers are loaded.
    std::optional<sf::Sound> m_jumpSound;
    std::optional<sf::Sound> m_coinSound;
    std::optional<sf::Sound> m_stompSound;
    std::optional<sf::Sound> m_damageSound;
    std::optional<sf::Sound> m_checkpointSound;
    std::optional<sf::Sound> m_levelCompleteSound;
    std::optional<sf::Sound> m_gameOverSound;
    std::optional<sf::Sound> m_victorySound;
    std::optional<sf::Sound> m_menuSelectSound;
};
