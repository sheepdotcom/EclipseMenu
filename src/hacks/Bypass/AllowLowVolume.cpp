#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Bypass {

    class AllowLowVolume : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");

            tab->addToggle("bypass.allowlowvolume")
                ->handleKeybinds()
                ->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Allow Low Volume"; }
    };

    REGISTER_HACK(AllowLowVolume)

#define GET_SLIDER(sender) geode::cast::typeinfo_cast<SliderThumb*>(sender); if (!slider) return

    class $modify(AllowLowVolumeOLHook, OptionsLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("bypass.allowlowvolume")

        void musicSliderChanged(cocos2d::CCObject* sender) {
            auto slider = GET_SLIDER(sender);
            auto value = slider->getValue();
            auto* audioEngine = FMODAudioEngine::get();
            float originalVolume = audioEngine->getBackgroundMusicVolume();
            audioEngine->setBackgroundMusicVolume(value);
            if (originalVolume <= 0.f && value > 0.f)
                GameManager::get()->playMenuMusic();
        }

        void sfxSliderChanged(cocos2d::CCObject* sender) {
            auto slider = GET_SLIDER(sender);
            auto value = slider->getValue();
            FMODAudioEngine::get()->setEffectsVolume(value);
        }
    };

    class $modify(AllowLowVolumePLHook, PauseLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("bypass.allowlowvolume")

        void musicSliderChanged(cocos2d::CCObject* sender) {
            auto slider = GET_SLIDER(sender);
            auto value = slider->getValue();
            FMODAudioEngine::get()->setBackgroundMusicVolume(value);
        }

// Function is merged with the one in OptionsLayer on Windows
#if !(defined(GEODE_IS_WINDOWS) && GEODE_COMP_GD_VERSION == 22060)
        void sfxSliderChanged(cocos2d::CCObject* sender) {
            auto slider = GET_SLIDER(sender);
            auto value = slider->getValue();
            FMODAudioEngine::get()->setEffectsVolume(value);
        }
#endif
    };
}
