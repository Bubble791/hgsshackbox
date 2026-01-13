## 使用注意
- 特性在某些情况时会根据pid重置为原本游戏设置的几个特性中的一个
- 修改等级后会根据当前等级重新生成技能组
- 修改宝可梦选项暂时只能修改首位宝可梦
- 按select会生成一个闪光PID

## 记录

由于include直接复制的反编译，某些带有enum结构体在不同编译器下类型不同，原版的编译器下是int类型注意修改

## TODO
- 宝可梦编号选择有问题

## 使用方法

参考[HGE](https://github.com/BluRosie/hg-engine)引擎的编译方法安装环境，把美版心金放到根目录命名为rom.nds，运行make指令生成game_patched.nds新rom