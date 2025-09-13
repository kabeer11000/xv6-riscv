
## XV6 `Shell` implementation



## Process creation

Process creation varies widely between Window's Server / API driven architecture vs Linux and XV6's system call interface. 

XV6, and Linux follow a common architecture pattern where a process is cloned / forked and then a system call for execution is called. While the actual call's and implementations defer the essence remains the same between them. 

*(Research not verified)*
On Linux e.g. on a GUI system with GNOME interface when a user clicks the icon of a program e.g. Chrome, a command sequence executes the command is first passed to GNOME shell running on top of Wayland, icon click will be resolved into a command like `gio launch google-chrome.desktop` where `.desktop` is application launchers file extension for GNOME to read.

> `.desktop` file describes to GNOME how to run the program. It may e.g. point to /usr/bin/google-chrome-stable.

Then GNOME shell calls `clone()` system call followed by `execve("/usr/bin/google-chrome-stable", …)` to replace the child's memory with Chrome's executable. 

Process is theoretically same for XV6, when a user tries to launch a program, XV6's shell creates a copy of itself using the `fork()` system call and then uses `exec()` call to replace current process's memory with the program. The process of creating new processes may seem similar but underlying complexity is very high in Linux compared to XV6.


Windows on the other hand deals with process creation completely differently. Windows API exposes `CreateProcess` in `windows.h` and a underlying API function: 

```c#
BOOL CreateProcessW(
    LPCWSTR lpApplicationName,  // const wchar_t*
    LPWSTR  lpCommandLine,      // wchar_t*
    ...
);
```

This "Creates a new process and its primary thread. The new process runs in the security context of the calling process." - Window's API documentation

This approach seems simpler where no separate `fork` + `exec` calls happen, `CreateProcess()` bundles process creation and program loading into one function call. 

After this the Kernal creates a new process object, virtual address space, loads Windows DLL's and more before loading `chrome.exe`'s source into memory PE format.

Chrome's execution would start from Windows runtime code and eventually get to `main()`.

There is a fundamental contrast in both OS's philosophy, where there's no concept of a process being a child of another process at launch, instead a `new` process is created from thin air, and allocated resources and run. While Unix approach seems complicated, it allows these systems to have goodies like daemons while also having copy-on-write like patterns. 

Daemon's can be easily created using double `fork()` calls from the Shell / Terminal, the process can now run in the background silent and isolated.


