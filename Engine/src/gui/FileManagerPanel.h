#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <functional>
#include <memory>
#include <unordered_map>
#include <core/AssetManager.h>

struct FileNode {
    std::string name;
    std::filesystem::path path;
    bool isDirectory = false;
    std::vector<std::shared_ptr<FileNode>> children;
    unsigned int previewTexture = 0;
};

class FileManagerPanel {
public:
    void init(const std::string& rootPath);
    void initScriptsPath(const std::string& path);
    void show(bool* open = nullptr);

    static constexpr const char* payload = "FM_ASSET_PATH";
    static constexpr const char* scriptPayload = "FM_SCRIPT_PATH";

    std::function<void(const std::string&)> onOpenScene;
    std::function<void(const std::string&)> onDropMesh;
    std::function<void(const std::string&)> onDropTexture;

    const std::filesystem::path& getCurrentDirectory() const { return currentDir; }
    std::vector<std::string> getScriptNames() const;

private:
    void drawFolderTree(FileNode& node);
    void drawContentBrowser();
    void drawBreadcrumb();
    void drawItem(FileNode& node);
    void drawContextMenuBackground();
    void drawContextMenuItem(FileNode& node);

    void refresh();
    FileNode buildTree(const std::filesystem::path& path, int depth = 0) const;
    void collectChildren(FileNode& node) const;

    void createFolder(const std::filesystem::path& parent, const std::string& name);
    void renameItem(FileNode& node, const std::string& newName);
    void deleteItem(FileNode& node);
    void importAsset(const std::string& srcAbsolutePath);

    unsigned int loadThumbnail(const std::filesystem::path& imagePath);
    void releaseThumbnails();

    static const char* iconForNode(const FileNode& node);
    static bool isImageFile(const std::filesystem::path& p);
    static bool isAudioFile(const std::filesystem::path& p);
    static bool isMeshFile(const std::filesystem::path& p);
    static bool isSceneFile(const std::filesystem::path& p);
    static bool isShaderFile(const std::filesystem::path& p);
    static bool isScriptFile(const std::filesystem::path& p);

    std::filesystem::path rootPath;
    std::filesystem::path currentDir;

    FileNode rootNode;
    std::vector<FileNode> currentItems;

    bool renamingActive = false;
    std::filesystem::path renamingPath;
    char renameBuffer[256] = {};

    bool createFolderOpen = false;
    char newFolderName[128] = "New Folder";

    bool importPopupOpen = false;
    std::filesystem::path selectedPath;
    std::unordered_map<std::string, unsigned int> thumbnailCache;

    float itemSize = 80.0f;
    float refreshTimer = 0.0f;
    float refreshInterval = 2.0f;

    std::filesystem::path pendingNavigate;

    enum class Tab { Assets, Scripts };
    Tab activeTab = Tab::Assets;

    std::filesystem::path scriptsPath;
    std::vector<FileNode> scriptItems;

    void refreshScripts();
    void drawScriptsTab();
    void importScript(const std::string& srcAbsolutePath);

    bool importScriptPopupOpen = false;
    static char importScriptPathBuf[512];
};