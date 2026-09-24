import wave
import math
import struct
import os

SAMPLE_RATE = 44100

def write_wav(filename, samples):
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)        # Mono
        wav.setsampwidth(2)        # 16-bit
        wav.setframerate(SAMPLE_RATE)
        packed = bytearray()
        for s in samples:
            # Clamp to 16-bit signed range
            val = max(-32767, min(32767, int(s * 32767)))
            packed.extend(struct.pack('<h', val))
        wav.writeframes(packed)

def generate_jump():
    duration = 0.20
    num_samples = int(SAMPLE_RATE * duration)
    samples = []
    for i in range(num_samples):
        t = i / SAMPLE_RATE
        progress = t / duration
        # Frequency rises from 180 Hz to 520 Hz
        freq = 180.0 + 340.0 * (progress ** 0.8)
        # Envelope: quick attack, smooth decay
        env = (1.0 - progress) * min(1.0, progress * 20.0)
        # Sine wave with gentle harmonic
        sample = (0.75 * math.sin(2.0 * math.pi * freq * t) + 0.25 * math.sin(4.0 * math.pi * freq * t)) * env
        samples.append(sample * 0.7)
    return samples

def generate_coin():
    duration = 0.32
    num_samples = int(SAMPLE_RATE * duration)
    samples = []
    for i in range(num_samples):
        t = i / SAMPLE_RATE
        if t < 0.08:
            freq = 987.77 # B5
            env = min(1.0, t * 100.0)
        else:
            freq = 1318.51 # E6
            env = max(0.0, 1.0 - (t - 0.08) / 0.24)
        # Bell-like tone with harmonic
        sample = (0.7 * math.sin(2.0 * math.pi * freq * t) + 0.3 * math.sin(4.0 * math.pi * freq * t)) * env
        samples.append(sample * 0.75)
    return samples

def generate_stomp():
    duration = 0.18
    num_samples = int(SAMPLE_RATE * duration)
    samples = []
    for i in range(num_samples):
        t = i / SAMPLE_RATE
        progress = t / duration
        # Pitch drop from 280 Hz to 50 Hz
        freq = 280.0 * math.exp(- progress * 3.5)
        env = (1.0 - progress) ** 1.5
        # Square-ish wave for crunch
        sin_val = math.sin(2.0 * math.pi * freq * t)
        sample = math.tanh(sin_val * 2.5) * env
        samples.append(sample * 0.8)
    return samples

def generate_damage():
    duration = 0.25
    num_samples = int(SAMPLE_RATE * duration)
    samples = []
    # Seeded pseudo-random for crunch
    rand_state = 12345
    for i in range(num_samples):
        t = i / SAMPLE_RATE
        progress = t / duration
        rand_state = (rand_state * 1103515245 + 12345) & 0x7fffffff
        noise = (rand_state / 0x7fffffff) * 2.0 - 1.0
        
        freq = 130.0 - 50.0 * progress
        buzz = math.sin(2.0 * math.pi * freq * t)
        env = (1.0 - progress) ** 1.2
        sample = (buzz * 0.6 + noise * 0.4) * env
        samples.append(sample * 0.8)
    return samples

def generate_checkpoint():
    duration = 0.55
    num_samples = int(SAMPLE_RATE * duration)
    samples = []
    notes = [523.25, 659.25, 783.99, 1046.50] # C5, E5, G5, C6
    note_dur = duration / len(notes)
    for i in range(num_samples):
        t = i / SAMPLE_RATE
        note_idx = min(len(notes) - 1, int(t / note_dur))
        note_t = t - note_idx * note_dur
        freq = notes[note_idx]
        env = max(0.0, 1.0 - note_t / (note_dur * 1.3)) * min(1.0, note_t * 50.0)
        sample = (0.75 * math.sin(2.0 * math.pi * freq * t) + 0.25 * math.sin(4.0 * math.pi * freq * t)) * env
        samples.append(sample * 0.7)
    return samples

