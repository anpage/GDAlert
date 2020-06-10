/**
 * @file
 *
 * @author CCHyper
 * @author OmniBlade
 * @author tomsons26
 *
 * @brief Game audio engine.
 *
 * @copyright Chronoshift is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "audio.h"
#include <algorithm>
#include <cmath>

enum
{
    AUD_CHUNK_MAGIC_ID = 0x0000DEAF,
    VOLUME_MIN = 0,
    VOLUME_MAX = 255,
    PRIORITY_MIN = 0,
    PRIORITY_MAX = 255,
    MAX_SAMPLE_TRACKERS = 30,
    STREAM_BUFFER_COUNT = 16,
    BUFFER_CHUNK_SIZE = 8192, // 256 * 32,
    UNCOMP_BUFFER_SIZE = 2098,
    BUFFER_TOTAL_BYTES = BUFFER_CHUNK_SIZE * 4, // 32 kb
    TIMER_DELAY = 25,
    TIMER_RESOLUTION = 1,
    TIMER_TARGET_RESOLUTION = 10 // 10-millisecond target resolution
};

struct SampleTrackerType
{
    BOOL m_Active; // Is this sample currently active?
    BOOL m_Loading; // Is this to be preloaded?
    BOOL m_DontTouch; // You heard the man. Seems to be used to skip callbacks.
    BOOL m_IsScore; // Is this sample score(music)?
    void* m_Original; // Original sample data pointer.
    int m_OriginalSize; // Original sample size, including header.
    int m_PlaybackRate; // Samples per second.
    int m_BitSize; // Bit size represented as 0 on 8 bits, 2 on 16 bits.
    BOOL m_Stereo; // Is this sample stereo?
    int m_DataLength; // Unused.
    unsigned m_DestPtr; // Used as both a pointer and cursor location.
    BOOL m_MoreSource; // Should load more data?
    BOOL m_OneShot; // Not sure, should play only once maybe?
    void* m_Source; // Sample data pointer, not including header.
    int m_Remainder; // Sample size not including header.
    int m_Priority; // Priority of this sample tracker.
    int16_t m_Handle; // Sample ID.
    unsigned m_Volume; // Volume of this sample tracker.
    unsigned m_Reducer; // By how much to reduce the volume.
    SoundCompressType m_Compression; // Sample compression type.
    int16_t m_TrailerLen; // Unused.
    char m_Trailer[32]; // Unused.
    int m_Pitch; // Unused.
    int16_t m_Flags; // Unused.
    int16_t m_Service; // Requires servicing, checked in callbacks.
    int m_Restart; // Unused.
    AudioStreamCallback m_Callback; // Function to call in callback loop when handling this tracker.
    void* m_QueueBuffer; // Queued file buffer.
    int m_QueueSize; // Queued file size.
    int16_t m_Odd;
    int m_FilePending; // Number of pending files.
    int m_FilePendingSize; // Total pending size of all files.
    int m_FileHandle; // File handle of the current sample.
    void* m_FileBuffer; // Current file buffer.
    ADPCMStreamType m_StreamInfo; // ADPCM stream info, only used on COMP_ADPCM.
};

struct LockedDataType
{
    int m_DigiHandle;
    int m_ServiceSomething;
    unsigned m_MagicNumber;
    void* m_UncompBuffer;
    int m_StreamBufferSize;
    int16_t m_StreamBufferCount;
    SampleTrackerType m_SampleTracker[MAX_SAMPLE_TRACKERS];
    unsigned m_SoundVolume;
    unsigned m_ScoreVolume;
    int m_LockCount;
};

LockedDataType g_LockedData;

/**
 * Simple copies one buffer to another.
 */
int Simple_Copy(void** source, int* ssize, void** alternate, int* altsize, void** dest, int size)
{
    int out = 0;

    if (*ssize == 0) {
        *source = *alternate;
        *ssize = *altsize;
        *alternate = nullptr;
        *altsize = 0;
    }

    if (*source == nullptr || *ssize == 0) {
        return out;
    }

    int s = size;

    if (*ssize < size) {
        s = *ssize;
    }

    memcpy(*dest, *source, s);
    *source = static_cast<char*>(*source) + s;
    *ssize -= s;
    *dest = static_cast<char*>(*dest) + s;
    out = s;

    if ((size - s) == 0) {
        return out;
    }

    *source = *alternate;
    *ssize = *altsize;
    *alternate = nullptr;
    *altsize = 0;

    out = Simple_Copy(source, ssize, alternate, altsize, dest, (size - s)) + s;

    return out;
}

/**
 * Copies one buffer to another, decompressing if needed.
 */
int Sample_Copy(SampleTrackerType* st, void** source, int* ssize, void** alternate, int* altsize, void* dest, int size,
    SoundCompressType scomp, void* trailer, int16_t* trailersize)
{
    int datasize = 0;

    // There is no compression or it doesn't match any of the supported compressions so we just copy the data over.
    if (scomp == COMP_NONE || (scomp != COMP_ZAP && scomp != COMP_ADPCM)) {
        return Simple_Copy(source, ssize, alternate, altsize, &dest, size);
    }

    ADPCMStreamType* s = &st->m_StreamInfo;

    while (size > 0) {
        uint16_t fsize;
        uint16_t dsize;
        unsigned magic;

        void* fptr = &fsize;
        void* dptr = &dsize;
        void* mptr = &magic;

        // Verify and seek over the chunk header.
        if (Simple_Copy(source, ssize, alternate, altsize, &fptr, sizeof(fsize)) < sizeof(fsize)) {
            break;
        }

        if (Simple_Copy(source, ssize, alternate, altsize, &dptr, sizeof(dsize)) < sizeof(dsize) || dsize > size) {
            break;
        }

        if (Simple_Copy(source, ssize, alternate, altsize, &mptr, sizeof(magic)) < sizeof(magic)
            || magic != g_LockedData.m_MagicNumber) {
            break;
        }

        if (fsize == dsize) {
            // File size matches size to decompress, so there's nothing to do other than copy the buffer over.
            if (Simple_Copy(source, ssize, alternate, altsize, &dest, fsize) < dsize) {
                return datasize;
            }
        }
        else {
            // Else we need to decompress it.
            void* uptr = g_LockedData.m_UncompBuffer;

            if (Simple_Copy(source, ssize, alternate, altsize, &uptr, fsize) < fsize) {
                return datasize;
            }

            if (scomp == COMP_ZAP) {
                Audio_Unzap(g_LockedData.m_UncompBuffer, dest, dsize);
            }
            else {
                s->m_Source = g_LockedData.m_UncompBuffer;
                s->m_Dest = dest;

                ADPCM_Decompress(s, dsize);
            }

            dest = reinterpret_cast<char*>(dest) + dsize;
        }

        datasize += dsize;
        size -= dsize;
    }

    return datasize;
}
