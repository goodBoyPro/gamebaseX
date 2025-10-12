#include <gui/imguiDx/imguiLib/imgui.h>
#include <vector>
#include"string"
#include <iostream>

// 用于显示 std::vector<int> 的 ImGui 子窗口浏览器
struct IntVectorBrowser
{
    // -------------------------- 核心数据（用户可直接修改/传入） --------------------------
    std::vector<int> Data;          // 要显示的 int 数组（核心数据）
    int              SelectedIndex; // 当前选中的元素索引（-1 表示未选中）
    const char*      WindowTitle;   // 主窗口标题
    const char*      ChildTitle;    // 子窗口标题

    // -------------------------- 构造函数（初始化默认值） --------------------------
    IntVectorBrowser(const char* main_title = "Int Vector Browser", 
                    const char* child_title = "Int Elements List")
        : SelectedIndex(-1), WindowTitle(main_title), ChildTitle(child_title)
    {
        // 默认添加10个测试数据（方便首次运行查看效果，可删除）
        AddTestData(50);
    }

    // -------------------------- 辅助功能（数据操作） --------------------------
    // 1. 添加测试数据（生成 0~count-1 的连续int）
    void AddTestData(int count)
    {
        if (count <= 0) return;
        // 预留内存，避免频繁扩容
        Data.reserve(Data.size() + count);
        int start_val = Data.empty() ? 0 : Data.back() + 1; // 从最后一个值+1开始
        for (int i = 0; i < count; i++)
            Data.push_back(start_val + i);
    }

    // 2. 清空所有数据
    void ClearData()
    {
        Data.clear();
        SelectedIndex = -1; // 清空时重置选中状态
    }

    // 3. 获取选中元素的值（未选中返回 0，可根据需求修改）
    int GetSelectedValue() const
    {
        return (SelectedIndex >= 0 && SelectedIndex < (int)Data.size()) 
            ? Data[SelectedIndex] 
            : 0;
    }

    // -------------------------- 核心 UI 绘制（子窗口浏览器核心） --------------------------
    void Draw(bool* p_open = nullptr)
    {
        // 1. 创建主窗口（子窗口必须嵌套在主窗口内）
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver); // 默认窗口大小
        if (!ImGui::Begin(WindowTitle, p_open))
        {
            ImGui::End(); // 主窗口未打开时，必须结束
            return;
        }

        // 2. 顶部菜单（用于操作数据：添加测试数据/清空）
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Data"))
            {
                // 添加10个测试数据（点击触发）
                if (ImGui::MenuItem("Add 10 Test Ints"))
                    AddTestData(10);
                // 清空所有数据（点击触发，仅当数据非空时可用）
                if (ImGui::MenuItem("Clear All Data", nullptr, false, !Data.empty()))
                    ClearData();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // 3. 绘制子窗口（核心：显示 vector<int> 的所有元素，支持滚动）
        // 子窗口大小：宽度填充主窗口，高度 = 主窗口高度 - 菜单高度 - 底部统计高度 - 边距
        const float child_height = ImGui::GetContentRegionAvail().y - 30; // 留30px显示统计信息
        if (ImGui::BeginChild(ChildTitle, ImVec2(0, child_height), true, 
           
            ImGuiChildFlags_Borders))                 // 显示子窗口边框
        {
            // 设置子窗口内边距（让元素不贴边，更美观）
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4)); // 元素垂直间距

            // 4. 遍历显示 vector<int> 的每个元素
            if (Data.empty())
            {
                // 数据为空时显示提示
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No int data to display (use 'Data->Add 10 Test Ints')");
            }
            else
            {
                for (size_t i = 0; i < Data.size(); i++)
                {
                    // 给每个元素分配唯一ID（ImGui 要求：同一窗口内元素ID不能重复）
                    ImGui::PushID((int)i); 

                    // 判断当前元素是否被选中
                    bool is_selected = (SelectedIndex == (int)i);

                    // 绘制可点击的元素项（点击时切换选中状态）
                    // Selectable：ImGui 基础交互组件，支持点击、选中高亮
                    if (ImGui::Selectable(
                        "",                  // 文本（这里留空，后面手动画文本）
                        is_selected,         // 是否选中
                        ImGuiSelectableFlags_SpanAllColumns, // 选中区域占满整行
                        ImVec2(0, 24)        // 每个元素项的高度（24px 方便点击）
                    ))
                    {
                        // 点击时更新选中索引（按 Ctrl 可取消选中，可选逻辑）
                        SelectedIndex = is_selected ? -1 : (int)i;
                    }

                    // 在 Selectable 右侧绘制元素信息：索引 + 值
                    // 注意：需要用 SameLine() 让文本和 Selectable 在同一行
                    ImGui::SameLine(10); // 距离左侧10px
                    // 选中元素用蓝色高亮，未选中用默认色
                    if (is_selected)
                        ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), 
                                           "Index: %-3zu | Value: %d", i, Data[i]);
                    else
                        ImGui::Text("Index: %-3zu | Value: %d", i, Data[i]);

                    ImGui::PopID(); // 释放当前元素的ID（避免冲突）
                }
            }

            ImGui::PopStyleVar(2); // 恢复之前设置的2个样式变量
        }
        ImGui::EndChild(); // 结束子窗口

        // 5. 底部统计信息（显示数据总数 + 当前选中元素）
        ImGui::Text("Total Elements: %zu | Selected: %s", 
                    Data.size(), 
                    (SelectedIndex >= 0) ? 
                    (std::to_string(GetSelectedValue()) + " (Index: " + std::to_string(SelectedIndex) + ")").c_str() : 
                    "None");

        ImGui::End(); // 结束主窗口
    }
};

// -------------------------- 使用示例（如何集成到你的 ImGui 项目中） --------------------------
// 假设你的项目已有 ImGui 初始化（如 GLFW/SDL 窗口 + ImGui 上下文创建）
// 在主循环中调用以下代码即可：
/*
// 1. 全局或静态创建浏览器实例（避免每次循环重新创建）
static IntVectorBrowser int_browser;

// 2. 在 ImGui 主循环中绘制
while (!glfwWindowShouldClose(window))
{
    // ... 其他 ImGui 初始化代码（NewFrame() 等）...

    // 绘制 int 数组浏览器
    int_browser.Draw();

    // ... 其他 ImGui 渲染代码（Render() 等）...
}
*/