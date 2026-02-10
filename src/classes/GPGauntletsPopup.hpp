#include "../GauntletProjection.hpp"
#include <Geode/ui/Popup.hpp>

typedef geode::Function<void()> GauntletCallback;

class GPGauntletsPopup : public geode::Popup {
protected:
    std::array<bool, GauntletProjection::gauntlets> m_enabledGauntlets;
    std::unordered_map<int, cocos2d::CCSprite*> m_gauntletSprites;
    CCMenuItemToggler* m_toggleAllButton;
    GauntletCallback m_callback;

    bool init(GauntletCallback);
    void onGauntlet(cocos2d::CCObject*);
    void onToggleAll(cocos2d::CCObject*);
    void onConfirm(cocos2d::CCObject*);
public:
    static GPGauntletsPopup* create(GauntletCallback);
};
