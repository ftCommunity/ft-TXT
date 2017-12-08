FT-TXT
======

This is the open source part of the [TXT Controller Open Source Archive](https://www.fischertechnik.de/-/media/fischertechnik/fite/service/downloads/robotics/txt-controller/documents/07-txt-open-source-archiv.ashx).

Currently missing is the whole ```board/knobloch/TXT/rootfs``` tree from that archive due to unknown licensing.

Build everything
----------------

Download the [TXT Controller Open Source Archive](https://www.fischertechnik.de/-/media/fischertechnik/fite/service/downloads/robotics/txt-controller/documents/07-txt-open-source-archiv.ashx) and copy its ```board/knobloch/TXT/rootfs``` tree into the matching part of your branch of this repository.

Then type
```
make
```

This will take an hour or two to download and build the linux
components for the root file system. By default a setup similar to the
one that your TXT shipped with will be built.

Once this finished run the ```sudo ./MakeSD.sh``` shell script to prepare
an SD card. By default this is using ```/dev/sdd```. Please adjust this
to match your SD card. Otherwise you might format your hard drive.

Finally use ```sudo CopyToSD.sh``` to populate the SD card with the
previously generate file system. Again check the device file.

To prepare your TXT to be able to boot from SD card please read in the
wiki [setup your TXT to boot from SD card](https://github.com/ftCommunity/ft-TXT/wiki/BootFromSD).
