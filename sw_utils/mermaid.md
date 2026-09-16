---
title: "Mermaid"
tags: [packages, mermaid]
draft: false
---

# Mermaid

主流 IDE 多支援渲染或有現成插件。為避免 CDN 服務中斷，備份手動編譯步驟。

----
## Build mermaid.js

```bash
$ sudo apt update
$ sudo apt install -y ca-certificates curl gnupg

# download and set up NodeSource v20 repo
$ curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash
# install Node.js
$ sudo apt install -y nodejs
# check package version
$ node -v && npm -v

$ git clone git@github.com:mermaid-js/mermaid.git
$ cd mermaid

$ sudo corepack enable pnpm

# Cypress 等測試工具
$ pnpm install

$ pnpm run build
# 主要的 lib 檔案位於 packages/mermaid/dist/
# 圖表核心和模組會封裝成 .js 或 .mjs(如 mermaid.js, mermaid.esm.min.mjs）

# 檢視修改過的程式碼效果
$ pnpm run dev
...
[00:29:55] Langium generator finished successfully in 967ms
[watch] build finished, watching for changes...
Listening on http://localhost:9005 (worktree: mermaid)
```

----
## Build mermaid Live Editor

```bash
$ curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash
# 或使用 Gitee 镜像
$ bash -c "$(curl -fsSL https://gitee.com/RubyMetric/nvm-cn/raw/main/install.sh)"

$ source ~/.bashrc
$ nvm -v
0.39.7
# live editor 需求版本 > 24.16.0
#nvm install 24.16.0
$ nvm install --lts

$ git clone git@vitaceae.github.com:mermaid-js/mermaid-live-editor.git
$ cd mermaid-live-editor
$ pnpm install
$ pnpm dev -- --open
```

----
## Problems

>   Internal Error: Error when performing the request to https://registry.npmjs.org/pnpm  
>     ...
>     errno: -3008,  
>     code: 'ENOTFOUND',  

```bash
$ env | grep -i proxy

# unset all proxy settings
$ unset http_proxy
$ unset https_proxy
...
```

>   ERR_PNPM_PATCH_NOT_APPLIED

依賴套件如 roughjs 的補丁不相容當前環境

```bash
$ pnpm install --no-frozen-lockfile
```

>   Error: Error when performing the request to https://registry.npmjs.org/pnpm/-/pnpm-10.30.3.tgz  
>   ...  
>   [cause]: Error: self-signed certificate in certificate chain  
>   ...  
>   code: 'SELF_SIGNED_CERT_IN_CHAIN'

Useless?

```bash
# change the mirror and update Corepack
$ npm config set registry https://registry.npmmirror.com
$ npm install -g corepack@latest
$ corepack prepare pnpm@latest --activate

$ npm cache clean --force
$ pnpm --version
```