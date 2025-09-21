//
// Created by Caden Vize on 9/21/2025.
//

#include "Updater.h"

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include <cctype>

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <archive.h>
#include <archive_entry.h>

namespace fs = std::filesystem;
using nlohmann::json;

static size_t curlWriteToString(void* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* s = static_cast<std::string*>(userdata);
    s->append(static_cast<const char*>(ptr), size * nmemb);
    return size * nmemb;
}

static size_t curlWriteToFile(void* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* f = static_cast<std::ofstream*>(userdata);
    f->write(static_cast<const char*>(ptr), static_cast<std::streamsize>(size * nmemb));
    return size * nmemb;
}

Updater::Updater(std::string repo, std::string installDir, std::string versionFile, std::string assetNameRegex, std::optional<std::string> githubToken)
    : repo_(std::move(repo)),
installDir_(std::move(installDir)),
versionFile_(std::move(versionFile)),
assetNameRegex_(std::move(assetNameRegex)),
githubToken_(std::move(githubToken)) {}

bool Updater::run() {
    localVersion_ = readLocalVersion();

    auto latest = fetchLatestRelease();
    remoteVersion_ = trimLeadingV(latest.tag);

    if (!localVersion_.empty() && remoteVersion_ == localVersion_) {
        return false;
    }

    ReleaseAsset asset = chooseAsset(latest);
    pickedAssetName_ = asset.name;

    ensureDir((fs::path(installDir_) / "_updater_staging").string());
    std::string zipPath = (fs::path(installDir_) / "_updater_staging" / asset.name).string();

    downloadToFile(asset.download_url, zipPath);

    std::string extractDir = (fs::path(installDir_) / "_updater_staging" / remoteVersion_).string();
    ensureDir(extractDir);
    extractZip(zipPath, extractDir);

    copyTreeReplace(extractDir, installDir_);

    {
        std::ofstream v(versionFile_, std::ios::binary | std::ios::trunc);
        if (!v) throw std::runtime_error("Failed to write version file: " + versionFile_);
        v << remoteVersion_;
    }

    try {
        removeTree((fs::path(installDir_) / "_updater_staging").string());
    } catch (...) {}

    return true;
}

