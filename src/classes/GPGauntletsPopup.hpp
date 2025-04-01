#include <Geode/ui/Popup.hpp>

#define NUM_GAUNTLETS 54

typedef std::function<void()> GauntletCallback;

class GPGauntletsPopup : public geode::Popup<GauntletCallback> {
protected:
    std::vector<bool> m_enabledGauntlets;

    bool setup(GauntletCallback) override;
public:
    static GPGauntletsPopup* create(GauntletCallback);
};
