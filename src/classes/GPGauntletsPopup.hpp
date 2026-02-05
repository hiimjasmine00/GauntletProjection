#include "../GauntletProjection.hpp"
#include <Geode/ui/Popup.hpp>

typedef geode::Function<void()> GauntletCallback;

class GPGauntletsPopup : public geode::Popup {
protected:
    std::array<bool, GauntletProjection::gauntlets> m_enabledGauntlets;
    std::unordered_map<int, cocos2d::CCSprite*> m_gauntletSprites;
    CCMenuItemToggler* m_toggleAllButton;

    bool init(GauntletCallback);
public:
    static GPGauntletsPopup* create(GauntletCallback);
};
