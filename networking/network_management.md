---
title: "Network Management"
tags: [networking]
draft: false
---

# Network Management

Get gateway IP

```bash
$ grep nameserver /etc/resolv.conf | awk '{print $2}'
```
