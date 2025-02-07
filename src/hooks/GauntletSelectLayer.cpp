#include "../classes/GPGauntletsPopup.hpp"
#include <Geode/binding/BoomScrollLayer.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GJMapPack.hpp>
#include <Geode/modify/GauntletSelectLayer.hpp>

using namespace geode::prelude;

class $modify(GPGauntletSelectLayer, GauntletSelectLayer) {
    struct Fields {
        CCMenu* m_timeMenu;
    };

    bool init(int idk) {
        if (!GauntletSelectLayer::init(idk)) return false;

        auto winSize = CCDirector::get()->getWinSize();
        auto f = m_fields.self();
        f->m_timeMenu = CCMenu::create();
        f->m_timeMenu->setPosition({ winSize.width - 30.0f, winSize.height - 30.0f });
        f->m_timeMenu->setID("time-menu"_spr);
        f->m_timeMenu->setVisible(GameLevelManager::get()->m_savedGauntlets->count() > 0);
        addChild(f->m_timeMenu, 2);

        auto timeButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png"),
            this, menu_selector(GPGauntletSelectLayer::onProject)
        );
        timeButton->setID("time-button"_spr);
        f->m_timeMenu->addChild(timeButton);

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
        auto glm = GameLevelManager::get();
        auto projectedIDs = Mod::get()->getSavedValue("projected-ids", std::vector<bool>(NUM_GAUNTLETS, false));
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

        auto f = m_fields.self();
        if (f->m_timeMenu) f->m_timeMenu->setVisible(glm->m_savedGauntlets->count() > 0);
    }

    void onPlay(CCObject* sender) {
        if (GameLevelManager::get()->m_savedGauntlets->objectForKey(std::to_string(sender->getTag()))) GauntletSelectLayer::onPlay(sender);
    }
};