def generate_level_complete():
    duration = 1.4
    num_samples = int(SAMPLE_RATE * duration)
    samples = []
    # Fanfare: C5, E5, G5, C6, E6
    notes = [(0.0, 0.22, 523.25), (0.20, 0.22, 659.25), (0.40, 0.22, 783.99), (0.60, 0.35, 1046.50), (0.90, 0.50, 1318.51)]
    for i in range(num_samples):
        t = i / SAMPLE_RATE
        sample = 0.0
        for start, dur, freq in notes:
            if start <= t < start + dur:
                local_t = t - start
                env = (1.0 - local_t / dur) * min(1.0, local_t * 80.0)
                sample += (0.6 * math.sin(2.0 * math.pi * freq * t) + 0.3 * math.sin(4.0 * math.pi * freq * t) + 0.1 * math.sin(6.0 * math.pi * freq * t)) * env
        samples.append(min(1.0, max(-1.0, sample * 0.75)))
    return samples

def generate_game_over():
    duration = 1.6
    num_samples = int(SAMPLE_RATE * duration)
    samples = []
    # Melancholy descending sequence: E4, D#4, D4, C#4, C4
    notes = [(0.0, 0.30, 329.63), (0.30, 0.30, 311.13), (0.60, 0.30, 293.66), (0.90, 0.35, 277.18), (1.20, 0.40, 261.63)]
    for i in range(num_samples):
        t = i / SAMPLE_RATE
        sample = 0.0
        for start, dur, freq in notes:
            if start <= t < start + dur:
                local_t = t - start
                env = (1.0 - local_t / dur) * min(1.0, local_t * 50.0)
                # Slight vibrato
                vib = 1.0 + 0.015 * math.sin(2.0 * math.pi * 5.0 * t)
                sample += (0.7 * math.sin(2.0 * math.pi * freq * vib * t) + 0.3 * math.sin(4.0 * math.pi * freq * vib * t)) * env
        samples.append(min(1.0, max(-1.0, sample * 0.75)))
    return samples

def generate_victory():
    duration = 2.2
    num_samples = int(SAMPLE_RATE * duration)
    samples = []
    # Triumphant notes
    notes = [(0.0, 0.18, 523.25), (0.18, 0.18, 523.25), (0.36, 0.18, 523.25), 
             (0.54, 0.45, 659.25), (0.95, 0.35, 783.99), (1.30, 0.85, 1046.50)]
    for i in range(num_samples):
        t = i / SAMPLE_RATE
        sample = 0.0
        for start, dur, freq in notes:
            if start <= t < start + dur:
                local_t = t - start
                env = (1.0 - local_t / dur) * min(1.0, local_t * 100.0)
                sample += (0.65 * math.sin(2.0 * math.pi * freq * t) + 0.25 * math.sin(4.0 * math.pi * freq * t) + 0.1 * math.sin(6.0 * math.pi * freq * t)) * env
        samples.append(min(1.0, max(-1.0, sample * 0.75)))
    return samples

def generate_menu_select():
    duration = 0.08
    num_samples = int(SAMPLE_RATE * duration)
    samples = []
    for i in range(num_samples):
        t = i / SAMPLE_RATE
        progress = t / duration
        freq = 600.0 + 300.0 * progress
        env = (1.0 - progress) * min(1.0, progress * 40.0)
        sample = math.sin(2.0 * math.pi * freq * t) * env
        samples.append(sample * 0.5)
    return samples

# --- MUSIC GENERATORS (Seamless loops) ---

