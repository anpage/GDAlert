#ifndef AUDIOSTREAMAUD_H
#define AUDIOSTREAMAUD_H

#include <Godot.hpp>
#include <String.hpp>
#include <AudioStreamSample.hpp>

#include "CNCDll.h"

namespace godot {

    class AudioStreamAud : public AudioStreamSample {
        GODOT_CLASS(AudioStreamAud, AudioStreamSample);

    private:
        String filename;
        bool loops;
        float loop_offset;

        void decode_aud();

    public:
        static void _register_methods();

        AudioStreamAud();
        ~AudioStreamAud();

        void _init();

        void set_filename(String name);
        String get_filename();

        void set_loops(bool loops);
        bool is_looping();

        void set_loop_offset(float loop_offset);
        float get_loop_offset();
    };

}

#endif
