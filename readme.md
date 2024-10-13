# Abstract

**Extracting secret data protected by an AppArmor profile by injecting a custom library function with `LD_PRELOAD`**


This is a TUM internal exploit, but I'm still publishing this writeup for educational purposes, as I consider both the security set-up and exploitation strategy interesting.

[![AppArmor](img/AppArmor.png)](https://www.apparmor.net/)
[AppArmor](https://www.apparmor.net/) is a Linux kernel security module that offers *per-program* security policies.
Programs are identified by *path*. Authorized programs can switch between different security policies at runtime.
