# autocuts-electron

node-gyp configure --target=1.8.4 --arch=x64 --dist-url=https://atom.io/download/atom-shell

call $(ProjectDir)..\post-build.bat $(TargetDir)$(TargetFileName) $(ProjectDir)latest-builds\

