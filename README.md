# Introduction
While i was looking for a way to bypass UAC (User Account Control) i came across a LOLBIN (Living of the land binary) called `cmstp.exe` that can be abused to bypass UAC and execute arbitrary commands from a malicious INF through an auto-elevated COM interface.

Running `cmstp.exe` with the following config will spwan an elevated `cmd.exe`.
```
[version]
Signature=$chicago$

[DefaultInstall]
RunPreSetupCommands=RunPreSetupCommandsSection

[RunPreSetupCommandsSection]
C:\Windows\System32\cmd.exe

[Strings]
ServiceName="EVIL"
ShortSvcName="EVIL"
```
The issue is that the user must select *All users* option then click OK in order to spawn the elevated `cmd.exe`.

![2024-10-2918-39-26-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/50dd1647-6159-4012-aa6c-c67a6d0e64b4)

# Analyzing cmstp.exe

As a starter I've attached x64dbg to `cmstp.exe` and examined the threads call stack.

![Screenshot 2024-10-27 222823](https://github.com/user-attachments/assets/12c9dd89-d183-4eed-8fd7-03ed21183e0f)

As you can see `cmstp.exe` calls `DialogBoxParamW` to display the options box, this is a very useful information because after opening `cmstp.exe` on IDA we'll just take a look where this API get referenced.

![Screenshot 2024-10-27 223934](https://github.com/user-attachments/assets/389939a1-20ea-4eea-99a6-40f96c311277)

IDA found the debugging symbols of the binary, this will speed up the analysis.

`DialogBoxParamW` is been call from two function, the one will be interesting to analyze is  `GetInstallOptions`.

i've created this graph to show the execution flow of `GetInstallOptions`.

![Untitled(5)](https://github.com/user-attachments/assets/c7189523-012b-41ee-b346-ab6bf55cf978)


if the COM instance is created, cmstp.exe will use it to spawn an elevated `cmd.exe` or the command we specify in the config.

As you can see from the graph, there is two ways to create an elevated COM interface, from displayed options box or the process is already elevated.

# Bypass

to bypass this Patch

