#ifndef REDALERTCONTENT_H
#define REDALERTCONTENT_H

#include <Godot.hpp>
#include <PoolArrays.hpp>
#include <Node.hpp>
#include <String.hpp>
#include <AudioStreamSample.hpp>

#include "CNCDll.h"

namespace godot {

    class RedAlertContent : public Node {
        GODOT_CLASS(RedAlertContent, Node);

        static const long FAST_KEY_EXP;
        static const char FAST_KEY_MOD[42];

    public:
        static void _register_methods();

        RedAlertContent();
        ~RedAlertContent();

        void _init();
    };

}

#endif
