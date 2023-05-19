# RunAsTrustedInstaller
A Windows command-line utility to run programs with TrustedInstaller privileges.

## Description
On Windows, TrustedInstaller privileges sometimes can be helpful to modify/delete criticial system files/folders that are protected by the OS.  
Likewise, there might be a need to deal with certain Registry keys that are protected with an ACL which restricts access to the TrustedInstaller account.  
The utility was developed as a standalone executable and does not require any installation.

## Getting Started

### Compilation
```
cmake .
cmake --build .
```
Alternatively, you can simply open `CMakeLists.txt` in Visual Studio and compile from there.

### Usage Examples

Delete a file that cannot be deleted the regular way:
```
RunAsTI.exe cmd.exe /c del C:\Windows\system32\foobar.dat
```

Open the Registry Editor to view/modify/delete Registry keys/values that are not accessible in normal operation mode:
```
RunAsTI.exe C:\Windows\regedit.exe
```

## Version History

* 0.1
    * Initial Release

## License
This project is licensed under the MIT License.