# GitHub Actions 配置完成总结

## ? 已创建的文件

### 1. Workflow 配置文件
```
.github/workflows/
├── build.yml              # 基础构建 workflow
├── build-release.yml      # 多平台发布 workflow
└── code-quality.yml       # 代码质量检查 workflow
```

### 2. 文档文件
```
├── GitHub-Actions使用说明.md    # 完整的 Actions 使用指南
├── 快速发布指南.md              # 快速发布新版本的步骤
└── GitHub-Badges配置.md         # Badge 徽章配置说明
```

## ?? 功能概述

### build.yml - 基础构建
- **触发时机**: 推送代码、PR、手动触发
- **构建平台**: x64
- **输出**: 构建产物（保留 30 天）
- **特性**: 标签推送时自动创建 Release

### build-release.yml - 多平台发布 ?
- **触发时机**: 推送代码、推送标签、PR、手动触发
- **构建平台**: x64 + Win32
- **输出**: 两个平台的 ZIP 压缩包
- **特性**: 
  - 矩阵构建（并行编译）
  - 自动创建 Release
  - 包含完整文档
  - 美化的 Release 说明

### code-quality.yml - 质量检查
- **触发时机**: 推送代码、PR、手动触发
- **检查内容**: 
  - C++ 文件存在性
  - Debug 编译测试
  - 文档完整性
  - 资源文件验证

## ?? 快速开始

### 第一次使用

1. **确保代码已推送到 GitHub**
   ```bash
   git add .
   git commit -m "Add GitHub Actions workflows"
   git push origin master
   ```

2. **查看 Actions 页面**
   - 访问: https://github.com/darknesstm/ClipboardAsFile/actions
   - 应该能看到三个 workflow 已自动运行

3. **测试发布流程**
   ```bash
   # 创建测试标签
   git tag -a v1.1.0 -m "Test release"
   git push origin v1.1.0
   
   # 等待构建完成
   # 访问 Releases 页面查看结果
   ```

### 日常使用

**开发阶段（不发布）:**
```bash
git add .
git commit -m "Your changes"
git push origin master
# 自动触发构建，但不创建 Release
```

**发布新版本:**
```bash
git tag -a v1.2.0 -m "Release v1.2.0"
git push origin master
git push origin v1.2.0
# 自动构建并创建 Release
```

## ?? 完整的发布流程

### 步骤 1: 准备代码
```bash
# 1. 确保所有更改已提交
git status
git add .
git commit -m "Prepare for release v1.1.0"

# 2. 更新文档（如需要）
# - README.md
# - 快速开始.md
# - 完整功能指南.md

# 3. 本地测试编译
msbuild /m /p:Configuration=Release /p:Platform=x64 ClipboardAsFile.sln
```

### 步骤 2: 创建版本标签
```bash
# 创建标签
git tag -a v1.1.0 -m "Release version 1.1.0

主要更新：
- 添加双快捷键支持
- 实现 Explorer 专用 Drop
- 添加气泡通知系统
- 修复多行文本处理问题
"

# 验证标签
git tag -l
git show v1.1.0
```

### 步骤 3: 推送到 GitHub
```bash
# 推送代码
git push origin master

# 推送标签（触发自动发布）
git push origin v1.1.0
```

### 步骤 4: 监控构建
1. 访问 Actions 页面: https://github.com/darknesstm/ClipboardAsFile/actions
2. 找到 "Build Multi-Platform" workflow
3. 查看实时日志
4. 等待完成（约 5-10 分钟）

构建包含：
- ? 签出代码
- ? 设置构建环境
- ? 编译 x64 版本
- ? 编译 Win32 版本
- ? 创建 ZIP 压缩包
- ? 创建 GitHub Release
- ? 上传产物

### 步骤 5: 验证发布
1. 访问 Releases 页面: https://github.com/darknesstm/ClipboardAsFile/releases
2. 确认新版本存在
3. 下载并测试两个平台的 ZIP 文件

## ?? 配置说明

### 构建环境
- **运行环境**: Windows Server 2022
- **MSBuild**: 自动配置最新版本
- **NuGet**: 自动恢复依赖
- **7-Zip**: 用于创建压缩包

### 构建配置
```yaml
BUILD_CONFIGURATION: Release
BUILD_PLATFORM: x64 / Win32
SOLUTION_FILE_PATH: ClipboardAsFile.sln
```

### 产物内容
每个 ZIP 包含：
- `ClipboardAsFile.exe` - 主程序
- `README.md` - 基础说明
- `快速开始.md` - 快速上手
- `完整功能指南.md` - 详细文档
- `RC文件更新说明-双快捷键.md` - 开发说明

## ?? 版本号规范

遵循语义化版本控制（Semantic Versioning）：

