# RunAsTrustedInstaller

A Windows command-line utility and Explorer extension to run programs with TrustedInstaller privileges.

## Description

On Windows, TrustedInstaller privileges can sometimes be helpful for modifying or deleting criticial system files and folders that are protected by the OS.  
Likewise, there may be a need to deal with certain registry keys that are protected by an ACL that restricts access to the TrustedInstaller account.  
The utility was developed as a standalone executable and does not require any installation.

Starting with Release 0.2, a Shell Extension Handler can optionally be installed, which provides an additional **"Run as TrustedInstaller"** context menu entry for `.exe` files in Windows Explorer. Note that both `RunAsTIShellExtension.dll` and `RunAsTI.exe` need to reside in the same directory.

## Getting Started

### Compilation

```text
MSBuild RunAsTI.sln
```

Alternatively, you can simply open the `RunAsTI.sln` Solution File in Visual Studio and compile from there.

### Usage Examples

Delete a file that cannot be deleted the regular way:

```text
RunAsTI.exe cmd.exe /c del C:\Windows\system32\foobar.dat
```

Open the Registry Editor to view/modify/delete registry keys/values which are not accessible in normal operation mode:

```text
RunAsTI.exe C:\Windows\regedit.exe
```

Install Shell Extension Handler:

```text
RunAsTI.exe /install
```

Uninstall Shell Extension Handler:

```text
RunAsTI.exe /uninstall
```

## Version History

* 0.2
  * Added Shell Extension Handler support
* 0.1
  * Initial Release

## License

This project is licensed under the MIT License.
