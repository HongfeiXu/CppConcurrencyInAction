---
name: git-commit-helper
description: Git commit assistant. Triggers when user says "提交代码", "commit", "提交一下", "帮我提交", "push code", or any commit-related requests. Automatically lists changed files, generates commit message, asks for confirmation, executes git add/commit/push after approval.
---

# Git Commit Helper

辅助用户完成 Git 代码提交的交互式工作流。

## 工作流程

### 1. 收集变更信息

执行以下命令获取当前状态：

```bash
git status --porcelain
git diff --stat
```

将变更分类整理为：
- **新增 (A/?)**: 新创建的文件
- **修改 (M)**: 已修改的文件  
- **删除 (D)**: 已删除的文件

如有必要，使用 `git diff <file>` 查看具体改动内容以理解变更。

### 2. 生成 Commit Message

根据变更内容生成简洁的 **英文** commit message，遵循格式：

```
<type>: <short description in English>
```

常用 type：
- `feat`: 新功能
- `fix`: 修复 bug
- `refactor`: 重构
- `docs`: 文档更新
- `style`: 代码格式调整
- `chore`: 构建/工具变更

描述控制在 50 字符以内，使用英文小写开头。

### 3. 展示并确认

向用户展示：

```
📋 本次提交变更：

新增：
  - path/to/new_file.cpp

修改：
  - path/to/modified_file.cpp (+15, -3)

删除：
  - path/to/deleted_file.cpp

📝 建议 commit message：
  feat: add player collision detection

是否需要修改？直接告诉我修改内容，或回复「ok」确认提交。
```

### 4. 循环确认

- 用户提出修改 → 调整后重新展示步骤 3
- 用户确认（ok/好/确认/可以/行）→ 进入步骤 5

### 5. 执行提交

```bash
git add -A
git commit -m "<确认的 message>"
git push
```

如果 push 失败（如需要 pull），提示用户并询问是否自动处理。

## 注意事项

- 如果工作区干净（无变更），直接告知用户
- 如果有未跟踪的大文件或敏感文件，提醒用户注意
- 保持简洁，不要过度解释每个文件的变更
