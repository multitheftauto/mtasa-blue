These binaries were packed with UPX (http://upx.sourceforge.net/) using the following settings:

    upx -4 awesomium.dll
    upx -4 icudt.dll

This gives a good tradeoff between fast decompression and ~50% reduction in size on disk. Just
beware that using these packed binaries may add a very negligible delay each time that
awesomium_process.exe is launched (usually launched each time a WebView is created).