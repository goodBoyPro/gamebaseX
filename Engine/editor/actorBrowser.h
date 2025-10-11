#include "gui/imguiDx/imguiLib/imgui.h"
#include "gui/imguiDx/imguiLib/imgui_internal.h"
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <map>

// 资产类型枚举
enum class AssetType {
    Folder,
    Texture,
    Model,
    Material,
    Script,
    Audio,
    Unknown
};

// 资产项结构
struct AssetItem {
    std::string name;
    std::string path;
    AssetType type;
    bool isSelected = false;
};

// 资产浏览器类
class AssetBrowser {
private:
    std::vector<AssetItem> m_assets;          // 当前目录下的资产
    std::vector<std::string> m_currentPath;   // 当前路径
    std::string m_searchQuery;                // 搜索查询
    bool m_showPreview = true;                // 是否显示预览
    bool m_useListView = false;               // 是否使用列表视图(否则为网格视图)
    float m_thumbnailSize = 96.0f;            // 缩略图大小
    std::string m_selectedAsset;              // 选中的资产路径

    // 模拟资产数据 - 实际应用中应从文件系统加载
    void LoadDummyAssets() {
        m_assets.clear();
        
        // 添加文件夹
        m_assets.push_back({"Textures", GetCurrentFullPath() + "/Textures", AssetType::Folder, false});
        m_assets.push_back({"Models", GetCurrentFullPath() + "/Models", AssetType::Folder, false});
        m_assets.push_back({"Materials", GetCurrentFullPath() + "/Materials", AssetType::Folder, false});
        m_assets.push_back({"Scripts", GetCurrentFullPath() + "/Scripts", AssetType::Folder, false});
        m_assets.push_back({"Audio", GetCurrentFullPath() + "/Audio", AssetType::Folder, false});
        
        // 根据当前路径添加不同的文件
        if (m_currentPath.empty() || m_currentPath.back() == "Textures") {
            m_assets.push_back({"wooden_plank.png", GetCurrentFullPath() + "/wooden_plank.png", AssetType::Texture, false});
            m_assets.push_back({"metal_surface.png", GetCurrentFullPath() + "/metal_surface.png", AssetType::Texture, false});
            m_assets.push_back({"grass_field.jpg", GetCurrentFullPath() + "/grass_field.jpg", AssetType::Texture, false});
            m_assets.push_back({"character_face.png", GetCurrentFullPath() + "/character_face.png", AssetType::Texture, false});
        } else if (m_currentPath.back() == "Models") {
            m_assets.push_back({"tree.obj", GetCurrentFullPath() + "/tree.obj", AssetType::Model, false});
            m_assets.push_back({"character.fbx", GetCurrentFullPath() + "/character.fbx", AssetType::Model, false});
            m_assets.push_back({"building.gltf", GetCurrentFullPath() + "/building.gltf", AssetType::Model, false});
        } else if (m_currentPath.back() == "Materials") {
            m_assets.push_back({"red_plastic.mat", GetCurrentFullPath() + "/red_plastic.mat", AssetType::Material, false});
            m_assets.push_back({"glass.mtl", GetCurrentFullPath() + "/glass.mtl", AssetType::Material, false});
        } else if (m_currentPath.back() == "Scripts") {
            m_assets.push_back({"player_controller.lua", GetCurrentFullPath() + "/player_controller.lua", AssetType::Script, false});
            m_assets.push_back({"enemy_ai.js", GetCurrentFullPath() + "/enemy_ai.js", AssetType::Script, false});
        } else if (m_currentPath.back() == "Audio") {
            m_assets.push_back({"background_music.mp3", GetCurrentFullPath() + "/background_music.mp3", AssetType::Audio, false});
            m_assets.push_back({"footsteps.wav", GetCurrentFullPath() + "/footsteps.wav", AssetType::Audio, false});
        }
    }

    // 获取当前完整路径字符串
    std::string GetCurrentFullPath() {
        std::string path;
        for (size_t i = 0; i < m_currentPath.size(); ++i) {
            if (i > 0) path += "/";
            path += m_currentPath[i];
        }
        return path.empty() ? "/" : path;
    }

