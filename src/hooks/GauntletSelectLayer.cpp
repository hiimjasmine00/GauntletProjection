#include "../classes/GPGauntletsPopup.hpp"
#include <Geode/binding/BoomScrollLayer.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GJMapPack.hpp>
#include <Geode/modify/GauntletSelectLayer.hpp>

using namespace geode::prelude;

class $modify(GPGauntletSelectLayer, GauntletSelectLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* m_timeButton;
        ~Fields() {
            CC_SAFE_RELEASE(m_timeButton);
        }
    };

    bool init(int p0) {
        if (!GauntletSelectLayer::init(p0)) return false;

        auto f = m_fields.self();
        f->m_timeButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png"),
            this, menu_selector(GPGauntletSelectLayer::onProject)
        );
        f->m_timeButton->setID("time-button"_spr);
        f->m_timeButton->retain();
        auto topRightMenu = getChildByID("top-right-menu");
        topRightMenu->addChild(f->m_timeButton);
        topRightMenu->updateLayout();

        return true;
    }

    void onProject(CCObject* sender) {
        GPGauntletsPopup::create([this, sender] {
            if (m_scrollLayer) {
                m_scrollLayer->removeFromParent();
                m_scrollLayer = nullptr;
            }
            m_leftButton->setVisible(false);
            m_rightButton->setVisible(false);
            static_cast<CCNode*>(sender)->setVisible(false);
            setupGauntlets();
            m_leftButton->setVisible(true);
            m_rightButton->setVisible(true);
            static_cast<CCNode*>(sender)->setVisible(true);
        })->show();
    }

    void setupGauntlets() {
        auto f = m_fields.self();
        if (f->m_timeButton) f->m_timeButton->removeFromParentAndCleanup(false);

        auto glm = GameLevelManager::get();
        auto projectedIDs = Mod::get()->getSavedValue<std::vector<bool>>("projected-ids", {});
        projectedIDs.resize(NUM_GAUNTLETS, false);
        auto newSavedGauntlets = CCDictionary::create();
        newSavedGauntlets->retain();
        std::vector<bool> gauntletsAdded;
        for (int i = 0; i < NUM_GAUNTLETS; i++) {
            auto id = std::to_string(i + 1);
            if (!glm->m_savedGauntlets->objectForKey(id) && projectedIDs[i]) {
                gauntletsAdded.push_back(true);
                auto gauntlet = GJMapPack::create();
                gauntlet->m_packID = i + 1;
                gauntlet->m_levels = CCArray::create();
                gauntlet->m_levels->retain();
                glm->m_savedGauntlets->setObject(gauntlet, id);
            }
            else gauntletsAdded.push_back(false);
            if (glm->m_savedGauntlets->objectForKey(id)) newSavedGauntlets->setObject(glm->m_savedGauntlets->objectForKey(id), id);
        }

        glm->m_savedGauntlets->release();
        glm->m_savedGauntlets = newSavedGauntlets;

        GauntletSelectLayer::setupGauntlets();

        for (int i = 0; i < NUM_GAUNTLETS; i++) {
            if (gauntletsAdded[i]) glm->m_savedGauntlets->removeObjectForKey(std::to_string(i + 1));
        }
        
        if (glm->m_savedGauntlets->count() > 0) {
            if (auto topRightMenu = getChildByID("top-right-menu"); topRightMenu && f->m_timeButton) {
                topRightMenu->addChild(f->m_timeButton);
                topRightMenu->updateLayout();
            }
        }
    }

    void onPlay(CCObject* sender) {
        if (GameLevelManager::get()->m_savedGauntlets->objectForKey(std::to_string(sender->getTag()))) GauntletSelectLayer::onPlay(sender);
    }
};
