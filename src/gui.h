#pragma once
#include "singleton.h"
#include "Libs/imgui/imgui.h"
#include "Libs/imgui/implot.h"

namespace kubvc::render
{
    class GUI : public utility::Singleton<GUI> 
    {
        public:
            void init();
            void destroy();

            void begin();
            void end();
    };
}