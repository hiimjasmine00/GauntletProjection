#include "GauntletProjection.hpp"
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

std::array<bool, GauntletProjection::gauntlets> GauntletProjection::projectedIDs;

$on_mod(Loaded) {
    auto& value = Mod::get()->getSavedSettingsData()["projected-ids"];
    if (auto arrRes = value.asArray()) {
        arrRes.unwrap().resize(GauntletProjection::gauntlets, false);
    }
    GauntletProjection::projectedIDs = value.as<std::array<bool, GauntletProjection::gauntlets>>().unwrapOrDefault();
}

$on_mod(DataSaved) {
    Mod::get()->setSavedValue("projected-ids", GauntletProjection::projectedIDs);
}
