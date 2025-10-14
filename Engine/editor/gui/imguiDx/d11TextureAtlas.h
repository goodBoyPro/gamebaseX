#ifndef D11TEXTUREATLAS_H
#define D11TEXTUREATLAS_H
#include <vector>
#include <algorithm>
#include <cmath>
#include"string"
#include <d3d11.h>
#include"imguiLib/imgui.h"
#include<unordered_map>
inline std::vector<const char*>imagePaths={"system/texture/a.png","system/texture/invalidTex.png"};
// 子图像在纹理集中的信息（包含UV坐标）
struct AtlasSubimage {
    const char* name;       // 子图像名称（用于索引）
    int x, y;               // 在纹理集中的像素坐标（左上角）
    int width, height;      // 子图像原始尺寸
    float u0, v0;           // 左上角UV坐标（归一化，0~1）
    float u1, v1;           // 右下角UV坐标（归一化，0~1）
};

// 纹理集整体信息
struct TextureAtlas {
    ID3D11ShaderResourceView* srv = nullptr;  // 纹理集的SRV
    int atlasWidth = 0;                       // 纹理集宽度
    int atlasHeight = 0;                      // 纹理集高度
    // std::vector<AtlasSubimage> subimages;     // 所有子图像信息
    std::unordered_map<std::string, AtlasSubimage>subimages;
};
inline TextureAtlas atlas;
// 辅助函数：计算不小于n的最小2的幂次方（GPU纹理优化）
inline int NextPowerOfTwo(int n) {
    if (n <= 0) return 1;
    return 1 << (int)ceil(log2(n));
}
inline void CleanupTextureAtlas() {
    // ① 释放 DX11 COM 对象（SRV）：必须调用 Release()，否则内存泄漏
    if (atlas.srv != nullptr) {
        atlas.srv->Release();   // 减少引用计数，触发资源释放（COM 机制）
        atlas.srv = nullptr;    // 置空指针，避免后续误操作（野指针风险）
    }

    // ② 清空子图像映射表：释放容器内所有元素，恢复初始状态
    atlas.subimages.clear();

    // ③ 重置纹理集尺寸：恢复为初始值，避免后续复用时代入旧值
    atlas.atlasWidth = 0;
    atlas.atlasHeight = 0;
}
// 生成纹理集
TextureAtlas CreateTextureAtlas(ID3D11Device *device);

// 从纹理集中查找子图像（通过名称匹配）
inline const AtlasSubimage* FindSubimageInAtlas(const char* targetName) {
  auto it = atlas.subimages.find(targetName);
  if (it == atlas.subimages.end()) {
    return nullptr;
  }
  return &(atlas.subimages[targetName]);
}

// 渲染纹理集中的指定子图像

inline void RenderAtlasImage(const AtlasSubimage *subimage, ImVec2 position,
                             ImVec2 size) {
   if (!subimage || !atlas.srv) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "BadImg");
        return;
    }

    // 1. 先设置光标位置 = 图像的屏幕左上角位置
    ImGui::SetCursorPos(position); 

    // 2. 调用ImGui::Image（参数顺序完全正确！）
    ImGui::Image(
        (ImTextureID)atlas.srv,       // 1. 纹理ID（DX11的SRV）
        size,                         // 2. 屏幕显示尺寸（宽x高，不是坐标！）
        ImVec2(subimage->u0, subimage->v0), // 3. UV左上角
        ImVec2(subimage->u1, subimage->v1), // 4. UV右下角
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f),     // 5. 染色（白色=无染色）
        ImVec4(0.0f, 0.0f, 0.0f, 0.0f)      // 6. 边框（透明=无边框）
    );
}
//开销大，建议使用另一个版本
inline void RenderAtlasImage(const char* imageName, ImVec2 position, ImVec2 size) {
    const AtlasSubimage* subimage = FindSubimageInAtlas(imageName);
    RenderAtlasImage(subimage,position,size);
}
///////////////////////////////////
ImTextureID LoadTextureSimple(ID3D11Device *device, const char *filePath);
#endif// D11TEXTUREATLAS_H