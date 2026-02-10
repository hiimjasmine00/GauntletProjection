#include "GPGauntletsPopup.hpp"
#include <algorithm>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GauntletSprite.hpp>
#include <Geode/binding/GJMapPack.hpp>
#include <jasmine/nodes.hpp>

using namespace geode::prelude;
using namespace jasmine::nodes;

GPGauntletsPopup* GPGauntletsPopup::create(GauntletCallback callback) {
    auto ret = new GPGauntletsPopup();
    if (ret->init(std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GPGauntletsPopup::init(GauntletCallback callback) {
    if (!Popup::init(350.0f, 270.0f, "GJ_square05.png")) return false;

    setID("GPGauntletsPopup");
    setTitle("Project Gauntlets");
    m_title->setID("project-gauntlets-title");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_bgSprite->setID("background");
    m_closeBtn->setID("close-button");

    m_enabledGauntlets = GauntletProjection::projectedIDs;
    m_callback = std::move(callback);

    auto glm = GameLevelManager::get();
    auto rows = (GauntletProjection::gauntlets + 5 - glm->m_savedGauntlets->count()) / 6;
    m_size.width = 350.0f;
    m_size.height = 150.0f + rows * 30.0f;
    m_mainLayer->setContentSize(m_size);
    m_mainLayer->updateLayout();

    auto table = TableNode::create(6, rows, 350.0f, rows * 70.0f, "gauntlet-button-row");
    static_cast<AxisLayout*>(table->getLayout())->setGap(15.0f);
    table->setPosition({ 175.0f, 80.0f + rows * 15.0f });
    table->setID("gauntlet-buttons");
    m_mainLayer->addChild(table);

    for (int i = 0; i < GauntletProjection::gauntlets; i++) {
        auto index = i + 1;
        if (glm->getSavedGauntlet(index)) {
            m_enabledGauntlets[i] = true;
            continue;
        }

        auto gauntletSprite = GauntletSprite::create((GauntletType)index, false);
        gauntletSprite->setScale(0.7f);
        auto innerSprite = gauntletSprite->getChildByType<CCSprite>(0);
        innerSprite->setColor(m_enabledGauntlets[i] ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
        m_gauntletSprites.emplace(i, innerSprite);

        auto gauntletButton = CCMenuItemSpriteExtra::create(gauntletSprite, this, menu_selector(GPGauntletsPopup::onGauntlet));
        gauntletButton->setTag(i);
        gauntletButton->setID(fmt::format("gauntlet-button-{}", index));
        table->addButton(gauntletButton);
    }

    table->updateAllLayouts();

    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    offSprite->setScale(0.8f);
    auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    onSprite->setScale(0.8f);
    m_toggleAllButton = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(GPGauntletsPopup::onToggleAll));
    m_toggleAllButton->setPosition({ 25.0f, 25.0f });
    m_toggleAllButton->toggle(std::ranges::all_of(m_enabledGauntlets, std::identity()));
    m_toggleAllButton->setID("toggle-all-button");
    m_buttonMenu->addChild(m_toggleAllButton);

    auto toggleAllLabel = CCLabelBMFont::create("All", "bigFont.fnt");
    toggleAllLabel->setPosition({ 25.0f, 50.0f });
    toggleAllLabel->setScale(0.5f);
    toggleAllLabel->setID("toggle-all-label");
    m_mainLayer->addChild(toggleAllLabel);

    auto confirmButton = CCMenuItemSpriteExtra::create(ButtonSprite::create("Confirm", 0.8f), this, menu_selector(GPGauntletsPopup::onConfirm));
    confirmButton->setPosition({ 175.0f, 25.0f });
    confirmButton->setID("confirm-button");
    m_buttonMenu->addChild(confirmButton);

    return true;
}

void GPGauntletsPopup::onGauntlet(CCObject* sender) {
    auto tag = sender->getTag();
    m_enabledGauntlets[tag] = !m_enabledGauntlets[tag];
    m_gauntletSprites[tag]->setColor(m_enabledGauntlets[tag] ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
    m_toggleAllButton->toggle(std::ranges::all_of(m_enabledGauntlets, std::identity()));
}

void GPGauntletsPopup::onToggleAll(CCObject* sender) {
    auto toggled = !static_cast<CCMenuItemToggler*>(sender)->m_toggled;
    for (auto& [i, sprite] : m_gauntletSprites) {
        m_enabledGauntlets[i] = toggled;
        sprite->setColor(toggled ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
    }
}

void GPGauntletsPopup::onConfirm(CCObject* sender) {
    GauntletProjection::projectedIDs = m_enabledGauntlets;
    m_callback();
    onClose(nullptr);
}
