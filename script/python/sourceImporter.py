import tkinter as tk
from module.tkpad import TkPad
import os
class Opt:
    def __init__(self):
        pass
class Pad(TkPad):
    def __init__(self):
        super().__init__()
        self.title("资源导入器")
        self.geometry("800x800")
        self.createMenuCommand(self.file_menu,"文件",lambda:print("test"))
        self.btn=tk.Button(self.main_frame,text="test")
        self.btn.grid(row=1,column=1)

if __name__=="__main__":
    Pad().mainloop()



