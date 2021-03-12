# 正在开发中

# 努力让代码整洁

* 3.5 重构shader结构，数据传输更简洁，有图形API内味了。
* 3.7 重构renderloop，形成CPU和GPU 两个stage阶段，封装模型render的函数，为shadow做准备。(假GPU Stage，其实都是CPU在执行
* 3.13 最近调MSAA和shadow的代码调的有点心累，导致代码有点乱了，得尽快整理一波

# 计划

* shadowMap

* 立方体贴图

* 纹理过大：mipmap

* PBR

# 阶段总结

[初期总结](https://zhuanlan.zhihu.com/p/355347725)
[MSAA采坑记录] (https://zhuanlan.zhihu.com/p/356752147)

# 目前进度

帧缓冲，读取模型和贴图，矩阵变换，透视投影，透视校正，光栅化，纹理，加入QTGUI界面，相机，背面裁剪，提取出几何着色器和片段着色器，法线贴图，blinn-Phone光照, MSAA
![图片](https://github.com/LinRayx/softRenderer/blob/master/image/output2.png)

