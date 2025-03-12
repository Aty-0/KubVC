#pragma once
#include "Libs/imgui/imgui.h"
#include "singleton.h"
#include <string>
// TODO:
#include <vector>

namespace kubvc::render
{
    struct GUIElement 
    {
        // What's we are would use 
        virtual void body() { }
        // What's we are render in body 
        virtual void inBody() { } 

        void setName(std::string name);
        inline std::string getName() const { return m_name; }

        protected:
            std::string m_name = "Element###ID";
    };

    struct GUIPlot : GUIElement
    {
        virtual void body() override;

        virtual void inBody() override { } // TODO:
    };

    class GUI : public utility::Singleton<GUI> 
    {
        public:
            void init();
            void destroy();

            void begin();
            void end();
            void draw();
        private:
            std::vector<GUIElement*> m_elements;
    };
}