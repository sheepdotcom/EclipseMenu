#pragma once

#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <string>
#include <vector>
#include <optional>
#include <functional>

namespace eclipse::config {

    /// @brief Get the container for the configuration file.
    nlohmann::json& getStorage();

    /// @brief Get the container for temporary storage.
    nlohmann::json& getTempStorage();

    /// @brief Used internally to trigger callbacks for a value change
    void executeCallbacks(std::string_view name);

    /// @brief Load the configuration file.
    void load();

    /// @brief Save the configuration file.
    void save();

    /// @brief Save the configuration file as a profile, to be loaded later.
    /// @param profile Name of the profile to save.
    void saveProfile(std::string_view profile);

    /// @brief Load a configuration profile from profile name.
    /// @param profile Name of the profile to load.
    void loadProfile(std::string_view profile);

    /// @brief Delete a configuration profile.
    /// @param profile Name of the profile to delete.
    void deleteProfile(std::string_view profile);

    /// @brief Get a list of all configuration profiles.
    std::vector<std::string> getProfiles();

    /// @brief Check if a key exists in the configuration.
    /// @param key Key to check.
    /// @return True if the key exists in the configuration.
    inline bool has(std::string_view key) {
        return getStorage().contains(key);
    }

    /// @brief Get a value by key from the configuration.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @param defaultValue Default value to return if the key does not exist.
    /// @return Value from the configuration or the default value if the key does not exist.
    template<typename T>
    T get(std::string_view key, const T& defaultValue) {
        if (!has(key))
            return defaultValue;

        return getStorage().at(key).get<T>();
    }

    /// @brief Get a value by key from the configuration.
    /// @note If the key does not exist, it will throw an exception.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @return Value from the configuration.
    template<typename T>
    geode::Result<T> get(std::string_view key) {
        if (!has(key))
            return geode::Err(fmt::format("Key '{}' does not exist", key));

        return geode::Ok(getStorage().at(key).get<T>());
    }

    /// @brief Set a value by key in the configuration.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    void set(std::string_view key, const T& value) {
        getStorage()[key] = value;
        executeCallbacks(key);
    }

    /// @brief Get the type of value by key in the configuration.
    /// @param key Key to get the type of.
    /// @return Type of the value.
    nlohmann::detail::value_t getType(std::string_view key);

    /// @brief Check if the value is of the specified type.
    /// @tparam T Type to check.
    /// @param key Key to check.
    /// @return True if the value is of the specified type.
    template<typename T>
    bool is(std::string_view key) {
        if (!has(key))
            return false;

        auto type = getType(key);
        if constexpr (std::is_same_v<T, std::string>) {
            return type == nlohmann::detail::value_t::string;
        } else if constexpr (std::is_same_v<T, bool>) {
            return type == nlohmann::detail::value_t::boolean;
        } else if constexpr (std::is_same_v<T, int>) {
            return type == nlohmann::detail::value_t::number_integer;
        } else if constexpr (std::is_same_v<T, float>) {
            return type == nlohmann::detail::value_t::number_float;
        }

        return false;
    }

    /// @brief Set a value by key in the configuration if the key does not exist.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    void setIfEmpty(std::string_view key, const T& value) {
        if (!has(key))
            set(key, value);
    }

    /// @brief Registers a delegate which is called when a specific value in config is changed
    /// @param key Key of the value which should have a delegate
    /// @param callback Callback to call when value is changed
    void addDelegate(std::string_view key, std::function<void()> callback);

    /// @brief Check if a key exists in the temporary storage.
    /// @param key Key to check.
    /// @return True if the key exists in the temporary storage.
    inline bool hasTemp(std::string_view key) {
        return getTempStorage().contains(key);
    }

    /// @brief Get a value by key from the temporary storage.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @param defaultValue Default value to return if the key does not exist.
    /// @return Value from the temporary storage or the default value if the key does not exist.
    template<typename T>
    T getTemp(std::string_view key, const T& defaultValue) {
        if (!hasTemp(key))
            return defaultValue;

        return getTempStorage().at(key).get<T>();
    }

    /// @brief Get a value by key from the temporary storage.
    /// @note If the key does not exist, it will throw an exception.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @return Value from the temporary storage.
    template<typename T>
    geode::Result<T> getTemp(std::string_view key) {
        if (!hasTemp(key))
            return geode::Err(fmt::format("Key '{}' does not exist", key));

        return geode::Ok(getTempStorage().at(key).get<T>());
    }

    /// @brief Set a value by key in the temporary storage.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    void setTemp(std::string_view key, const T& value) {
        getTempStorage()[key] = value;
    }
}