    // 根据文件名获取资产类型
    AssetType GetAssetTypeFromName(const std::string& name) {
        size_t dotPos = name.find_last_of('.');
        if (dotPos == std::string::npos) return AssetType::Folder;
        
        std::string ext = name.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "tga" || ext == "bmp")
            return AssetType::Texture;
        if (ext == "obj" || ext == "fbx" || ext == "gltf" || ext == "glb" || ext == "dae")
            return AssetType::Model;
        if (ext == "mat" || ext == "mtl")
            return AssetType::Material;
        if (ext == "lua" || ext == "js" || ext == "py" || ext == "cpp" || ext == "h")
            return AssetType::Script;
        if (ext == "mp3" || ext == "wav" || ext == "ogg" || ext == "flac")
            return AssetType::Audio;
            
        return AssetType::Unknown;
    }

    // 绘制资产图标
    void DrawAssetIcon(AssetType type, float size) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        
        switch (type) {
            case AssetType::Folder:
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "📁");
                break;
            case AssetType::Texture:
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "🖼️");
                break;
            case AssetType::Model:
                ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "📦");
                break;
            case AssetType::Material:
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "🎨");
                break;
            case AssetType::Script:
                ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "📜");
                break;
            case AssetType::Audio:
                ImGui::TextColored(ImVec4(0.7f, 0.3f, 1.0f, 1.0f), "🎵");
                break;
            default:
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "📄");
        }
        
        ImGui::PopFont();
    }

    // 过滤资产列表
    std::vector<AssetItem> GetFilteredAssets() {
        std::vector<AssetItem> filtered;
        
        if (m_searchQuery.empty()) {
            return m_assets;
        }
        
        std::string queryLower = m_searchQuery;
        std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);
        
        for (const auto& asset : m_assets) {
            std::string nameLower = asset.name;
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            
            if (nameLower.find(queryLower) != std::string::npos) {
                filtered.push_back(asset);
            }
        }
        
        return filtered;
    }

