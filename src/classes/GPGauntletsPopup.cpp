#include "GPGauntletsPopup.hpp"
#include "TableNode.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GauntletSprite.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

GPGauntletsPopup* GPGauntletsPopup::create(GauntletCallback callback) {
    auto ret = new GPGauntletsPopup();
    if (ret->initAnchored(350.0f, 270.0f, callback)) {
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

    m_enabledGauntlets = Mod::get()->getSavedValue<std::vector<bool>>("projected-ids", {});
    m_enabledGauntlets.resize(NUM_GAUNTLETS, false);

    auto savedGauntlets = GameLevelManager::get()->m_savedGauntlets;
    auto rows = (NUM_GAUNTLETS + 5 - savedGauntlets->count()) / 6;
    m_size = CCSize { 350.0f, 150.0f + rows * 30.0f };
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

    for (int i = 0; i < NUM_GAUNTLETS; i++) {
        if (savedGauntlets->objectForKey(std::to_string(i + 1))) continue;

        auto gauntletSprite = GauntletSprite::create((GauntletType)(i + 1), false);
        gauntletSprite->setScale(0.7f);
        auto innerSprite = gauntletSprite->getChildByType<CCSprite>(0);
        innerSprite->setColor(m_enabledGauntlets[i] ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
        auto gauntletButton = CCMenuItemExt::createSpriteExtra(gauntletSprite, [this, i, innerSprite](auto) {
            m_enabledGauntlets[i] = !m_enabledGauntlets[i];
            innerSprite->setColor(m_enabledGauntlets[i] ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
        });
        gauntletButton->setID(fmt::format("gauntlet-button-{}", i + 1));
        table->addButton(gauntletButton);
    }

    table->updateAllLayouts();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        Mod::get()->setSavedValue("projected-ids", m_enabledGauntlets);
        callback();
        onClose(nullptr);
    });
    confirmButton->setPosition({ 175.0f, 25.0f });
    confirmButton->setID("confirm-button");
    m_buttonMenu->addChild(confirmButton);

    return true;
}
