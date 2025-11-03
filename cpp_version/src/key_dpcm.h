// key_dpcm.h

#pragma once

struct KeyDpcm {
    int inst_idx, octave, note, sample, pitch, loop, loop_point, delta;

    // Ctor
    KeyDpcm() = default;

    // Ctor2
    KeyDpcm(
        int m_inst_idx,
        int m_octave,
        int m_note,
        int m_sample,
        int m_pitch,
        int m_loop,
        int m_loop_point,
        int m_delta
    ) : 
        inst_idx(m_inst_idx),
        octave(m_octave),
        note(m_note),
        sample(m_sample),
        pitch(m_pitch),
        loop(m_loop),
        loop_point(m_loop_point),
        delta(m_delta) { /* */
    }
};