def generate_menu_music():
    # 8.0 seconds calm ambient loop (F - G - Em - Am chords)
    duration = 8.0
    num_samples = int(SAMPLE_RATE * duration)
    samples = [0.0] * num_samples
    
    # 4 bars of 2 seconds each
    chords = [
        [349.23, 440.00, 523.25], # F major
        [392.00, 493.88, 587.33], # G major
        [329.63, 392.00, 493.88], # E minor
        [440.00, 523.25, 659.25]  # A minor
    ]
    
    bar_dur = 2.0
    for bar_idx, chord in enumerate(chords):
        bar_start = bar_idx * bar_dur
        # Arpeggio pattern (8 eighth notes per bar, 0.25s each)
        for step in range(8):
            note_start = bar_start + step * 0.25
            freq = chord[step % len(chord)]
            if step >= 4:
                freq *= 1.5 # Octave / fifth inversion
            for i in range(int(0.24 * SAMPLE_RATE)):
                idx = int((note_start * SAMPLE_RATE) + i)
                if idx < num_samples:
                    t = i / SAMPLE_RATE
                    env = max(0.0, 1.0 - t / 0.24) * min(1.0, t * 80.0)
                    samples[idx] += 0.25 * math.sin(2.0 * math.pi * freq * (idx / SAMPLE_RATE)) * env

    # Add gentle bass root
    roots = [174.61, 196.00, 164.81, 220.00] # F2, G2, E2, A2
    for bar_idx, root_freq in enumerate(roots):
        bar_start = bar_idx * bar_dur
        for i in range(int(bar_dur * SAMPLE_RATE)):
            idx = int((bar_start * SAMPLE_RATE) + i)
            if idx < num_samples:
                t = i / SAMPLE_RATE
                env = (1.0 - (t / bar_dur) * 0.3)
                samples[idx] += 0.18 * math.sin(2.0 * math.pi * root_freq * (idx / SAMPLE_RATE)) * env

    # Normalize gently
    max_amp = max(max(abs(s) for s in samples), 0.01)
    return [s / max_amp * 0.65 for s in samples]

def generate_level1_music():
    # 9.6 seconds cheerful meadow adventure melody (BPM 125, 4 beats/sec ~ 2.4s per bar, 4 bars)
    duration = 9.6
    num_samples = int(SAMPLE_RATE * duration)
    samples = [0.0] * num_samples
    
    # Bass rhythm: C3 -> G2 -> A2 -> F2
    bass_notes = [130.81, 98.00, 110.00, 87.31]
    bar_dur = 2.4
    for b_idx, b_freq in enumerate(bass_notes):
        for beat in range(4):
            note_start = b_idx * bar_dur + beat * 0.6
            for i in range(int(0.45 * SAMPLE_RATE)):
                idx = int((note_start * SAMPLE_RATE) + i)
                if idx < num_samples:
                    t = i / SAMPLE_RATE
                    env = (1.0 - t / 0.45) * min(1.0, t * 100.0)
                    samples[idx] += 0.28 * math.sin(2.0 * math.pi * b_freq * (idx / SAMPLE_RATE)) * env

    # Melodic cheerful lead
    # Bar 1 (C): C5, E5, G5, E5, D5, C5
    # Bar 2 (G): B4, D5, G5, D5, C5, B4
    # Bar 3 (Am): C5, E5, A5, G5, E5, D5
    # Bar 4 (F): F5, A5, C6, G5, F5, D5
    melody = [
        # (time, duration, freq)
        (0.0, 0.35, 523.25), (0.4, 0.35, 659.25), (0.8, 0.50, 783.99), (1.4, 0.35, 659.25), (1.8, 0.45, 587.33),
        (2.4, 0.35, 493.88), (2.8, 0.35, 587.33), (3.2, 0.50, 783.99), (3.8, 0.35, 587.33), (4.2, 0.45, 523.25),
        (4.8, 0.35, 523.25), (5.2, 0.35, 659.25), (5.6, 0.50, 880.00), (6.2, 0.35, 783.99), (6.6, 0.45, 659.25),
        (7.2, 0.35, 698.46), (7.6, 0.35, 880.00), (8.0, 0.50, 1046.50), (8.6, 0.35, 783.99), (9.0, 0.50, 587.33)
    ]
    for start, dur, freq in melody:
        for i in range(int(dur * SAMPLE_RATE)):
            idx = int((start * SAMPLE_RATE) + i)
            if idx < num_samples:
                t = i / SAMPLE_RATE
                env = (1.0 - t / dur) * min(1.0, t * 120.0)
                # Square + sine mix for retro 16-bit console feel
                sqr = 1.0 if math.sin(2.0 * math.pi * freq * (idx / SAMPLE_RATE)) > 0 else -1.0
                sin = math.sin(2.0 * math.pi * freq * (idx / SAMPLE_RATE))
                samples[idx] += (0.18 * sin + 0.12 * sqr) * env

    max_amp = max(max(abs(s) for s in samples), 0.01)
    return [s / max_amp * 0.65 for s in samples]

