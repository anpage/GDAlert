#ifndef SHAPETEXTURE_H
#define SHAPETEXTURE_H

#include <Godot.hpp>
#include <AnimatedTexture.hpp>
#include <String.hpp>
#include <Image.hpp>

#include "CNCDll.h"

namespace godot {
    struct shp_header {
        uint16_t num_shapes;
        uint32_t offsets[1];
    };

    struct d2_shape_header {
        uint16_t shape_type;
        uint8_t slices;
        uint16_t width;
        uint8_t height;
        uint16_t filesize;
        uint16_t datasize;
    };

    class CursorTexture : public AnimatedTexture {
        GODOT_CLASS(CursorTexture, AnimatedTexture);

    private:
        static const int CURSOR_MAX_WIDTH = 64;
        static const int CURSOR_MAX_HEIGHT = 64;

        static uint8_t cursor_decompress_buffer[65000];

        static void* extract_shape(const void* buffer, int shape);
        static Image* decode_d2_shape(const void* buffer);

    public:
        static void _register_methods();

        CursorTexture();
        ~CursorTexture();

        void _init();

        bool load_cursor_texture(String filename, int start_frame, int num_frames);
    };

}

#endif
