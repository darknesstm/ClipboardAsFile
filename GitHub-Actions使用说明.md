# GitHub Actions 自动构建配置说明

## 概述

本项目已配置完整的 GitHub Actions CI/CD 流程，支持：
- ? 自动编译构建（x64 和 Win32 平台）
- ? 自动创建发布版本
- ? 代码质量检查
- ? 自动上传构建产物

## Workflow 文件说明

### 1. build.yml - 基础构建

**触发条件：**
- 推送到 master/main/develop 分支
- 创建 Pull Request 到 master/main 分支
- 手动触发

**构建内容：**
- x64 Release 版本
- 自动上传构建产物（保留 30 天）
- 标签推送时创建 GitHub Release

**使用方法：**
```bash
# 推送代码触发构建
git add .
git commit -m "Your changes"
git push origin master

# 或手动触发（在 GitHub Actions 页面）
```

### 2. build-release.yml - 多平台发布

**触发条件：**
- 推送到 master/main 分支
- 推送版本标签（v*）
- 创建 Pull Request
- 手动触发

**构建内容：**
- x64 Release 版本
- Win32 Release 版本
- 标签推送时创建包含两个平台的 Release

**发布流程：**
```bash
# 1. 更新版本号并提交
git add .
git commit -m "Release v1.1.0"

# 2. 创建标签
git tag v1.1.0

# 3. 推送标签触发发布
git push origin v1.1.0

# GitHub Actions 会自动：
# - 编译 x64 和 Win32 版本
# - 创建 ZIP 压缩包
# - 创建 GitHub Release
# - 上传编译产物
```

### 3. code-quality.yml - 代码质量检查

**触发条件：**
- 推送到 master/main/develop 分支
- 创建 Pull Request
- 手动触发

**检查内容：**
- C++ 文件存在性检查
- Debug 版本编译测试
- 文档完整性检查
- 资源文件验证

## 发布新版本步骤

### 完整发布流程

1. **准备发布**
   ```bash
   # 确保所有更改已提交
   git status
   git add .
   git commit -m "Prepare for release v1.1.0"
   ```

2. **创建版本标签**
   ```bash
   # 创建标签（版本号格式：v主版本.次版本.修订号）
   git tag -a v1.1.0 -m "Release version 1.1.0"
   
   # 查看标签
   git tag -l
   ```

3. **推送标签**
   ```bash
   # 推送代码和标签
   git push origin master
   git push origin v1.1.0
   ```

4. **监控构建**
   - 访问 GitHub 仓库的 Actions 页面
   - 查看 "Build Multi-Platform" workflow 运行状态
   - 等待构建完成（约 5-10 分钟）

5. **验证发布**
   - 访问 GitHub 仓库的 Releases 页面
   - 确认新版本已创建
   - 下载并测试发布的 ZIP 文件

### 版本号规范

遵循语义化版本控制（Semantic Versioning）：

```
v主版本.次版本.修订号

例如：
v1.0.0  - 初始版本
v1.1.0  - 添加新功能（双快捷键）
v1.1.1  - Bug 修复
v2.0.0  - 重大更新（不兼容旧版）
```

**版本号递增规则：**
- **主版本号**：不兼容的 API 修改
- **次版本号**：向下兼容的功能性新增
- **修订号**：向下兼容的问题修正

## GitHub Actions 配置详解

### 构建环境

**运行环境：** `windows-latest`（Windows Server 2022）

**预装工具：**
- MSBuild（Visual Studio Build Tools）
- NuGet
- 7-Zip
- Git

**构建配置：**
```yaml
BUILD_CONFIGURATION: Release
BUILD_PLATFORM: x64 或 Win32
```

### 构建产物

**x64 版本包含：**
```
ClipboardAsFile-v1.1.0-x64.zip
├── ClipboardAsFile.exe        # 主程序（x64）
├── README.md                  # 基础说明
├── 快速开始.md                 # 快速上手
├── 完整功能指南.md             # 详细文档
└── RC文件更新说明-双快捷键.md   # 开发说明
```

**Win32 版本包含：**
```
ClipboardAsFile-v1.1.0-Win32.zip
├── ClipboardAsFile.exe        # 主程序（Win32）
├── README.md
├── 快速开始.md
├── 完整功能指南.md
└── RC文件更新说明-双快捷键.md
```

### Release 页面说明

自动生成的 Release 包含：

1. **版本信息**
   - 版本号和发布日期
   - 主要功能列表
   - 使用方法简介

2. **下载链接**
   - x64 版本 ZIP（推荐）
   - Win32 版本 ZIP

3. **更新日志**
   - 自动生成的 Release Notes
   - 包含所有提交信息

4. **系统要求**
   - Windows 版本要求
   - 运行库要求

## 常见问题

