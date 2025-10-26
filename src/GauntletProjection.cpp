#include "GauntletProjection.hpp"
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

std::array<bool, GauntletProjection::gauntlets> GauntletProjection::projectedIDs;

template <class T, size_t N>
struct matjson::Serialize<std::array<T, N>> {
    static Result<std::array<T, N>> fromJson(const matjson::Value& value) {
        if (!value.isArray()) return Err("not an array");
        std::array<T, N> arr{};
        for (size_t i = 0; i < N && i < value.size(); i++) {
            GEODE_UNWRAP_INTO(arr[i], value[i].as<T>());
        }
        return Ok(arr);
    }

    static matjson::Value toJson(const std::array<T, N>& arr) {
        std::vector<matjson::Value> vec;
        vec.reserve(N);
        for (auto& v : arr) {
            vec.push_back(v);
        }
        return vec;
    }
};

$on_mod(Loaded) {
    GauntletProjection::projectedIDs = Mod::get()->getSavedValue<std::array<bool, GauntletProjection::gauntlets>>("projected-ids");
}

$on_mod(DataSaved) {
    Mod::get()->setSavedValue("projected-ids", GauntletProjection::projectedIDs);
}
