#include "AudioStreamAud.h"

#include "gamefile.h"
#include "audio.h"
#include "adpcm.h"

using namespace godot;

void AudioStreamAud::_register_methods() {
    register_property<AudioStreamAud, String>("name", &AudioStreamAud::set_filename, &AudioStreamAud::get_filename, "");
    register_property<AudioStreamAud, bool>("looping", &AudioStreamAud::set_loops, &AudioStreamAud::is_looping, false);
    register_property<AudioStreamAud, float>("loop_offset", &AudioStreamAud::set_loop_offset, &AudioStreamAud::get_loop_offset, 0.0f);
}

AudioStreamAud::AudioStreamAud() {
    loops = false;
    loop_offset = 0.0f;
}

AudioStreamAud::~AudioStreamAud() {
}

void AudioStreamAud::_init() {
}

void AudioStreamAud::decode_aud() {
    char* filename_c;
    if (filename.find(".") == -1) {
        filename_c = (filename + ".AUD").alloc_c_string();
    }
    else {
        filename_c = filename.alloc_c_string();
    }

    unsigned char* aud_data = (unsigned char*)GameMixFile::Retrieve(filename_c);
    if (filename_c != nullptr) godot::api->godot_free(filename_c);

    if (aud_data == nullptr) return;

    ADPCMStreamType stream_info;

    // Read in the file's header.
    AUDHeaderStruct raw_header;
    memcpy(&raw_header, aud_data, sizeof(raw_header));
    aud_data += sizeof(raw_header);

    // Compression is ADPCM so we need to init its stream info.
    if (raw_header.m_Compression == COMP_ADPCM) {
        stream_info.m_Channels = (raw_header.m_Flags & 1) + 1;
        stream_info.m_BitsPerSample = raw_header.m_Flags & 2 ? 16 : 8;
        stream_info.m_CompSize = raw_header.m_Size;
        stream_info.m_UnCompSize = raw_header.m_Size * (stream_info.m_BitsPerSample / 4);
        ADPCM_Stream_Init(&stream_info);
    }
    else
    {
        return;
    }

    unsigned char* dechunk_pba_data = new unsigned char[stream_info.m_CompSize];
    unsigned char* pba_data = new unsigned char[stream_info.m_UnCompSize];

    void* aud_data_buf = (void*)aud_data;
    int out_size = Sample_Copy(&stream_info, &aud_data_buf, &stream_info.m_CompSize, pba_data, stream_info.m_UnCompSize, dechunk_pba_data);

    PoolByteArray sound_buffer;
    sound_buffer.resize(out_size);
    PoolByteArray::Write sound_buffer_write = sound_buffer.write();
    unsigned char* sound_buffer_data = sound_buffer_write.ptr();

    memcpy(sound_buffer_data, pba_data, out_size);

    delete[] dechunk_pba_data;
    delete[] pba_data;

    if (out_size)
    {
        if (stream_info.m_BitsPerSample == 16)
        {
            set_format(AudioStreamSample::FORMAT_16_BITS);
        }
        if (stream_info.m_Channels == 2)
        {
            set_stereo(true);
        }
        set_mix_rate(raw_header.m_Rate);
        set_data(sound_buffer);
        set_loop_end(int(out_size / (get_format() + 1)));
    }
}

void AudioStreamAud::set_filename(String name) {
    filename = name;
    decode_aud();
}

String AudioStreamAud::get_filename() {
    return filename;
}

void AudioStreamAud::set_loops(bool loops) {
    this->loops = loops;
    if (loops) {
        set_loop_mode(LOOP_FORWARD);
    }
    else {
        set_loop_mode(LOOP_DISABLED);
    }
}

bool AudioStreamAud::is_looping() {
    return loops;
}

void AudioStreamAud::set_loop_offset(float loop_offset) {
    this->loop_offset = loop_offset;
    set_loop_begin((double)loop_offset * get_mix_rate());
}

float AudioStreamAud::get_loop_offset() {
    return loop_offset;
    _get_property_list();
}
