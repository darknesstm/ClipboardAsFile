# ClipboardAsFile - 剪贴板转文件工具

## 功能说明

这是一个 Windows 应用程序，可以将剪贴板中的文本内容转换为文件，并将该文件放回剪贴板，方便直接粘贴为文件对象。

## 主要功能

### 1. 系统托盘图标
- 启动后程序以系统托盘图标方式运行，不显示主窗口
- 右键点击托盘图标可显示菜单：
  - **转换剪贴板**: 手动触发转换
  - **设置**: 打开设置对话框
  - **关于**: 显示关于信息
  - **退出**: 退出应用程序

### 2. 剪贴板文本转文件
- 读取剪贴板中的文本内容
- 将文本保存为临时文件（格式：clipboard_YYYYMMDD_HHMMSS.txt）
- 将文件对象放入剪贴板，可直接粘贴到文件夹或其他支持文件拖放的位置

### 3. 设置功能

#### 临时文件设置
- **临时文件目录**: 可自定义临时文件保存位置
  - 默认路径：`%LOCALAPPDATA%\ClipboardAsFile\Temp`
  - 可通过"浏览"按钮选择其他目录
- **自动删除时间**: 设置多少小时后自动删除旧文件
  - 设置为 0 表示不自动删除
  - 默认值：24 小时
  - 程序每小时检查一次并清理过期文件

#### 启动设置
- **随Windows启动**: 勾选后程序将随系统启动自动运行
  - 自动在注册表添加/删除启动项

#### 快捷键设置
- **启用全局快捷键**: 设置全局热键快速转换剪贴板
- **修饰键**: 支持 Ctrl、Alt、Shift 组合
- **按键**: 可选择 A-Z 任意字母
- 默认快捷键：Ctrl + Alt + V

### 4. 自动清理
- 程序运行时每小时自动检查并清理超过设定时间的临时文件
- 程序启动时立即执行一次清理

## 使用方法

### 基本使用流程
1. 运行程序，托盘区域会出现程序图标
2. 复制任意文本到剪贴板
3. 使用以下任一方式触发转换：
   - 按下全局快捷键（默认：Ctrl + Alt + V）
   - 右键点击托盘图标，选择"转换剪贴板"
4. 转换成功后，可以直接在文件夹中粘贴（Ctrl + V），文本将作为 .txt 文件被粘贴

### 配置说明
1. 右键托盘图标，选择"设置"
2. 根据需要调整各项设置
3. 点击"确定"保存设置

## 技术实现

### 文件结构
- **ClipboardAsFile.cpp**: 主程序文件，包含窗口处理和消息循环
- **Config.h**: 配置管理类，负责读写配置文件
- **ClipboardManager.h**: 剪贴板管理类，处理文本转文件和清理操作
- **AutoStartup.h**: 开机启动管理类，处理注册表操作
- **Resource.h**: 资源定义文件
- **ClipboardAsFile.rc**: 资源文件（需手动添加设置对话框和托盘菜单）

### 配置文件位置
- 配置保存在：`%LOCALAPPDATA%\ClipboardAsFile\config.ini`
- 使用 INI 格式存储，包含以下项：
  - TempPath: 临时文件目录
  - DeleteAfterHours: 自动删除时间
  - AutoStartup: 开机启动开关
  - HotkeyEnabled: 快捷键启用状态
  - HotkeyMod: 快捷键修饰符
  - HotkeyVk: 快捷键虚拟键码

### 注意事项

#### 资源文件手动修改
由于 .rc 文件无法通过工具直接编辑，需要手动在 Visual Studio 中打开 `ClipboardAsFile.rc` 文件，添加以下内容：

1. **添加托盘菜单**（在现有菜单定义后）：
```rc
// Tray menu
200 MENU
BEGIN
    POPUP ""
    BEGIN
        MENUITEM "转换剪贴板(&C)",          ID_TRAY_CONVERT
        MENUITEM SEPARATOR
        MENUITEM "设置(&S)...",             ID_TRAY_SETTINGS
        MENUITEM "关于(&A)...",             ID_TRAY_ABOUT
        MENUITEM SEPARATOR
        MENUITEM "退出(&X)",                ID_TRAY_EXIT
    END
END
```

2. **添加设置对话框**（在 IDD_ABOUTBOX 定义后）：
```rc
IDD_SETTINGS DIALOGEX 0, 0, 320, 180
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "设置"
FONT 9, "MS Shell Dlg"
BEGIN
    GROUPBOX        "临时文件设置",IDC_STATIC,7,7,306,60
    LTEXT           "临时文件目录:",IDC_STATIC,14,22,60,8
    EDITTEXT        IDC_TEMP_PATH,80,20,180,14,ES_AUTOHSCROLL
    PUSHBUTTON      "浏览...",IDC_BROWSE,265,20,40,14
    LTEXT           "自动删除时间(小时):",IDC_STATIC,14,42,100,8
    EDITTEXT        IDC_DELETE_AFTER,120,40,40,14,ES_AUTOHSCROLL | ES_NUMBER
    LTEXT           "0表示不自动删除",IDC_STATIC,165,42,80,8
    
    GROUPBOX        "启动设置",IDC_STATIC,7,72,306,30
    CONTROL         "随Windows启动",IDC_AUTO_STARTUP,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,86,120,10
    
    GROUPBOX        "快捷键设置",IDC_STATIC,7,107,306,43
    CONTROL         "启用全局快捷键",IDC_HOTKEY_ENABLED,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,121,80,10
    CONTROL         "Ctrl",IDC_HOTKEY_CTRL,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,100,121,35,10
    CONTROL         "Alt",IDC_HOTKEY_ALT,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,140,121,30,10
    CONTROL         "Shift",IDC_HOTKEY_SHIFT,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,175,121,35,10
    COMBOBOX        IDC_HOTKEY_KEY,215,119,50,100,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
    LTEXT           "按下快捷键转换剪贴板文本为文件",IDC_STATIC,14,134,150,8
    
    DEFPUSHBUTTON   "确定",IDOK,209,159,50,14
    PUSHBUTTON      "取消",IDCANCEL,263,159,50,14
END
```

3. **添加 DESIGNINFO**（在现有 DESIGNINFO 的 IDD_ABOUTBOX 部分后）：
```rc
    IDD_SETTINGS, DIALOG
    BEGIN
        LEFTMARGIN, 7
        RIGHTMARGIN, 313
        TOPMARGIN, 7
        BOTTOMMARGIN, 173
    END
```

完成以上修改后，重新编译项目即可。

## 系统要求
- Windows 7 或更高版本
- Visual C++ Runtime

## 编译要求
- Visual Studio 2019 或更高版本
- Windows SDK

## 许可证
版权所有 (c) 2026
