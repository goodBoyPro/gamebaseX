import tkinter as tk
from tkinter import ttk, messagebox
import random
class TkPad(tk.Tk):
    def __init__(self):
        super().__init__()
        
        # 设置窗口标题和尺寸
        self.title("Tkinter 示例应用")
        self.geometry("600x400")
        self.resizable(True, True)
        
        # 创建主框架
        self.main_frame = ttk.Frame(self, padding="20")
        self.main_frame.pack(fill=tk.BOTH, expand=True)

        #主菜单
        self.mainMenu=tk.Menu(self)
        self.file_menu = tk.Menu(self.mainMenu, tearoff=0)
        self.mainMenu.add_cascade(label="文件", menu=self.file_menu)
        self.edit_menu = tk.Menu(self.mainMenu, tearoff=0)
        self.mainMenu.add_cascade(label="编辑", menu=self.edit_menu)
        self.option_menu = tk.Menu(self.mainMenu, tearoff=0)
        self.mainMenu.add_cascade(label="设置", menu=self.option_menu)        
        self.config(menu=self.mainMenu)
        
        
        
    def createMenuCommand(self,menu:tk.Menu,text,command=None,quikKey=None,addSep=False):
        menu.add_command(label=text,command=command,accelerator=quikKey)
        if addSep:
            menu.add_separator()
        
if __name__=="__main__":      
    TkPad().mainloop()