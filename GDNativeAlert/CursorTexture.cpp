#include "CursorTexture.h"

#include <Image.hpp>
#include <ImageTexture.hpp>

#include "gamefile.h"
#include "lcw.h"

using namespace godot;

uint8_t CursorTexture::cursor_decompress_buffer[65000];

void CursorTexture::_register_methods() {
    register_method("load_cursor_texture", &CursorTexture::load_cursor_texture);
}

CursorTexture::CursorTexture() {
}

CursorTexture::~CursorTexture() {
}

void CursorTexture::_init() {
}

void* CursorTexture::extract_shape(const void* buffer, int shape) {
    shp_header* block = (shp_header*)buffer;
    uint32_t offset;
    uint8_t* byte_buffer = (uint8_t*)buffer;

    int size = sizeof(uint16_t);

    if (!buffer || shape < 0 || shape >= block->num_shapes)
        return(nullptr);

    offset = block->offsets[shape];

    return(byte_buffer + 2 + offset);
}

Image* CursorTexture::decode_d2_shape(const void* buffer) {
    d2_shape_header* shape = (d2_shape_header*)buffer;
    uint8_t* byte_buffer = (uint8_t*)buffer;

    uint8_t* decmp_buff;
    uint8_t* lcw_buff;

    int width = shape->width;
    int height = shape->height;
    int uncompsz = shape->datasize;

    unsigned char* palette_data = (unsigned char* )GameFileClass::Retrieve("TEMPERAT.PAL");
    unsigned char palette[256][3];
    memcpy(palette, palette_data, 768);

    PoolByteArray image_buffer;
    {
        image_buffer.resize(CURSOR_MAX_WIDTH * CURSOR_MAX_HEIGHT * 4);
        PoolByteArray::Write image_buffer_write = image_buffer.write();
        unsigned char* image_buffer_data = image_buffer_write.ptr();

        uint16_t frame_flags = shape->shape_type;

        if (width <= CURSOR_MAX_WIDTH && height <= CURSOR_MAX_HEIGHT) {
            //Flag bit 2 is flag for no compression on frame, decompress to
            //intermediate buffer if flag is clear
            if (!(frame_flags & 2)) {
                decmp_buff = cursor_decompress_buffer;
                lcw_buff = reinterpret_cast<uint8_t*>(shape);
                frame_flags = shape->shape_type | 2;

                memcpy(decmp_buff, lcw_buff, sizeof(d2_shape_header));
                decmp_buff += sizeof(d2_shape_header);
                lcw_buff += sizeof(d2_shape_header);

                //Copies a small lookup table if it exists, probably not in RA.
                if (frame_flags & 1) {
                    memcpy(decmp_buff, lcw_buff, 16);
                    decmp_buff += 16;
                    lcw_buff += 16;
                }

                LCW_Uncomp(lcw_buff, decmp_buff, uncompsz);
                byte_buffer = cursor_decompress_buffer;
            }

            if (frame_flags & 1) {
                // We don't need to handle this for now
            }
            else {
                uint8_t* data_start = byte_buffer + sizeof(d2_shape_header);
                int image_size = height * width;
                int run_len = 0;

                while (image_size) {
                    uint8_t current_byte = *data_start++;
                    if (current_byte) {
                        for (int component = 0; component < 3; component++) {
                            *image_buffer_data++ = palette[current_byte][component] << 2;
                        }
                        *image_buffer_data++ = 0xFF;
                        --image_size;
                        continue;
                    }

                    if (!image_size) {
                        break;
                    }

                    run_len = *data_start;
                    image_size -= run_len;
                    ++data_start;

                    while (run_len--) {
                        for (int component = 0; component < 3; component++) {
                            *image_buffer_data++ = 0;
                        }
                        *image_buffer_data++ = 0;
                    }
                }
            }
        }
    }

    image_buffer.resize(width * height * 4);

    Image* image = Image::_new();
    image->create_from_data(width, height, false, Image::FORMAT_RGBA8, image_buffer);
    image->resize((int64_t)width * 2, (int64_t)height * 2, Image::INTERPOLATE_NEAREST);

    return image;
}

bool CursorTexture::load_cursor_texture(String filename, int start_frame, int num_frames) {
    char* name;
    name = filename.alloc_c_string();

    uint8_t* shp_data = (uint8_t*)GameFileClass::Retrieve(name);
    if (name != nullptr) godot::api->godot_free(name);

    if (shp_data == nullptr) return false;

    shp_header* block = (shp_header*)shp_data;

    if (block->num_shapes <= start_frame + num_frames) return false;

    for (int i = start_frame; i < start_frame + num_frames; i++)
    {
        void* shape_data = extract_shape(shp_data, i);
        Image* image = decode_d2_shape(shape_data);
        ImageTexture* texture = ImageTexture::_new();
        texture->create_from_image(image, 0);
        uint64_t frame_texture_index = (uint64_t)i - start_frame;
        this->set_frame_texture(frame_texture_index, texture);
    }

    this->set_frames(num_frames);

    return true;
}
