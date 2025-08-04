import tkinter as tk
from tkinter import ttk, messagebox
from tkinterdnd2 import DND_FILES, TkinterDnD
import random
import re
class TkPad(TkinterDnD.Tk):
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
    def setComponentBeDropFile(self,tkComponent,callback):
        tkComponent.drop_target_register(DND_FILES)
        tkComponent.dnd_bind('<<Drop>>',callback)
    def createChildWindow(self,bBlock=True):
        child = tk.Toplevel(self)
        child.title("子窗口")
        child.geometry("300x200")

        # 添加组件
        ttk.Label(child, text="这是子窗口内容").pack(pady=20)
        ttk.Button(child, text="关闭", command=child.destroy).pack()

       #是否阻塞主窗口
        if bBlock:
            child.grab_set()         
            child.transient(self) 
class LabelInput:
    def __init__(self,parent_,lableText,bNumber=False):
        self.valueInput=tk.StringVar()
        self.valueInput.set("0")
        
        self.frame=tk.Frame(parent_)
        self.frame.pack(fill="both", padx=5, pady=5)
        if bNumber:
            self.vcmd = (parent_.register(self.validate_input), '%P')
        else:self.vcmd =None
        self.label=tk.Label(self.frame,text=lableText)
        self.label.pack(side='left')
        self.input=tk.Entry(self.frame,textvariable=self.valueInput,validate='key',validatecommand=self.vcmd)
        self.input.pack(side='left')
        
        
    def validate_input(self,new_value):    
        try:
        # 尝试转换为浮点数（允许负数和小数）
            float(new_value)
            return True
        except ValueError:
            return new_value == ""  # 允许清空

            

if __name__=="__main__":      
    TkPad().mainloop()