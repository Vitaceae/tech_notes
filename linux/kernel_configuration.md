---
title: "Linux Kernel Configuration"
tags: [linux, kernel]
draft: false
date = "2026-10-20"
---

# Linux Kernel Configuration

----
## Check Config in Runtime

```bash
KCONFIG="/proc/config.gz"
CONFIGS_TO_CHECK="
    CONFIG_DEVMEM
    CONFIG_DEVPORT
"

for cfg in $CONFIGS_TO_CHECK; do
    val="$(zcat "$KCONFIG" | grep -E "^(${cfg}=|# ${cfg} is not set)" 2>/dev/null)"
    printf "%-40s | $val\n" "$cfg"
done
```