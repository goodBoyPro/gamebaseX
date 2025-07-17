import tkinter as tk
from tkinter import ttk, messagebox
import random

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        
        # 设置窗口标题和尺寸
        self.title("Tkinter 示例应用")
        self.geometry("600x400")
        self.resizable(True, True)
        
        # 创建主框架
        self.main_frame = ttk.Frame(self, padding="20")
        self.main_frame.pack(fill=tk.BOTH, expand=True)
        
        # 创建界面元素
        self.create_widgets()
        
        # 绑定事件
        self.bind_events()
        
        # 初始化数据
        self.items = []
        self.update_item_list()
    
    def create_widgets(self):
        """创建界面元素"""
        # 标题标签
        ttk.Label(self.main_frame, text="Tkinter 示例应用", font=("Arial", 16, "bold")).grid(
            row=0, column=0, columnspan=2, pady=(0, 20))
        
        # 输入框和按钮
        ttk.Label(self.main_frame, text="输入文本:").grid(row=1, column=0, sticky=tk.W, pady=5)
        self.text_entry = ttk.Entry(self.main_frame, width=40)
        self.text_entry.grid(row=1, column=1, sticky=tk.W, pady=5)
        
        self.add_button = ttk.Button(self.main_frame, text="添加项目", command=self.add_item)
        self.add_button.grid(row=2, column=0, pady=10)
        
        self.clear_button = ttk.Button(self.main_frame, text="清空列表", command=self.clear_items)
        self.clear_button.grid(row=2, column=1, pady=10)
        
        # 项目列表
        ttk.Label(self.main_frame, text="项目列表:").grid(row=3, column=0, sticky=tk.W, pady=5)
        self.item_listbox = tk.Listbox(self.main_frame, width=50, height=10)
        self.item_listbox.grid(row=4, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=5)
        
        # 滚动条
        scrollbar = ttk.Scrollbar(self.main_frame, orient=tk.VERTICAL, command=self.item_listbox.yview)
        scrollbar.grid(row=4, column=2, sticky=(tk.N, tk.S))
        self.item_listbox.config(yscrollcommand=scrollbar.set)
        
        # 状态栏
        self.status_var = tk.StringVar()
        self.status_var.set("就绪")
        self.status_bar = ttk.Label(self, textvariable=self.status_var, relief=tk.SUNKEN, anchor=tk.W)
        self.status_bar.pack(side=tk.BOTTOM, fill=tk.X)
        
        # 创建菜单
        self.create_menu()
    
    def create_menu(self):
        """创建菜单栏"""
        menubar = tk.Menu(self)
        
        # 文件菜单
        file_menu = tk.Menu(menubar, tearoff=0)
        file_menu.add_command(label="新建", command=self.new_file, accelerator="Ctrl+N")
        file_menu.add_command(label="打开", command=self.open_file, accelerator="Ctrl+O")
        file_menu.add_separator()
        file_menu.add_command(label="退出", command=self.quit, accelerator="Ctrl+Q")
        menubar.add_cascade(label="文件", menu=file_menu)
        
        # 操作菜单
        action_menu = tk.Menu(menubar, tearoff=0)
        action_menu.add_command(label="随机添加", command=self.random_add)
        action_menu.add_separator()
        action_menu.add_command(label="关于", command=self.show_about)
        menubar.add_cascade(label="操作", menu=action_menu)
        
        # 设置菜单
        theme_menu = tk.Menu(menubar, tearoff=0)
        theme_menu.add_command(label="Light Theme", command=lambda: self.set_theme("clam"))
        theme_menu.add_command(label="Dark Theme", command=lambda: self.set_theme("alt"))
        menubar.add_cascade(label="主题", menu=theme_menu)
        
        # 设置菜单栏
        self.config(menu=menubar)
    
    def bind_events(self):
        """绑定事件处理"""
        # 绑定键盘快捷键
        self.bind("<Control-n>", lambda event: self.new_file())
        self.bind("<Control-o>", lambda event: self.open_file())
        self.bind("<Control-q>", lambda event: self.quit())
        
        # 绑定列表框双击事件
        self.item_listbox.bind("<Double-Button-1>", self.on_double_click)
    
    def add_item(self):
        """添加项目到列表"""
        text = self.text_entry.get().strip()
        if text:
            self.items.append(text)
            self.update_item_list()
            self.text_entry.delete(0, tk.END)
            self.status_var.set(f"已添加: {text}")
        else:
            self.status_var.set("请输入内容")
    
    def clear_items(self):
        """清空项目列表"""
        self.items = []
        self.update_item_list()
        self.status_var.set("列表已清空")
    
    def update_item_list(self):
        """更新项目列表显示"""
        self.item_listbox.delete(0, tk.END)
        for item in self.items:
            self.item_listbox.insert(tk.END, item)
    
    def random_add(self):
        """随机添加示例项目"""
        samples = ["苹果", "香蕉", "橙子", "草莓", "西瓜", "葡萄"]
        random_item = random.choice(samples)
        self.items.append(random_item)
        self.update_item_list()
        self.status_var.set(f"随机添加: {random_item}")
    
    def new_file(self):
        """新建文件操作"""
        if self.items and not messagebox.askyesno("确认", "是否清空当前数据？"):
            return
        self.clear_items()
        self.status_var.set("新建文件")
    
    def open_file(self):
        """打开文件操作"""
        messagebox.showinfo("提示", "文件打开功能尚未实现")
        self.status_var.set("打开文件")
    
    def show_about(self):
        """显示关于对话框"""
        messagebox.showinfo("关于", "Tkinter 示例应用\n版本 1.0\n用于演示 Tkinter 的基本功能")
    
    def on_double_click(self, event):
        """列表框双击事件处理"""
        selection = self.item_listbox.curselection()
        if selection:
            index = selection[0]
            item = self.items[index]
            messagebox.showinfo("项目详情", f"你选择了: {item}")
    
    def set_theme(self, theme_name):
        """设置主题"""
        try:
            self.style = ttk.Style()
            self.style.theme_use(theme_name)
            self.status_var.set(f"已切换到 {theme_name} 主题")
        except Exception as e:
            self.status_var.set(f"无法切换到 {theme_name} 主题")

if __name__ == "__main__":
    app = App()
    app.mainloop()    