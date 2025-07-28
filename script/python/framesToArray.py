import PIL.Image
import os

def func(folderPath,row=20,column=20,width=200,height=200):
    files=[]
    for a,b,c in os.walk(folderPath):
        for filename in c:
            filePath=os.path.join(a,filename)
            files.append(filePath)
    print(files)
    imgSave=PIL.Image.new('RGBA',size=(column*width,row*height),color=(0,0,0,0))
    flag=0
    for file in files:
        pic=PIL.Image.open(file)
        pic=pic.resize(size=(width,height))
        imgSave.paste(pic,(int(flag%column)*width,int(flag/column)*height),pic)
        flag+=1
    imgSave.save('C:/Users/WS/Desktop/comb.png')
    
        
        

    
func("C:/Users/WS/Desktop/anim")