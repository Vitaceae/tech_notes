---
title: "yt-dlp"
tags: [app]
draft: false
---

# yt-dlp

下載使用 m3u8 串流的影片，並使用 ffmpeg 將 .ts 轉檔成 .mp4 格式，不限 Youtube。

## Installation

### Ubuntu 安裝指令

```bash
# 安裝 yt-dlp
$ sudo wget https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp -O /usr/local/bin/yt-dlp
$ sudo chmod a+rx /usr/local/bin/yt-dlp

# 更新 yt-dlp
$ sudo yt-dlp -U

# 安裝 ffmpeg
$ sudo apt install ffmpeg

# 檢查版本
$ yt-dlp --version && ffmpeg -version
```

### Windows 安裝指令

```powershell
> winget install -e --id yt-dlp.yt-dlp
# update yt-dlp
> winget upgrade yt-dlp
> yt-dlp --version
```

### Python 安裝指令

```bash
# download uv with curl
$ curl -LsSf https://astral.sh/uv/install.sh | sh
# download uv with wget
$ wget -qO- https://astral.sh/uv/install.sh | sh

# install uv
$ pip install uv

# install yt-dlp
$ uv tool install "yt-dlp[default]" --with ffmpeg,deno
```

## 使用方式

```bash
$ yt-dlp <param 1> <param 2> <param 3> "url_1" "url_2" "url_3"
```

### 將目標影片另存為 .mp4

檔案名稱同來源網址標題，並嵌入縮圖與影片資訊。

```bash
$ yt-dlp \
    --output "%(title)s.%(ext)s" \
    --embed-thumbnail --add-metadata \
    --merge-output-format mp4 \
    "<video_url>"
```

### 指定畫質為 1080p

使用 H.264 編碼

```bash
# height: 720(720p), 1080(1080p), 2160(4k), 
$ yt-dlp \
    --output "%(title)s.%(ext)s" \
    -f "bestvideo[height<=1080]+bestaudio[ext=m4a]" \
    -S vcodec:h264 \
    --embed-thumbnail \
    --add-metadata \
    --merge-output-format mp4 \
    "<video_url>"

# 若需包含 CC 字幕加上 --write-sub <lang-code>
# 包含自動生成的字幕檔再加上 --write-auto-subs  
# 支援的 lang-code 以下面指令查詢
$ yt-dlp --list-subs "<video_url>"

# 僅下載字幕檔
$ yt-dlp \
    --output "%(title)s.%(ext)s" \
    --sub-lang zh-TW \
    --write-sub \
    --convert-subs srt \
    --skip-download \
    "<video_url>"
```

### 指定格式為 mp3

位元率 320kbps

```bash
$ yt-dlp \
    --output "%(title)s.%(ext)s" \
    --embed-thumbnail \
    --add-metadata \
    --extract-audio \
    --audio-format mp3 \
    --audio-quality 320K \
    "<video_url>"
```

### 下載清單影片

```bash
$ yt-dlp \
    --output "%(playlist)s/%(title)s.%(ext)s" \
    --embed-thumbnail \
    --add-metadata \
    --merge-output-format mp4 \
    "<video_url>"
```

### 批次下載影片

```bash
# 建立清單
$ vi dl-list.txt

https://www.youtube.com/watch?v=xxx1
https://www.youtube.com/watch?v=xxx2
https://www.youtube.com/watch?v=xxx3
...

$ yt-dlp \
    --output "%(title)s.%(ext)s" \
    --batch-file dl-list.txt \
    --embed-thumbnail \
    --add-metadata \
    --merge-output-format mp4
```

----
## Configuration

```bash
# 填入常用參數
$ vi ~/.config/yt-dlp/config

--output "%(title)s.%(ext)s"
--merge-output-format
--embed-thumbnail
--add-metadata
```