#ifndef D11TEXTUREATLAS_H
#define D11TEXTUREATLAS_H
#include <vector>
#include <algorithm>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
inline TextureAtlas CreateTextureAtlas(ID3D11Device* device) {
    if (atlas.srv != nullptr) {
        CleanupTextureAtlas();
    }
    std::vector<stbi_uc*> subimagePixels;  // 临时存储所有子图像的像素数据
    std::vector<AtlasSubimage> tempSubimages;  // 临时存储子图像尺寸信息

    // 第一步：加载所有子图像，获取尺寸和像素数据
    for (const char* path : imagePaths) {
        int w, h, channels;
        stbi_uc* pixels = stbi_load(path, &w, &h, &channels, STBI_rgb_alpha);  // 统一转为RGBA
        if (!pixels) {
            // 错误处理：释放已加载的资源并返回空纹理集
            for (auto p : subimagePixels) stbi_image_free(p);
            return atlas;
        }
        subimagePixels.push_back(pixels);
        tempSubimages.push_back({path, 0, 0, w, h, 0, 0, 0, 0});  // 暂存尺寸，坐标后续计算
    }

    // 第二步：计算纹理集的尺寸（按行优先排列）
    const int padding = 1;  // 子图像之间的间隔（防止采样时边缘混淆）
    int maxSubimageHeight = 0;
    int totalWidth = 0;
    
    // 先计算最大高度和总宽度（按单行排列的理论值）
    for (const auto& sub : tempSubimages) {
        maxSubimageHeight =((( maxSubimageHeight) > (sub.height)) ? ( maxSubimageHeight) : (sub.height));
        totalWidth += sub.width + padding;
    }

    // 确定纹理集尺寸（取2的幂次方）
    atlas.atlasWidth = NextPowerOfTwo(totalWidth);
    atlas.atlasHeight = NextPowerOfTwo(maxSubimageHeight);

    // 第三步：排列子图像（行优先，超出宽度则换行）
    int currentX = 0;
    int currentY = 0;

    for (size_t i = 0; i < tempSubimages.size(); ++i) {
        auto& sub = tempSubimages[i];
        // 如果当前行放不下，换行
        if (currentX + sub.width > atlas.atlasWidth) {
            currentX = 0;
            currentY += maxSubimageHeight + padding;
            // 确保不超过纹理集高度（实际项目中可能需要更复杂的排版算法）
            if (currentY + sub.height > atlas.atlasHeight) {
                // 错误处理：纹理集尺寸不足，释放资源
                for (auto p : subimagePixels) stbi_image_free(p);
                return atlas;
            }
        }
        // 记录子图像在纹理集中的位置
        sub.x = currentX;
        sub.y = currentY;
        currentX += sub.width + padding;
    }

    // 第四步：合并所有子图像的像素数据到纹理集
    const int pixelSize = 4;  // RGBA每个像素4字节
    size_t atlasDataSize = atlas.atlasWidth * atlas.atlasHeight * pixelSize;
    stbi_uc* atlasPixels = new stbi_uc[atlasDataSize]();  // 初始化全0（透明）

    for (size_t i = 0; i < tempSubimages.size(); ++i) {
        const auto& sub = tempSubimages[i];
        stbi_uc* srcPixels = subimagePixels[i];
        // 逐行复制子图像像素到纹理集
        for (int y = 0; y < sub.height; ++y) {
            // 计算纹理集当前行的起始地址
            stbi_uc* atlasRow = atlasPixels + (sub.y + y) * atlas.atlasWidth * pixelSize + sub.x * pixelSize;
            // 计算子图像当前行的起始地址
            stbi_uc* subRow = srcPixels + y * sub.width * pixelSize;
            // 复制一行像素（RGBA格式，每个像素4字节）
            memcpy(atlasRow, subRow, sub.width * pixelSize);
        }
    }

    // 第五步：创建DX11纹理集资源
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = atlas.atlasWidth;
    texDesc.Height = atlas.atlasHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = atlasPixels;
    subData.SysMemPitch = atlas.atlasWidth * pixelSize;  // 每行字节数

    ID3D11Texture2D* atlasTex = nullptr;
    HRESULT hr = device->CreateTexture2D(&texDesc, &subData, &atlasTex);
    if (FAILED(hr)) {
        delete[] atlasPixels;
        for (auto p : subimagePixels) stbi_image_free(p);
        return atlas;
    }

    // 创建Shader资源视图（SRV）
    hr = device->CreateShaderResourceView(atlasTex, nullptr, &atlas.srv);
    atlasTex->Release();  // 纹理对象可释放，SRV持有引用
    if (FAILED(hr)) {
        delete[] atlasPixels;
        for (auto p : subimagePixels) stbi_image_free(p);
        atlas.srv = nullptr;
        return atlas;
    }

    // 第六步：计算子图像的UV坐标（归一化到0~1范围）
    for (const auto& sub : tempSubimages) {
        AtlasSubimage atlasSub = sub;
        atlasSub.u0 = (float)sub.x / atlas.atlasWidth;
        atlasSub.v0 = (float)sub.y / atlas.atlasHeight;
        atlasSub.u1 = (float)(sub.x + sub.width) / atlas.atlasWidth;
        atlasSub.v1 = (float)(sub.y + sub.height) / atlas.atlasHeight;
        // atlas.subimages.push_back(atlasSub);
        atlas.subimages[atlasSub.name]=atlasSub;
    }

    // 清理临时资源
    delete[] atlasPixels;
    for (auto p : subimagePixels) stbi_image_free(p);

    return atlas;
}

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
inline ImTextureID LoadTextureSimple(ID3D11Device* device, const char* filePath) {
    // 1. 加载图片数据（RGBA 格式）
    int width, height, channels;
    stbi_uc* pixels = stbi_load(filePath, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels) {
        // 可选：添加错误日志
        return NULL;
    }
  
    // 2. 创建 DX11 纹理
    ID3D11Texture2D* tex = nullptr;
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  
    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = pixels;
    subData.SysMemPitch = width * 4;  // RGBA 每个像素 4 字节
  
    HRESULT hr = device->CreateTexture2D(&desc, &subData, &tex);
    stbi_image_free(pixels);  // 释放图像原始数据
  
    if (FAILED(hr)) {
        // 可选：添加错误日志
        return NULL;
    }
  
    // 3. 创建着色器资源视图（SRV）
    ID3D11ShaderResourceView* srv = nullptr;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
  
    hr = device->CreateShaderResourceView(tex, &srvDesc, &srv);
    tex->Release();  // 纹理对象可释放，SRV 会持有引用
  
    if (FAILED(hr)) {
        // 可选：添加错误日志
        return NULL;
    }
  
    // 4. 直接将 SRV 转换为 ImTextureID（这是正确做法）
    return (ImTextureID)srv;
  }
#endif// D11TEXTUREATLAS_H