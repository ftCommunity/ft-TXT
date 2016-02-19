FT-TXT
======

This is the open source part of the [TXT Controller Open Source Archive](http://www.fischertechnik.de/ResourceImage.aspx?raid=8400).

Currently missing is the whole ```board/knobloch/TXT/rootfs``` tree from that archive due to unknown licensing.

Build everything
----------------

Download the [TXT Controller Open Source Archive](http://www.fischertechnik.de/ResourceImage.aspx?raid=8400) and copy its ```board/knobloch/TXT/rootfs``` tree into the matching part of your branch of this repository.

Then type
```make```

This will take an hour or two to download and build the linux
components for the root file system.



See the [Wiki](https://github.com/ftCommunity/ft-TXT/wiki) for details
and how to [setup your TXT to boot from SD card](https://github.com/ftCommunity/ft-TXT/wiki/BootFromSD).
