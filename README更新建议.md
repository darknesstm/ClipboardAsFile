# 建议更新 README.md

在 README.md 的顶部添加以下内容以展示项目状态：

## 建议的新 README 头部

```markdown
# ClipboardAsFile - 剪贴板转文件工具

![Build Status](https://github.com/darknesstm/ClipboardAsFile/actions/workflows/build-release.yml/badge.svg)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/darknesstm/ClipboardAsFile)
![GitHub all releases](https://img.shields.io/github/downloads/darknesstm/ClipboardAsFile/total)
![Platform](https://img.shields.io/badge/platform-Windows%207%2B-blue)
![Language](https://img.shields.io/github/languages/top/darknesstm/ClipboardAsFile)

> ?? 快速将剪贴板文本转换为文件的 Windows 托盘工具

[?? 下载最新版本](https://github.com/darknesstm/ClipboardAsFile/releases/latest) | 
[?? 快速开始](快速开始.md) | 
[?? 完整指南](完整功能指南.md) |
[?? 报告问题](https://github.com/darknesstm/ClipboardAsFile/issues)

---

## ? 主要特性

- ?? **双快捷键支持** - 两种独立的操作模式
  - `Ctrl+Alt+V`: 转换到剪贴板（可多次粘贴）
  - `Ctrl+Shift+V`: 直接拖放到窗口（不修改剪贴板）
- ?? **静默通知** - Windows 气泡通知，不打断工作流
- ?? **智能拖放** - 优先使用 Explorer 专用 Drop 方法
- ?? **自动清理** - 定时清理过期临时文件
- ?? **高度可配置** - 快捷键、路径、清理时间均可自定义
- ?? **完整 Unicode** - 支持所有语言文字，UTF-8 编码

## ?? 快速下载

选择适合您系统的版本：

- **x64 版本**（推荐）: [ClipboardAsFile-latest-x64.zip](https://github.com/darknesstm/ClipboardAsFile/releases/latest)
- **Win32 版本**: [ClipboardAsFile-latest-Win32.zip](https://github.com/darknesstm/ClipboardAsFile/releases/latest)

## ?? 快速演示

### 场景1：快速转换（Ctrl+Alt+V）
```
1. 复制文本到剪贴板
2. 按 Ctrl+Alt+V
3. 看到"转换成功"通知
4. 在任意位置 Ctrl+V 粘贴文件
```

### 场景2：直接投放（Ctrl+Shift+V）
```
1. 打开目标文件夹
2. 复制文本到剪贴板
3. 按 Ctrl+Shift+V
4. 文件直接出现，剪贴板不变
```

## 功能说明

### 1. 系统托盘图标
...（保持原内容）
```

## 建议在文档末尾添加

```markdown
## ?? 自动构建

本项目使用 GitHub Actions 实现自动构建和发布：

- ? 推送代码自动编译测试
- ? 创建标签自动发布新版本
- ? 支持 x64 和 Win32 两个平台
- ? 自动创建 GitHub Releases

查看 [构建状态](https://github.com/darknesstm/ClipboardAsFile/actions)

## ?? 贡献

欢迎提交问题和功能请求！

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

## ?? 更新日志

### v1.1.0 (当前版本)
- ? 新增：双快捷键支持
- ? 新增：Explorer 专用 Drop 实现
- ? 新增：气泡通知系统
- ? 新增：GitHub Actions 自动构建
- ?? 修复：多行文本只保存第一行的问题
- ?? 修复：UTF-8 编码问题
- ?? 更新：完整的文档

### v1.0.0
- ?? 初始版本
- 基本的剪贴板转文件功能

[查看完整更新日志](https://github.com/darknesstm/ClipboardAsFile/releases)

## ?? 联系方式

- 项目主页: https://github.com/darknesstm/ClipboardAsFile
- 问题反馈: https://github.com/darknesstm/ClipboardAsFile/issues
- 功能建议: https://github.com/darknesstm/ClipboardAsFile/discussions

## ?? 致谢

感谢所有贡献者和使用者的支持！

## ?? 许可证

版权所有 (c) 2026

本软件按"原样"提供，不提供任何形式的明示或暗示担保。

---

**Made with ?? by [darknesstm](https://github.com/darknesstm)**
```

## 使用说明

1. **复制建议的头部**到现有 README.md 的开头
2. **添加建议的末尾内容**到现有 README.md 的结尾
3. **保留中间的功能说明**部分
4. **提交更改**并推送到 GitHub

## 预览效果

添加这些内容后，README 会：
- ? 显示实时构建状态
- ? 显示最新版本号
- ? 显示下载次数
- ? 提供快速下载链接
- ? 展示清晰的快速演示
- ? 包含更新日志
- ? 显示贡献指南

这将使您的项目看起来更专业和用户友好！
