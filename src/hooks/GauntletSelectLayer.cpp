#include "../classes/GPGauntletsPopup.hpp"
#include <Geode/binding/BoomScrollLayer.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GJMapPack.hpp>
#include <Geode/modify/GauntletSelectLayer.hpp>

using namespace geode::prelude;

class $modify(GPGauntletSelectLayer, GauntletSelectLayer) {
    struct Fields {
        Ref<CCMenuItemSpriteExtra> m_timeButton;
    };

    bool init(int p0) {
        if (!GauntletSelectLayer::init(p0)) return false;

        if (auto topRightMenu = getChildByID("top-right-menu")) {
            auto f = m_fields.self();
            f->m_timeButton = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png"),
                this, menu_selector(GPGauntletSelectLayer::onProject)
            );
            f->m_timeButton->setID("time-button"_spr);
            topRightMenu->addChild(f->m_timeButton);
            topRightMenu->updateLayout();
        }

        return true;
    }

    void onProject(CCObject* sender) {
        GPGauntletsPopup::create([this, sender = static_cast<CCNode*>(sender)] {
            if (m_scrollLayer) {
                m_scrollLayer->removeFromParent();
                m_scrollLayer = nullptr;
            }
            m_leftButton->setVisible(false);
            m_rightButton->setVisible(false);
            sender->setVisible(false);
            GauntletSelectLayer::setupGauntlets();
            m_leftButton->setVisible(true);
            m_rightButton->setVisible(true);
            sender->setVisible(true);
        })->show();
    }

    void setupGauntlets() {
        auto f = m_fields.self();
        if (f->m_timeButton) f->m_timeButton->removeFromParentAndCleanup(false);

        auto glm = GameLevelManager::get();
        auto savedGauntlets = glm->m_savedGauntlets;
        auto newSavedGauntlets = CCDictionary::create();

        for (int i = 0; i < GauntletProjection::gauntlets; i++) {
            auto id = fmt::to_string(i + 1);
            auto gauntlet = static_cast<GJMapPack*>(savedGauntlets->objectForKey(id));
            if (!gauntlet && GauntletProjection::projectedIDs[i]) {
                gauntlet = GJMapPack::create();
                gauntlet->m_packID = i + 1;
                gauntlet->m_levels = CCArray::create();
                gauntlet->m_levels->retain();
                gauntlet->m_isGauntlet = true;
            }
            if (gauntlet) newSavedGauntlets->setObject(gauntlet, id);
        }

        glm->m_savedGauntlets = newSavedGauntlets;

        GauntletSelectLayer::setupGauntlets();

        glm->m_savedGauntlets = savedGauntlets;

        if (auto topRightMenu = getChildByID("top-right-menu")) {
            if (newSavedGauntlets->count() > 0 && f->m_timeButton) topRightMenu->addChild(f->m_timeButton);
            topRightMenu->updateLayout();
        }
    }

    void onPlay(CCObject* sender) {
        if (GameLevelManager::get()->m_savedGauntlets->objectForKey(fmt::to_string(sender->getTag()))) {
            GauntletSelectLayer::onPlay(sender);
        }
    }
};