public:
    AssetBrowser() {
        // 初始化根目录
        LoadDummyAssets();
    }

    // 绘制资产浏览器窗口
    void Draw(const char* windowTitle = "Asset Browser") {
        ImGui::Begin(windowTitle, nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);
        
        // 菜单栏
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("Show Preview", &m_showPreview);
                ImGui::Checkbox("List View", &m_useListView);
                ImGui::SliderFloat("Thumbnail Size", &m_thumbnailSize, 64.0f, 256.0f);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        // 路径导航栏
        ImGui::TextUnformatted("Path:");
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        
        // 根目录按钮
        if (ImGui::Button("/")) {
            m_currentPath.clear();
            LoadDummyAssets();
        }
        
        // 路径中的每个文件夹
        for (size_t i = 0; i < m_currentPath.size(); ++i) {
            ImGui::SameLine();
            ImGui::TextUnformatted(">");
            ImGui::SameLine();
            
            if (ImGui::Button(m_currentPath[i].c_str())) {
                m_currentPath.erase(m_currentPath.begin() + i + 1, m_currentPath.end());
                LoadDummyAssets();
                break;
            }
        }
        
        ImGui::PopStyleColor(3);
        
        // 搜索框
        ImGui::SameLine(ImGui::GetWindowWidth() - 250);
        ImGui::SetNextItemWidth(200);
        if (ImGui::InputText("##Search", (char*)(m_searchQuery.c_str()), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
            // 搜索提交时的处理
        }
        ImGui::SameLine();
        if (ImGui::Button("X##ClearSearch") && !m_searchQuery.empty()) {
            m_searchQuery.clear();
        }
        
        ImGui::Separator();
        
        // 左侧文件夹树
        ImGui::BeginChild("##Folders", ImVec2(200, 0), true);
        ImGui::TextUnformatted("Folders");
        ImGui::Separator();
        
        // 绘制文件夹树
        DrawFolderTreeNode("Textures");
        DrawFolderTreeNode("Models");
        DrawFolderTreeNode("Materials");
        DrawFolderTreeNode("Scripts");
        DrawFolderTreeNode("Audio");
        
        ImGui::EndChild();
        ImGui::SameLine();
        
        // 右侧资产视图
        ImGui::BeginChild("##Assets", ImVec2(0, 0), true);
        
        // 视图切换工具栏
        ImGui::TextUnformatted("Assets");
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        if (ImGui::Button(m_useListView ? "📱 List View" : "🔲 Grid View")) {
            m_useListView = !m_useListView;
        }
        
        ImGui::Separator();
        
        // 获取过滤后的资产
        auto filteredAssets = GetFilteredAssets();
        
        if (m_useListView) {
            // 列表视图
            ImGui::BeginTable("##AssetList", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            
            for (auto& asset : filteredAssets) {
                ImGui::TableNextRow();
                
                // 名称列
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Selectable(asset.name.c_str(), &asset.isSelected, 
                                     ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                    if (asset.type == AssetType::Folder) {
                        m_currentPath.push_back(asset.name);
                        LoadDummyAssets();
                    } else {
                        m_selectedAsset = asset.path;
                    }
                }
                
                // 类型列
                ImGui::TableSetColumnIndex(1);
                switch (asset.type) {
                    case AssetType::Folder: ImGui::TextUnformatted("Folder"); break;
                    case AssetType::Texture: ImGui::TextUnformatted("Texture"); break;
                    case AssetType::Model: ImGui::TextUnformatted("Model"); break;
                    case AssetType::Material: ImGui::TextUnformatted("Material"); break;
                    case AssetType::Script: ImGui::TextUnformatted("Script"); break;
                    case AssetType::Audio: ImGui::TextUnformatted("Audio"); break;
                    default: ImGui::TextUnformatted("Unknown");
                }
                
                // 路径列
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(asset.path.c_str());
            }
            
            ImGui::EndTable();
        } else {
            // 网格视图
            int columns = std::max(1, (int)(ImGui::GetWindowWidth() / (m_thumbnailSize + 16)));
            ImGui::Columns(columns, nullptr, false);
            
            for (auto& asset : filteredAssets) {
                // 资产卡片
                ImVec2 cellSize(m_thumbnailSize, m_thumbnailSize + 30);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
                
                // 选中状态背景
                if (asset.isSelected) {
                    ImVec2 min = ImGui::GetItemRectMin();
                    ImVec2 max ={ ImGui::GetItemRectMin().x + cellSize.x, ImGui::GetItemRectMin().y + cellSize.y};
                    ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::GetColorU32(ImGuiCol_HeaderActive), 4.0f);
                }
                
                // 资产图标/缩略图
                ImGui::BeginChild(asset.name.c_str(), cellSize, true);
                ImGui::SetCursorPosX((cellSize.x - 32) / 2);
                ImGui::SetCursorPosY((cellSize.y - 60) / 2);
                DrawAssetIcon(asset.type, 32);
                
                // 资产名称（自动换行）
                ImGui::SetCursorPosY(cellSize.y - 30);
                ImGui::TextWrapped("%s", asset.name.c_str());
                
                ImGui::EndChild();
                
                // 处理点击
                if (ImGui::IsItemClicked()) {
                    // 清除其他选中状态
                    for (auto& a : m_assets) a.isSelected = false;
                    asset.isSelected = true;
                    
                    if (asset.type == AssetType::Folder) {
                        m_currentPath.push_back(asset.name);
                        LoadDummyAssets();
                    } else {
                        m_selectedAsset = asset.path;
                    }
                }
                
                ImGui::NextColumn();
                ImGui::PopStyleVar(2);
            }
            
            ImGui::Columns(1);
        }
        
        ImGui::EndChild();
        
        // 底部状态栏
        ImGui::Separator();
        ImGui::Text("Selected: %s", m_selectedAsset.empty() ? "None" : m_selectedAsset.c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - 150);
        ImGui::Text("%d assets", filteredAssets.size());
        
        ImGui::End();
    }

    // 绘制文件夹树节点
    void DrawFolderTreeNode(const std::string& folderName) {
        bool isCurrent = !m_currentPath.empty() && m_currentPath.back() == folderName;
        ImGui::PushStyleColor(ImGuiCol_Text, isCurrent ? ImVec4(1.0f, 0.8f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        if (ImGui::TreeNodeEx(folderName.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
            if (ImGui::IsItemClicked()) {
                // 如果已经在当前文件夹，点击则返回上一级
                if (isCurrent && m_currentPath.size() > 0) {
                    m_currentPath.pop_back();
                } 
                // 否则进入该文件夹
                else if (!isCurrent) {
                    // 清除当前路径并进入新文件夹
                    m_currentPath.clear();
                    m_currentPath.push_back(folderName);
                }
                LoadDummyAssets();
            }
            ImGui::TreePop();
        }
        
        ImGui::PopStyleColor();
    }
};

// 使用示例
// 在你的ImGui渲染循环中添加：
// 
// static AssetBrowser assetBrowser;
// assetBrowser.Draw();
