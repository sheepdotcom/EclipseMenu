#include "Geode/binding/GJGameLevel.hpp"
#include "Geode/binding/LevelInfoLayer.hpp"
#include "Geode/cocos/cocoa/CCObject.h"
#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/LevelInfoLayer.hpp>

namespace eclipse::hacks::Bypass {

    class CopyBypass : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Copy Bypass", "bypass.copybypass")
                ->handleKeybinds()
                ->setDescription("Allows you to copy any level.");
        }

        [[nodiscard]] const char* getId() const override { return "Level Copy Bypass"; }
    };

    REGISTER_HACK(CopyBypass)

    class $modify(CopyBypassLILHook, LevelInfoLayer) {
        struct Fields {
            int password;
        };

        bool init(GJGameLevel* level, bool challenge) {
            m_fields->password = level->m_password;

            if (config::get<bool>("bypass.copybypass", false))
                level->m_password = 1;

            return LevelInfoLayer::init(level, challenge);
        }

        void updateLabelValues() {
            m_fields->password = this->m_level->m_password;
            
            if (config::get<bool>("bypass.copybypass", false))
                this->m_level->m_password = 1;

            LevelInfoLayer::updateLabelValues();
        }

        void onBack(cocos2d::CCObject* sender) {
            this->m_level->m_password = m_fields->password;

            LevelInfoLayer::onBack(sender);
        }

        void confirmClone(cocos2d::CCObject* sender) {
            this->m_level->m_password = m_fields->password;

            LevelInfoLayer::confirmClone(sender);
        }
    };

}
