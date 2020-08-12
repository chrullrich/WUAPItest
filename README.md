# README

This solution contains three projects:

* WUAPItest tests the Windows Update API (C++, early binding to the WUAPI COM objects)
* WUAPItestTLB tests the Windows Update API (C++, late binding to the WUAPI COM objects)
* WUAPItestCs determines applicable updates and installs all updates that do not require user interaction during installation


## WUAPItestCs

The program is designed to be run as part of a [PDQ Deploy](https://www.pdq.com/) package to install all required updates
on a target system. It works outside of PDQ Deploy as well.

If the following conditions are met, it will create a file at `C:\Windows\reboot.required`:

* The environment variable `PDQSERVER` is set.
* The value of this environment variable does not match the local host name. (If the value contains dots, only the part
  before the first dot is considered.)

### Exit codes

* 0: No applicable updates, or all non-interactive updates installed and no reboot required
* 1: There are applicable updates, but all of them require interaction
* 1603: Installation error
* 3010: All non-interactive updates installed, reboot required

### But why?

“But why? There is plenty of VBScript (or PowerShell) out there that runs entirely without a C# compiler! Just check Google!”

Well ... have you tried any of it on Windows 10 2004 recently?