LatestReleaseInfo Updater::fetchLatestRelease() {
    std::string url = "https://api.github.com/repos/" + repo_ + "/releases/latest";

    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Failed to initialize curl");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "BeneathTheSurface-Updater/1.0");


    std::string body;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/vnd.github+json");
    headers = curl_slist_append(headers, "User-Agent: BeneathTheSurface-Updater/1.0");
    std::string auth;
    if (githubToken_) {
        auth = "Authorization: token " + *githubToken_;
        headers = curl_slist_append(headers, auth.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

    CURLcode rc = curl_easy_perform(curl);
    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (rc != CURLE_OK)
        throw std::runtime_error("Failed to fetch latest release: " + std::to_string(code));
    if (code != 200) {
        std::ostringstream oss;
        oss << "GitHub latest release HTTP " << code << " body: " << body;
        throw std::runtime_error(oss.str());
    }

    json j = json::parse(body);
    LatestReleaseInfo info;
    info.tag = j.value("tag_name", "");

    if (j.contains("assets") && j["assets"].is_array()) {
        for (auto& a : j["assets"]) {
            ReleaseAsset ra;
            ra.name = a.value("name", "");
            ra.download_url = a.value("browser_download_url", "");
            ra.size_bytes = static_cast<size_t>(a.value("size", 0));
            if (!ra.name.empty() && !ra.download_url.empty())
                info.assets.push_back(std::move(ra));
        }
    }
    if (info.tag.empty()) {
        throw std::runtime_error("No tag_name in latest release JSON");
    }
    return info;
}

std::string Updater::readLocalVersion() {
    std::ifstream f(versionFile_, std::ios::binary);
    if (!f) return {};
    std::ostringstream ss; ss << f.rdbuf();
    auto s = ss.str();
    while (!s.empty() && (s.back()=='\n'||s.back()=='\r'||s.back()==' '||s.back()=='\t')) s.pop_back();
    return s;
}

std::string Updater::trimLeadingV(std::string s) {
    if (!s.empty() && s[0] == 'v' || s[0] == 'V') s.erase(s.begin());
    return s;
}

ReleaseAsset Updater::chooseAsset(const LatestReleaseInfo& rel) {
    std::regex re(assetNameRegex_, std::regex::icase);
    for (const auto& a : rel.assets) {
        if (std::regex_match(a.name, re)) return a;
    }
    std::ostringstream oss;
    oss << "No asset matching regex: " << assetNameRegex_ << " in latest release";
    throw std::runtime_error(oss.str());
}

void Updater::downloadToFile(const std::string& url, const std::string& outPath) {
    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Failed to initialize curl");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "BeneathTheSurface-Updater/1.0");

    ensureDir(fs::path(outPath).parent_path().string());
    std::ofstream out(outPath, std::ios::binary | std::ios::trunc);
    if (!out) {
        curl_easy_cleanup(curl);
        throw std::runtime_error("Failed to open file for writing: " + outPath);
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "User-Agent: BeneathTheSurface-Updater/1.0");
    if (githubToken_) {
        std::string auth = "Authorization: token " + *githubToken_;
        headers = curl_slist_append(headers, auth.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteToFile);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

    CURLcode rc = curl_easy_perform(curl);
    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    out.close();

    if (rc != CURLE_OK)
        throw std::runtime_error("Failed to download file: " + std::to_string(code));
    if (code < 200 || code >= 300) {
        std::ostringstream oss;
        oss << "Download HTTP " << code;
        throw std::runtime_error(oss.str());
    }
}

void Updater::extractZip(const std::string& zipPath, const std::string& destDir) {
    ensureDir(destDir);

    struct archive* a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    if (archive_read_open_filename(a, zipPath.c_str(), 10240) != ARCHIVE_OK) {
        throw std::runtime_error("Failed to open zip file: " + zipPath);
    }

    struct archive* ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
        ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);
    archive_write_disk_set_standard_lookup(ext);

    struct archive_entry* entry;
    int r;
    while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK) {
        const char* currentFile = archive_entry_pathname(entry);
        fs::path fullpath = fs::path(destDir) / currentFile;
        archive_entry_set_pathname(entry, fullpath.string().c_str());
        if ((r = archive_write_header(ext, entry)) != ARCHIVE_OK) {
            throw std::runtime_error("Failed to write zip entry: " + std::to_string(r));
        }
        const void* buff;
        size_t size;
        la_int64_t offset;
        while (true) {
            r = archive_read_data_block(a, &buff, &size, &offset);
            if (r == ARCHIVE_EOF) break;
            if (r != ARCHIVE_OK) {
                throw std::runtime_error("Failed to read zip entry: " + std::to_string(r));
            }
            if (archive_write_data_block(ext, buff, size, offset) != ARCHIVE_OK) {
                throw std::runtime_error("Failed to write zip entry: " + std::to_string(r));
            }
        }
        if ((r = archive_write_finish_entry(ext)) != ARCHIVE_OK) {
            throw std::runtime_error("Failed to write zip entry: " + std::to_string(r));
        }
    }

    archive_write_close(ext);
    archive_write_free(ext);
    archive_read_close(a);
    archive_read_free(a);
}

static std::string toLower(std::string s) {
    for (auto& c : s)
        c = (char)std::tolower((unsigned char)c);
    return s;
}

bool Updater::pathEqualsInsensitive(const std::string& a, const std::string& b) {
#ifdef _WIN32
    return toLower(a) == toLower(b);
#else
    return a == b;
#endif
}

void Updater::ensureDir(const std::string& dir) {
    fs::create_directories(dir);
}

void Updater::removeTree(const std::string& path) {
    std::error_code ec;
    fs::remove_all(path, ec);
}

std::vector<std::string> Updater::listFilesRecursive(const std::string& root) {
    std::vector<std::string> out;
    for (auto& p : fs::recursive_directory_iterator(root)) {
        if (fs::is_regular_file(p.path())) out.push_back(p.path().string());
    }
    return out;
}

void Updater::copyFileAtomic(const std::string &from, const std::string &to) {
    fs::path dst = to;
    fs::create_directories(dst.parent_path());
    fs::path tmp = dst; tmp += ".tmpupd";
    fs::copy_file(from, tmp, fs::copy_options::overwrite_existing);
    fs::rename(tmp, dst);
}


void Updater::copyTreeReplace(const std::string& formFir, const std::string& toDir) {
#ifdef _WIN32
    char exePathBuf[MAX_PATH];
    GetModuleFileNameA(nullptr, exePathBuf, MAX_PATH);
    std::string runningExe = exePathBuf;
#else
    std::string runningExe;
#endif
    for (auto& p : fs::recursive_directory_iterator(formFir)) {
        if (fs::is_directory(p.path())) continue;
        auto rel = fs::relative(p.path(), formFir);
        auto dst = fs::path(toDir) / rel;

        if (!runningExe.empty() && pathEqualsInsensitive(dst.string(), runningExe)) {
            continue;
        }

        copyFileAtomic(p.path().string(), dst.string());
    }
}

Updater::CheckResult Updater::checkLatest(std::string* outLocal,
                                          std::string* outRemote) {
    auto latest = fetchLatestRelease();
    std::string remote = trimLeadingV(latest.tag);
    std::string local  = readLocalVersion();

    if (outLocal)  *outLocal  = local;
    if (outRemote) *outRemote = remote;

    if (!local.empty() && local == remote) {
        return CheckResult::UpToDate;
    }
    return CheckResult::UpdateAvailable;
}
