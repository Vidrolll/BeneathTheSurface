//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_UPDATER_H
#define BENEATHTHESURFACE_UPDATER_H

#pragma once
#include <string>
#include <optional>
#include <vector>

struct ReleaseAsset {
    std::string name;
    std::string download_url;
    size_t size_bytes{};
};

struct LatestReleaseInfo {
    std::string tag;
    std::vector<ReleaseAsset> assets;
};

class Updater {
public:
    Updater(std::string repo,
        std::string installDir,
        std::string versionFile,
        std::string assetNameRegex,
        std::optional<std::string> githubToken = std::nullopt);

    bool run();

    std::string getLocalVersion() const { return localVersion_; }
    std::string getRemoteVersion() const { return remoteVersion_; }
    std::string getPickedAssetName() const { return pickedAssetName_; }

    enum class CheckResult { UpToDate, UpdateAvailable };

    CheckResult checkLatest(std::string* outLocal = nullptr,
                            std::string* outRemote = nullptr);
private:
    std::string repo_;
    std::string installDir_;
    std::string versionFile_;
    std::string assetNameRegex_;
    std::optional<std::string> githubToken_;

    std::string localVersion_;
    std::string remoteVersion_;
    std::string pickedAssetName_;

    LatestReleaseInfo fetchLatestRelease();
    std::string readLocalVersion();
    static std::string trimLeadingV(std::string s);

    ReleaseAsset chooseAsset(const LatestReleaseInfo& rel);

    void downloadToFile(const std::string& url, const std::string& outPath);

    void extractZip(const std::string& zipPath, const std::string& destDir);

    void copyTreeReplace(const std::string& formFir, const std::string& toDir);

    static bool pathEqualsInsensitive(const std::string& a, const std::string& b);
    static void ensureDir(const std::string& dir);
    static void removeTree(const std::string& path);
    static void copyFileAtomic(const std::string& from, const std::string& to);
    static std::vector<std::string> listFilesRecursive(const std::string& root);
};

#endif //BENEATHTHESURFACE_UPDATER_H