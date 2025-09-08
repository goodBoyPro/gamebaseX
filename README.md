# 技术规范
## 注意
  1. 最小可感知单位：0.01(float)
## 1. 命名
 1. 静态图片
   - arr_id_level_row_column_centerX_centerY_name.png
 2. 动画图片
   - ani_id_level_row_column_centerX_centerY_name.png
 3. 资源应使用导入器导入
 4. 引用文件路径统一不使用./开头，直接使用res/name.file

## 2. cpp
 1. 使用工厂函数创建的对象必须显式定义默认构造函数，和init方法

 头文件中使用namespace{inline int a;}，会在每个包含该头文件的编译单元中生成一个独立的实例，inline作用被匿名空间覆盖。
 inline int a;所有编译单元共用一个实例。
 int a；重定义错误。