# GitHub Badges 配置

## 可用的 Badge 样式

将以下 badge 添加到 README.md 顶部以显示项目状态：

### 1. 构建状态

```markdown
![Build Status](https://github.com/darknesstm/ClipboardAsFile/actions/workflows/build-release.yml/badge.svg)
```

效果：![Build Status](https://github.com/darknesstm/ClipboardAsFile/actions/workflows/build-release.yml/badge.svg)

### 2. 最新版本

```markdown
![GitHub release (latest by date)](https://img.shields.io/github/v/release/darknesstm/ClipboardAsFile)
```

效果：![GitHub release (latest by date)](https://img.shields.io/github/v/release/darknesstm/ClipboardAsFile)

### 3. 下载次数

```markdown
![GitHub all releases](https://img.shields.io/github/downloads/darknesstm/ClipboardAsFile/total)
```

效果：![GitHub all releases](https://img.shields.io/github/downloads/darknesstm/ClipboardAsFile/total)

### 4. 许可证

```markdown
![License](https://img.shields.io/github/license/darknesstm/ClipboardAsFile)
```

效果：![License](https://img.shields.io/github/license/darknesstm/ClipboardAsFile)

### 5. 代码大小

```markdown
![GitHub repo size](https://img.shields.io/github/repo-size/darknesstm/ClipboardAsFile)
```

效果：![GitHub repo size](https://img.shields.io/github/repo-size/darknesstm/ClipboardAsFile)

### 6. 平台支持

```markdown
![Platform](https://img.shields.io/badge/platform-Windows%207%2B-blue)
```

效果：![Platform](https://img.shields.io/badge/platform-Windows%207%2B-blue)

### 7. 编程语言

```markdown
![Language](https://img.shields.io/github/languages/top/darknesstm/ClipboardAsFile)
```

效果：![Language](https://img.shields.io/github/languages/top/darknesstm/ClipboardAsFile)

## 推荐的 README 顶部

在 README.md 顶部添加：

```markdown
# ClipboardAsFile - 剪贴板转文件工具

![Build Status](https://github.com/darknesstm/ClipboardAsFile/actions/workflows/build-release.yml/badge.svg)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/darknesstm/ClipboardAsFile)
![GitHub all releases](https://img.shields.io/github/downloads/darknesstm/ClipboardAsFile/total)
![Platform](https://img.shields.io/badge/platform-Windows%207%2B-blue)
![License](https://img.shields.io/badge/license-Custom-lightgrey)

> 快速将剪贴板文本转换为文件的 Windows 工具

[下载](https://github.com/darknesstm/ClipboardAsFile/releases/latest) | [快速开始](快速开始.md) | [完整指南](完整功能指南.md)

---
```

## 自定义 Badge

使用 shields.io 创建自定义 badge：

```markdown
![Custom Badge](https://img.shields.io/badge/标签-内容-颜色)
```

**颜色选项：**
- brightgreen / green / yellowgreen / yellow
- orange / red / lightgrey / blue

**示例：**

```markdown
![Status](https://img.shields.io/badge/status-stable-brightgreen)
![Version](https://img.shields.io/badge/version-v1.1.0-blue)
![C++](https://img.shields.io/badge/C%2B%2B-14-blue)
```

## 工作流状态 Badge

针对不同的 workflow：

```markdown
<!-- 主构建 -->
![Build](https://github.com/darknesstm/ClipboardAsFile/actions/workflows/build.yml/badge.svg)

<!-- 多平台构建 -->
![Release Build](https://github.com/darknesstm/ClipboardAsFile/actions/workflows/build-release.yml/badge.svg)

<!-- 代码质量 -->
![Code Quality](https://github.com/darknesstm/ClipboardAsFile/actions/workflows/code-quality.yml/badge.svg)
```

## 完整示例

```markdown
# ClipboardAsFile

<div align="center">

![Build](https://github.com/darknesstm/ClipboardAsFile/actions/workflows/build-release.yml/badge.svg)
![Release](https://img.shields.io/github/v/release/darknesstm/ClipboardAsFile)
![Downloads](https://img.shields.io/github/downloads/darknesstm/ClipboardAsFile/total)
![Platform](https://img.shields.io/badge/platform-Windows%207%2B-blue)
![C++](https://img.shields.io/badge/C%2B%2B-14-blue)
![License](https://img.shields.io/badge/license-Custom-lightgrey)

### 快速将剪贴板文本转换为文件的 Windows 工具

[?? 下载最新版本](https://github.com/darknesstm/ClipboardAsFile/releases/latest) · 
[?? 快速开始](快速开始.md) · 
[?? 完整指南](完整功能指南.md) · 
[?? 报告问题](https://github.com/darknesstm/ClipboardAsFile/issues)

</div>

---

## ? 特性

- ?? **双快捷键支持** - 两种操作模式满足不同需求
- ?? **静默通知** - 气泡提示不打断工作
- ...
```

## 更新建议

在每次发布新版本后：
1. 确认所有 badge 正常显示
2. 下载次数会自动更新
3. 版本号会自动更新
4. 构建状态会实时反映

## 相关链接

- Shields.io: https://shields.io
- GitHub Badges: https://docs.github.com/en/actions/monitoring-and-troubleshooting-workflows/adding-a-workflow-status-badge
- Simple Icons: https://simpleicons.org (用于图标)