```
v主版本.次版本.修订号

示例：
v1.0.0  - 初始版本
v1.1.0  - 添加新功能（双快捷键）
v1.1.1  - Bug 修复
v2.0.0  - 重大更新（不兼容）
```

**递增规则：**
- **主版本号**: 不兼容的 API 修改
- **次版本号**: 向下兼容的功能新增
- **修订号**: 向下兼容的问题修正

## ??? 实用工具

### 快速发布脚本

**PowerShell 版本:**
```powershell
# release.ps1
param([string]$Version)

git pull origin master
git tag -a $Version -m "Release $Version"
git push origin master
git push origin $Version

Write-Host "? 发布完成！" -ForegroundColor Green
Write-Host "监控: https://github.com/darknesstm/ClipboardAsFile/actions"
```

使用：
```powershell
.\release.ps1 -Version v1.1.0
```

### 查看构建状态

```bash
# 查看最新标签
git describe --tags --abbrev=0

# 查看标签之间的变更
git log v1.0.0..v1.1.0 --oneline

# 生成 Changelog
git log v1.0.0..v1.1.0 --pretty=format:"- %s" > CHANGELOG.md
```

## ?? 添加状态徽章

在 README.md 顶部添加：

```markdown
![Build Status](https://github.com/darknesstm/ClipboardAsFile/actions/workflows/build-release.yml/badge.svg)
![Release](https://img.shields.io/github/v/release/darknesstm/ClipboardAsFile)
![Downloads](https://img.shields.io/github/downloads/darknesstm/ClipboardAsFile/total)
![Platform](https://img.shields.io/badge/platform-Windows%207%2B-blue)
```

效果：
- ?? 构建状态（通过/失败）
- ??? 最新版本号
- ?? 总下载次数
- ?? 支持的平台

详细配置参见 `GitHub-Badges配置.md`

## ? 常见问题

### Q: 构建失败怎么办？
**A: 检查步骤：**
1. 查看 Actions 页面的详细日志
2. 本地测试编译是否通过
3. 确认所有文档文件已提交
4. 检查 RC 文件是否已更新

### Q: 如何删除错误的标签？
```bash
# 删除本地标签
git tag -d v1.1.0

# 删除远程标签
git push origin :refs/tags/v1.1.0

# 删除对应的 Release（在 GitHub 网页操作）
```

### Q: 如何修改已发布的 Release？
1. 在 GitHub Releases 页面找到对应版本
2. 点击 "Edit release"
3. 修改说明文字或上传新文件
4. 点击 "Update release"

### Q: 构建产物保留多久？
- **Artifacts**: 30 天（可在 workflow 中修改）
- **Releases**: 永久保留

### Q: 如何手动触发构建？
1. 访问 Actions 页面
2. 选择要运行的 workflow
3. 点击 "Run workflow" 按钮
4. 选择分支并确认

## ?? 安全建议

1. **不要提交敏感信息**
   - API 密钥
   - 密码
   - 证书私钥

2. **使用 GitHub Secrets**
   - 在仓库设置中添加
   - 在 workflow 中引用: `${{ secrets.SECRET_NAME }}`

3. **保护主分支**
   - 启用分支保护规则
   - 要求 PR 审查
   - 要求状态检查通过

## ?? 相关文档

- [GitHub Actions 使用说明](GitHub-Actions使用说明.md) - 详细的配置和使用指南
- [快速发布指南](快速发布指南.md) - 快速发布新版本的步骤
- [GitHub Badges 配置](GitHub-Badges配置.md) - 徽章配置说明

## ?? 下一步

1. **推送到 GitHub**
   ```bash
   git add .
   git commit -m "Add GitHub Actions workflows"
   git push origin master
   ```

2. **查看 Actions 运行**
   - 访问: https://github.com/darknesstm/ClipboardAsFile/actions

3. **创建第一个 Release**
   ```bash
   git tag -a v1.1.0 -m "First automated release"
   git push origin v1.1.0
   ```

4. **添加 Badges 到 README**
   - 参考 `GitHub-Badges配置.md`

## ? 检查清单

配置完成后确认：

- [ ] `.github/workflows/` 目录存在
- [ ] 三个 workflow 文件已创建
- [ ] 代码已推送到 GitHub
- [ ] Actions 页面能看到 workflows
- [ ] 尝试推送代码触发构建
- [ ] 测试创建标签触发发布
- [ ] 下载并测试发布的文件
- [ ] 添加 badges 到 README

---

**?? 恭喜！GitHub Actions 自动构建已配置完成！**

现在你可以：
- ? 自动编译 x64 和 Win32 版本
- ? 自动创建 GitHub Releases
- ? 自动上传构建产物
- ? 自动化的代码质量检查

享受自动化构建的便利吧！??