### Q: 构建失败怎么办？

**A: 检查以下几点：**

1. **代码编译错误**
   ```bash
   # 本地先测试编译
   msbuild /m /p:Configuration=Release /p:Platform=x64 ClipboardAsFile.sln
   ```

2. **资源文件缺失**
   ```bash
   # 确保 RC 文件已按说明更新
   # 检查 Resource.h 中的 ID 定义
   ```

3. **文档文件缺失**
   ```bash
   # 确保所有文档都已提交
   git ls-files | grep -E "\.md$"
   ```

4. **查看构建日志**
   - 访问 Actions 页面
   - 点击失败的 workflow
   - 查看详细错误信息

### Q: 如何手动触发构建？

**A: 在 GitHub 页面操作：**

1. 访问仓库的 Actions 页面
2. 选择要运行的 workflow
3. 点击 "Run workflow" 按钮
4. 选择分支
5. 点击绿色的 "Run workflow" 按钮

### Q: 如何下载历史构建产物？

**A: 两种方式：**

1. **从 Actions 页面：**
   - 访问 Actions 页面
   - 点击具体的 workflow run
   - 在 Artifacts 部分下载

2. **从 Releases 页面：**
   - 访问 Releases 页面
   - 选择版本
   - 下载对应的 ZIP 文件

### Q: 如何修改构建配置？

**A: 编辑 workflow 文件：**

```yaml
# 修改 .github/workflows/build-release.yml

# 更改保留天数
retention-days: 90  # 改为 90 天

# 添加新平台
matrix:
  platform: [x64, Win32, ARM64]  # 添加 ARM64

# 更改构建配置
BUILD_CONFIGURATION: Debug  # 改为 Debug
```

### Q: 如何禁用某个 workflow？

**A: 两种方式：**

1. **临时禁用（GitHub 页面）：**
   - Actions → 选择 workflow → "..." → Disable workflow

2. **永久删除：**
   ```bash
   git rm .github/workflows/code-quality.yml
   git commit -m "Remove code quality check"
   git push
   ```

## 高级配置

### 自定义 Release 说明

编辑 `.github/workflows/build-release.yml` 中的 `body` 部分：

```yaml
- name: Create GitHub Release
  uses: softprops/action-gh-release@v1
  with:
    body: |
      ## 您的自定义说明
      
      ### 新功能
      - 功能1
      - 功能2
      
      ### Bug 修复
      - 修复1
      - 修复2
```

### 添加构建通知

在 workflow 末尾添加通知步骤：

```yaml
- name: Send notification
  if: always()
  run: |
    # 发送邮件、Slack、Discord 等通知
    # 需要配置相应的 secrets
```

### 配置构建缓存

加速构建过程：

```yaml
- name: Cache NuGet packages
  uses: actions/cache@v3
  with:
    path: ~/.nuget/packages
    key: ${{ runner.os }}-nuget-${{ hashFiles('**/packages.lock.json') }}
    restore-keys: |
      ${{ runner.os }}-nuget-
```

## 安全建议

### 保护敏感信息

**不要在代码中硬编码：**
- API 密钥
- 密码
- 证书私钥

**使用 GitHub Secrets：**
```yaml
env:
  MY_SECRET: ${{ secrets.MY_SECRET }}
```

### 签名配置（可选）

如需代码签名：

1. **添加证书到 Secrets**
   - 设置 → Secrets → New repository secret
   - 名称：CODE_SIGNING_CERT
   - 值：证书 Base64 编码

2. **在 workflow 中使用**
   ```yaml
   - name: Sign executable
     run: |
       # 解码证书
       # 使用 signtool 签名
   ```

## 监控与维护

### 定期检查

**每月检查：**
- ? Workflow 运行状态
- ? 构建成功率
- ? 存储空间使用

**每季度检查：**
- ? 更新 Actions 版本
- ? 清理旧的 Artifacts
- ? 审查 Workflow 配置

### 更新依赖

**Actions 版本更新：**
```yaml
# 从
uses: actions/checkout@v3

# 更新到
uses: actions/checkout@v4
```

**MSBuild 工具更新：**
```yaml
- name: Setup MSBuild
  uses: microsoft/setup-msbuild@v1.3  # 更新版本号
```

## 总结

GitHub Actions 配置完成后：

? **自动化构建**：推送代码自动编译  
? **多平台支持**：x64 和 Win32 同时构建  
? **版本管理**：标签自动创建 Release  
? **产物归档**：自动上传构建结果  
? **质量保证**：代码检查和文档验证  

**下一步：**
1. 推送代码到 GitHub 仓库
2. 查看 Actions 页面确认 workflow 运行
3. 创建第一个版本标签测试发布流程

有问题请查看 GitHub Actions 文档：https://docs.github.com/actions
