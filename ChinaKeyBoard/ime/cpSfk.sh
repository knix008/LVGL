#!/bin/bash
make
cp  .libs/libsoftkeywin.so /usr/local/minigui30-procs/lib
cp  .libs/libsoftkeywin.so /usr/local/minigui3012-arm-procs/lib
cd .libs/
tar -cf sfk.tar libsoftkeywin.so
cp sfk.tar /mnt/hgfs/shareWorkspace/tftpboot/
cd ..
