#include <Geode/ui/Popup.hpp>

typedef std::function<void()> GauntletCallback;

class GPGauntletsPopup : public geode::Popup<GauntletCallback> {
protected:
    std::vector<bool> m_enabledGauntlets;
    geode::Ref<cocos2d::CCDictionary> m_gauntletSprites;
    CCMenuItemToggler* m_toggleAllButton;

    bool setup(GauntletCallback) override;
public:
    static GPGauntletsPopup* create(GauntletCallback);
};
