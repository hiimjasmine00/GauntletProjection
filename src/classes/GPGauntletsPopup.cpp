#include "GPGauntletsPopup.hpp"
#include "TableNode.hpp"
#include "../GauntletProjection.hpp"
#include <algorithm>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GauntletSprite.hpp>
#include <Geode/binding/GJMapPack.hpp>

using namespace geode::prelude;

GPGauntletsPopup* GPGauntletsPopup::create(GauntletCallback callback) {
    auto ret = new GPGauntletsPopup();
    if (ret->initAnchored(350.0f, 270.0f, std::move(callback), "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GPGauntletsPopup::setup(GauntletCallback callback) {
    setID("GPGauntletsPopup");
    setTitle("Project Gauntlets");
    m_title->setID("project-gauntlets-title");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_bgSprite->setID("background");
    m_closeBtn->setID("close-button");

    m_enabledGauntlets = GauntletProjection::projectedIDs;

    auto savedGauntlets = GameLevelManager::get()->m_savedGauntlets;
    auto rows = (GauntletProjection::gauntlets + 5 - savedGauntlets->count()) / 6;
    m_size.width = 350.0f;
    m_size.height = 150.0f + rows * 30.0f;
    m_mainLayer->setContentSize(m_size);
    m_mainLayer->updateLayout();

    auto table = TableNode::create(6, rows);
    table->setColumnLayout(ColumnLayout::create()->setGap(7.5f)->setAxisReverse(true));
    table->setRowLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    table->setRowHeight(60.0f);
    table->setRowPrefix("gauntlet-button-row");
    table->setContentSize({ 350.0f, rows * 70.0f });
    table->setPosition({ 175.0f, 80.0f + rows * 15.0f });
    table->setID("gauntlet-buttons");
    m_mainLayer->addChild(table);

    m_gauntletSprites = CCDictionary::create();

    for (int i = 0; i < GauntletProjection::gauntlets; i++) {
        auto index = i + 1;
        if (savedGauntlets->objectForKey(fmt::to_string(index))) {
            m_enabledGauntlets[i] = true;
            continue;
        }

        auto gauntletSprite = GauntletSprite::create((GauntletType)index, false);
        gauntletSprite->setScale(0.7f);
        auto innerSprite = gauntletSprite->getChildByType<CCSprite>(0);
        innerSprite->setColor(m_enabledGauntlets[i] ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
        m_gauntletSprites->setObject(innerSprite, i);

        auto gauntletButton = CCMenuItemExt::createSpriteExtra(gauntletSprite, [this, i, innerSprite](auto) {
            m_enabledGauntlets[i] = !m_enabledGauntlets[i];
            innerSprite->setColor(m_enabledGauntlets[i] ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
            m_toggleAllButton->toggle(std::ranges::all_of(m_enabledGauntlets, [](bool v) {
                return v;
            }));
        });
        gauntletButton->setID(fmt::format("gauntlet-button-{}", index));
        table->addButton(gauntletButton);
    }

    table->updateAllLayouts();

    m_toggleAllButton = CCMenuItemExt::createTogglerWithStandardSprites(0.8f, [this](CCMenuItemToggler* sender) {
        auto toggled = !sender->m_toggled;
        for (auto [i, sprite] : CCDictionaryExt<intptr_t, CCNodeRGBA*>(m_gauntletSprites)) {
            m_enabledGauntlets[i] = toggled;
            sprite->setColor(toggled ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
        }
    });
    m_toggleAllButton->setPosition({ 25.0f, 25.0f });
    m_toggleAllButton->toggle(std::ranges::all_of(m_enabledGauntlets, [](bool v) {
        return v;
    }));
    m_toggleAllButton->setID("toggle-all-button");
    m_buttonMenu->addChild(m_toggleAllButton);

    auto toggleAllLabel = CCLabelBMFont::create("All", "bigFont.fnt");
    toggleAllLabel->setPosition({ 25.0f, 50.0f });
    toggleAllLabel->setScale(0.5f);
    toggleAllLabel->setID("toggle-all-label");
    m_mainLayer->addChild(toggleAllLabel);

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", 0.8f), [this, callback = std::move(callback)](auto) {
        GauntletProjection::projectedIDs = m_enabledGauntlets;
        callback();
        onClose(nullptr);
    });
    confirmButton->setPosition({ 175.0f, 25.0f });
    confirmButton->setID("confirm-button");
    m_buttonMenu->addChild(confirmButton);

    return true;
}
