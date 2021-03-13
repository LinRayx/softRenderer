# 正在开发中

# 努力让代码整洁

* 3.5 重构shader结构，数据传输更简洁，有图形API内味了。
* 3.7 重构renderloop，形成CPU和GPU 两个stage阶段，封装模型render的函数，为shadow做准备。(假GPU Stage，其实都是CPU在执行
* 3.13 最近调MSAA和shadow的代码调的有点心累，导致代码有点乱了，得尽快整理一波

# 计划

* 立方体贴图

* 纹理过大：mipmap

* PBR

# 阶段总结

[初期总结](https://zhuanlan.zhihu.com/p/355347725)

[MSAA采坑记录](https://zhuanlan.zhihu.com/p/356752147)

[ShadowMap采坑记录](https://zhuanlan.zhihu.com/p/356862977)

# 目前进度

帧缓冲，读取模型和贴图，矩阵变换，透视投影，透视校正，光栅化，纹理，加入QTGUI界面，相机，背面裁剪，提取出几何着色器和片段着色器，法线贴图，blinn-Phone光照, MSAA，shadowMap

### BP光照+法线贴图

![图片](https://github.com/LinRayx/softRenderer/blob/master/image/output2.png)

### MSAA

![图片](https://pic1.zhimg.com/v2-df8a7f798712d5a7d2824d73627b8554_r.jpg)

### 阴影
![图片](https://pic1.zhimg.com/80/v2-8ab9206266aa3f3c2dc1d837f52edea4_1440w.jpg)