# 正在开发中
* 代码架构: framework.txt

# ** 时刻保持代码整洁 **
希望写的代码就像一首诗~ （欢迎针对不足之处给出建议~）
* 3.5 重构shader结构，数据传输更简洁，有图形API内味了。
* 3.7- 重构renderloop，封装CPU和GPU 两个stage阶段，适配多pass，为shadow做准备。(假GPU Stage，其实都是CPU在执行

# 计划

* shadowMap

* 立方体贴图

* 纹理过大：mipmap

* PBR

* 重写数学库

# 目前进度

帧缓冲，读取模型和贴图，矩阵变换，透视投影，透视校正，光栅化，纹理，加入QTGUI界面，相机，背面裁剪，提取出几何着色器和片段着色器，法线贴图，blinn-Phone光照
![图片](https://github.com/LinRayx/softRenderer/blob/master/image/output1.png)

