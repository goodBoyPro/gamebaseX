import tkinter as tk
from tkpad import TkPad
import os
class Opt:
    def __init__(self):
        pass
class Pad(TkPad):
    def __init__(self):
        super().__init__()
        self.title("资源导入器")
        self.geometry("800x800")
        self.createMenuCommand(self.file_menu,"wenjian",lambda:print("test"))
    

if __name__=="__main__":
    Pad().mainloop()



