#include "GauntletProjection.hpp"
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

std::vector<bool> GauntletProjection::projectedIDs;

$on_mod(Loaded) {
    auto mod = Mod::get();
    auto& saveContainer = mod->getSaveContainer();
    auto ids = saveContainer.get("projected-ids").andThen([](matjson::Value& v) {
        return v.asArray();
    });
    if (ids.isOkAnd([](const std::vector<matjson::Value>& arr) {
        for (auto& v : arr) {
            if (!v.isBool()) return false;
        }
        return true;
    })) {
        ids.unwrap().resize(GauntletProjection::gauntlets, false);
    }
    else {
        saveContainer.set("projected-ids", std::vector<bool>(GauntletProjection::gauntlets, false));
    }

    GauntletProjection::projectedIDs = mod->getSavedValue<std::vector<bool>>("projected-ids");
}

$on_mod(DataSaved) {
    Mod::get()->setSavedValue("projected-ids", GauntletProjection::projectedIDs);
}
