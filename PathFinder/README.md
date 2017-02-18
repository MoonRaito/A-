
# [最短路径之A*搜索](http://my.oschina.net/vaero/blog/262744)

## 目录树

    PathFinder/
    ├─benchmark/
    ├─build/
    │  └─PathFinder-msvc.cbp  # test/与boost astar_search例子工程
    ├─src/                    # A* hpp 实现代码
    │  ├─core/
    │  └─finders/
    ├─test/                   # 单元测试
    ├─third_party/            # 需要引用boost库
    │  └─boost_1_55_0/
    ├─tools/
    └─visual/
        └─VisualPathFinder/   # A*可视化Qt工程

## 用法

includepath追加`src/`与`boost_1_55_0/`目录即可。

例如：`VisualPathFinder/`下Qt A*简单的可视化工程，修改`VisualPathFinder.pro`如下这段：

    LIBRARYPATH = /Users/John/Workspace/Library
    INCLUDEPATH += ../../src \
        $$LIBRARYPATH/boost_1_55_0

为你本机的路径即可。
