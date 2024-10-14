# Abstract

**Extracting secret data protected by an AppArmor profile by injecting a custom library function with `LD_PRELOAD`.**


This is a TUM internal exploit, but I'm still publishing this writeup for educational purposes, as I consider both the security set-up and the exploitation strategy interesting.

<img src="img/AppArmor.png" alt="AppArmor" width="200"/>

[AppArmor](https://www.apparmor.net/) is a Linux kernel security module that offers *per-program* security policies.
Programs are identified by **path**. Authorized programs can switch between different security policies at runtime.

## Background
The set-up can roughly be explained as follows:

The Students have to write a highly optimized C++ program that can parse multiple gigabyte of data in just a few milliseconds (by using SIMD instructions).
They push their source code to a GitLab instance, which then builds and tests it against a hidden dataset, so that the students can not cheat by hardcoding the answer.
The only piece of information they get back is whether their submission was a success or failure.
If their submission is fast enough, they get a grade bonus for the exam.

If one were able to leak the test dataset, one could either hardcode an answer, or optimize ones submission against the test dataset, claiming pole position on the leaderboard.

As the students are able to submit arbitrary code, a special solution was devised to keep them from leaking the hidden dataset:
A special, *trusted* tester program executes the student program and disables all input,  output, file system access and network access while it is active.
It then compares the result to the expected result and logs the time the student program took onto the leaderboard.
