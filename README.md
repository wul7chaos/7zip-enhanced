# 我的7-Zip修改版

**本项目是基于官方7-Zip(2501)的修改版本**

- 原作者：Igor Pavlov
- 原项目官网： `https://www.7-zip.org/`
- 原版许可证：主要为 GNU Lesser General Public License (LGPL) + unRAR restriction（详见 [DOC/License.txt](DOC/License.txt)）
- 本仓库遵守原版全部许可条款，仅对部分代码进行了修改/增加
- 修改内容仅供学习/个人使用，如用于商用请仔细阅读原版许可条款

## 新增功能

### 密码历史记录功能

本修改版为7-Zip添加了密码历史记录功能，方便用户在解压加密压缩包时快速选择之前使用过的密码。

#### 功能特性

- 自动保存最近使用的密码（最多保存16个）
- 在密码输入对话框中以下拉列表形式显示历史密码
- 支持密码去重，避免重复记录
- 密码数据存储在Windows注册表中
- 保留原有的"显示密码"功能

#### 使用方法

1. 解压加密压缩包时，会弹出密码输入对话框
2. 输入密码并点击确定后，密码会自动保存到历史记录中
3. 下次输入密码时，可以点击密码输入框右侧的下拉箭头
4. 从历史记录中选择之前使用过的密码

![密码历史记录演示](test.png)

## 修改说明

### 修改的文件

1. **CPP/7zip/UI/Common/ZipRegistry.h**
   - 添加了密码历史相关的数据结构定义
   - 添加了 `Save_PasswordHistory` 和 `Load_PasswordHistory` 函数声明

2. **CPP/7zip/UI/Common/ZipRegistry.cpp**
   - 实现了密码历史的保存和加载功能
   - 密码历史存储在注册表的 `Software\7-Zip\Extraction\PasswordHistory` 路径下
   - 实现了密码去重和数量限制（最多16个）

3. **CPP/7zip/UI/FileManager/PasswordDialog.rc**
   - 将密码输入框从单行编辑控件改为下拉列表控件（ComboBox）
   - 调整了对话框布局以适应新的控件

4. **CPP/7zip/UI/FileManager/PasswordDialog.h**
   - 将密码编辑控件替换为下拉列表控件
   - 移除了不必要的编辑控件成员变量

5. **CPP/7zip/UI/FileManager/PasswordDialog.cpp**
   - 在对话框初始化时加载密码历史记录
   - 在用户点击确定时保存新输入的密码
   - 实现了密码显示/隐藏功能与下拉列表控件的集成

### 技术实现细节

- 使用 Windows 注册表存储密码历史数据
- 采用与路径历史相同的存储模式
- 密码历史记录按时间顺序排列，最新的密码排在最前面
- 实现了密码去重逻辑，避免重复记录相同的密码
- 限制历史记录数量为16个，超出时删除最旧的记录

## 编译说明

本项目使用与原版7-Zip相同的编译环境和工具链。详细的编译方法请参考原版7-Zip的编译文档。

## 许可证

本修改版遵循原版7-Zip的所有许可条款：

- 大部分代码遵循 GNU Lesser General Public License (LGPL)
- 部分代码遵循 BSD 3-clause License
- unRAR 相关代码有额外的许可限制

完整的许可证信息请查看项目根目录下的 License.txt 文件。（详见 [DOC/License.txt](DOC/License.txt)）

## 免责声明

本修改版仅供学习和个人使用。修改者不对使用本软件造成的任何损失或数据安全问题负责。请妥善保管您的密码，不要在不安全的环境下使用密码历史记录功能。

## 致谢

感谢 Igor Pavlov 开发了优秀的 7-Zip 软件，并采用开源许可证允许用户进行修改和学习。
