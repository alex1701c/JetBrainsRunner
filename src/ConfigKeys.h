#ifndef JETBRAINSRUNNER_CONFIGKEYS_H
#define JETBRAINSRUNNER_CONFIGKEYS_H

struct Config {
    constexpr static const auto launchByAppName = "LaunchByAppName";
    constexpr static const auto launchByProjectName = "LaunchByProjectName";
    constexpr static const auto notifyUpdates = "NotifyUpdates";
    constexpr static const auto formatString = "FormatString";
    constexpr static const auto formatStringDefault = "%APPNAME launch %PROJECT";
    constexpr static const auto displayInCategories = "DisplayInCategories";
    constexpr static const auto customMappingGroup = "CustomMapping";
    constexpr static const auto checkedUpdateDate = "checkedUpdateDate";
};
struct FormatString {
    constexpr static const auto PROJECT = "%PROJECT";
    constexpr static const auto APPNAME = "%APPNAME";
    constexpr static const auto APP = "%APP";
};
#endif //JETBRAINSRUNNER_CONFIGKEYS_H