def generate_level2_music():
    # 9.6 seconds darker, rhythmic cavern bassline + mysterious melody
    duration = 9.6
    num_samples = int(SAMPLE_RATE * duration)
    samples = [0.0] * num_samples
    
    # Driving bass: D2 -> Bb1 -> C2 -> A1
    bass_chords = [73.42, 58.27, 65.41, 55.00]
    bar_dur = 2.4
    for b_idx, b_freq in enumerate(bass_chords):
        for pulse in range(8): # 8 pulses per bar
            note_start = b_idx * bar_dur + pulse * 0.3
            for i in range(int(0.25 * SAMPLE_RATE)):
                idx = int((note_start * SAMPLE_RATE) + i)
                if idx < num_samples:
                    t = i / SAMPLE_RATE
                    env = (1.0 - t / 0.25) ** 1.5
                    samples[idx] += 0.35 * math.sin(2.0 * math.pi * b_freq * (idx / SAMPLE_RATE)) * env

    # Minor mystical arpeggio
    # Bar 1: D4, F4, A4, D5 (293.66, 349.23, 440.00, 587.33)
    # Bar 2: Bb3, D4, F4, Bb4 (233.08, 293.66, 349.23, 466.16)
    # Bar 3: C4, E4, G4, C5 (261.63, 329.63, 392.00, 523.25)
    # Bar 4: A3, C#4, E4, A4 (220.00, 277.18, 329.63, 440.00)
    patterns = [
        [293.66, 349.23, 440.00, 587.33],
        [233.08, 293.66, 349.23, 466.16],
        [261.63, 329.63, 392.00, 523.25],
        [220.00, 277.18, 329.63, 440.00]
    ]
    for b_idx, pattern in enumerate(patterns):
        bar_start = b_idx * bar_dur
        for step in range(8):
            note_start = bar_start + step * 0.3
            freq = pattern[step % len(pattern)]
            for i in range(int(0.28 * SAMPLE_RATE)):
                idx = int((note_start * SAMPLE_RATE) + i)
                if idx < num_samples:
                    t = i / SAMPLE_RATE
                    env = (1.0 - t / 0.28) * min(1.0, t * 100.0)
                    samples[idx] += 0.22 * math.sin(2.0 * math.pi * freq * (idx / SAMPLE_RATE)) * env

    max_amp = max(max(abs(s) for s in samples), 0.01)
    return [s / max_amp * 0.65 for s in samples]

def main():
    sfx_dir = "assets/audio/sfx"
    music_dir = "assets/audio/music"
    os.makedirs(sfx_dir, exist_ok=True)
    os.makedirs(music_dir, exist_ok=True)
    
    print("Generating SFX...")
    write_wav(os.path.join(sfx_dir, "jump.wav"), generate_jump())
    write_wav(os.path.join(sfx_dir, "coin.wav"), generate_coin())
    write_wav(os.path.join(sfx_dir, "stomp.wav"), generate_stomp())
    write_wav(os.path.join(sfx_dir, "damage.wav"), generate_damage())
    write_wav(os.path.join(sfx_dir, "checkpoint.wav"), generate_checkpoint())
    write_wav(os.path.join(sfx_dir, "level_complete.wav"), generate_level_complete())
    write_wav(os.path.join(sfx_dir, "game_over.wav"), generate_game_over())
    write_wav(os.path.join(sfx_dir, "victory.wav"), generate_victory())
    write_wav(os.path.join(sfx_dir, "menu_select.wav"), generate_menu_select())
    print("SFX generated successfully!")

    print("Generating Music...")
    write_wav(os.path.join(music_dir, "menu.wav"), generate_menu_music())
    write_wav(os.path.join(music_dir, "level1.wav"), generate_level1_music())
    write_wav(os.path.join(music_dir, "level2.wav"), generate_level2_music())
    print("Music generated successfully!")

if __name__ == "__main__":
    main()
