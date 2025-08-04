import tkinter as tk
from module.tkpad import TkPad
from module.tkpad import LabelInput
import os
import shutil
class Opt:
    def __init__(self):
        pass
class ChildPadPng(tk.Toplevel):
    def __init__(self, master=None,filePath_='C:/Users/WS/Desktop/g/test.py', **kwargs):
        # 移除冗余参数，使用更简洁的写法
        super().__init__(
            master,
            bg="#f0f0f0",         
            bd=0,                 
            relief="flat",        
            highlightthickness=0, 
            **kwargs
        )        
        self.filePath=filePath_
        self.title("导入图片")
        self.geometry("300x300")        
        # 正确设置模态对话框
        self.transient(master)  # 绑定到父窗口
        self.grab_set()         # 捕获焦点

        # inputId=tk.Entry(self).pack()
        self.inputRow=LabelInput(self,'行    数',True)
        self.inputColumn=LabelInput(self,'列    数',True)
        self.inputCenterX=LabelInput(self,'中心点x',True)
        self.inputCenterY=LabelInput(self,'中心点y',True)
        self.buttonImport=tk.Button(self,text="导入",command=self.buttonImportCbk)
        self.buttonImport.pack()
    def buttonImportCbk(self):
        row=int(float((self.inputRow.valueInput.get())))
        column=int(float(self.inputColumn.valueInput.get()))
        centerx=float(self.inputCenterX.valueInput.get())
        centery=float(self.inputCenterY.valueInput.get())
        name=os.path.basename(self.filePath)
        newName=f"arr_id_c_{row}_{column}_{centerx}_{centery}_{name}"
        
        dst="output/bin/res/"+newName
        shutil.copy2(self.filePath,dst)
        self.destroy()
        
        
        
 
    
class Pad(TkPad):
    def __init__(self):
        super().__init__()
        self.setComponentBeDropFile(self,lambda event:self.importPngPad(event.data))
        self.title("资源导入器")
        self.geometry("800x800")
        self.createMenuCommand(self.file_menu,"文件",lambda:print("test"))
        self.createMenuCommand(self.edit_menu,"撤销",lambda:print("撤销"))
        self.btn=tk.Button(self.main_frame,text="test")
        self.btn.grid(row=1,column=1)
        

        self.input1=LabelInput(self,'test')
    def importPngPad(self,path):
        self.childPad=ChildPadPng(self,path)
        

        pass
if __name__=="__main__":
    Pad().mainloop()



