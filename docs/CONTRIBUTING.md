# Contributing

## Getting the repo (choose one)

### Option A: Clone (recommended for core team members with write access)
1) On GitHub, open the repository page and click **Code**.
2) Copy the HTTPS (or SSH) URL.
3) Clone locally:

```bash
git clone <REPO_URL>
cd microkernel-os
```

### Option B: Fork + clone (recommended if you don’t have write access)
1) On GitHub, click **Fork** to create your copy.
2) Clone your fork:

```bash
git clone <YOUR_FORK_URL>
cd microkernel-os
```

3) Add the upstream remote so you can sync later:

```bash
git remote add upstream <REPO_URL>
git remote -v
```

## One-time local setup

### Configure your identity
```bash
git config --global user.name "Your Name"
git config --global user.email "you@example.com"
```

### Authentication (HTTPS vs SSH)
- **HTTPS**: you may need a GitHub Personal Access Token (PAT) instead of a password.
- **SSH**: you need an SSH key added to your GitHub account.

If you can clone but cannot push, it’s usually permissions or auth.

## Daily workflow

### 1) Sync before you start
If you cloned the main repo:

```bash
git pull
```

If you’re working from a fork:

```bash
git fetch upstream
git checkout main
git merge upstream/main
git push origin main
```

### 2) Create a branch
Branch names:
- `feature/<name>`
- `fix/<name>`
- `docs/<name>`

```bash
git checkout -b feature/ipc-queue
```

## Branching
- Keep branches short-lived
- Keep PRs small and focused

### 3) Make changes, then commit
Check what changed:

```bash
git status
```

Stage changes:

```bash
git add -A
```

Commit (write a clear message):

```bash
git commit -m "ipc: add endpoint ring buffer"
```

### 4) Push your branch
```bash
git push -u origin feature/ipc-queue
```

### 5) Open a Pull Request (PR)
On GitHub, open a PR from your branch to `main`.
- Link the issue you worked on (see `docs/ISSUES.md`)
- Include QEMU output / screenshot for behavior changes

## PR rules
- Must boot in QEMU (`make run`) before requesting review
- Include logs/screenshots for behavior changes

## Code style
- Keep C code warning-clean where practical
- Prefer small functions and explicit naming

## Issue workflow
- Every PR links an issue
- Mark blockers clearly and ask early

## Troubleshooting

### `git add .` fails
Common causes:
- You’re not inside a git repo (no `.git/` directory). Fix by cloning the repo instead of downloading a ZIP.
- Git isn’t installed or isn’t on PATH.

To check if you’re in a repo:
```bash
git rev-parse --is-inside-work-tree
```

### Keeping your branch up to date
Before opening a PR (or if CI fails due to conflicts):

```bash
git fetch origin
git rebase origin/main
```

If you rebased, you may need a force push to your branch:
```bash
git push --force-with-lease
```
