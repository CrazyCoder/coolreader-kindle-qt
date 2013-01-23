Cool Reader 3 port for Kindle Paperwhite, Touch, 4NT, 3, DX
=========================================
This project is a port of Cool Reader application (by **Vadim Lopatin** AKA **Buggins**) to Kindle.
It supports epub (non-DRM), fb2, doc, txt, rtf, html, chm, tcr, pdb, prc, mobi (non-DRM), pml formats.

The port was originally created by **Andy Wooden**, **fau** and **VladKi**, the project had no fixed home and its
sources were distributed as tarballs, making it hard to maintain and provide updates.
This site is the **new project home**.

The fronentd is Qt based (QWS) and has custom display and keyboard drivers. Up until October 2012 there was no good
working graphics driver available for Kindle 4 (2011 and 2012 models), this caused a problem with poor eink contrast
display. Finally, **Andy Wooden** has found some time to make it work properly on Kindle 4, he contacted me with the
updated driver source that I compiled and released with the new Cool Reader distribution on various forums.

Another goal of this project is to make it easier to install on new Kindle devices. As K4 has no keyboard, there is no
hack like launchpad for it. However, it's possible to run **Kindlets** (these are small apps written in Java, almost
like the old Java Applets). [cr3runner](http://code.google.com/p/cr3runner/) by **Victor Pyankov** provides an easy way
to run custom applications and scripts on Kindle 4 devices. Kindlets are signed with developer keys that need to be
available on your device, it makes the installation process pretty hard (2 system files need to be replaced manually).

[KindleTool](https://github.com/NiLuJe/KindleTool) project allows to package custom files and scripts to be run on
jailbroken devices from the standard Kindle Update menu. Some users were providing **cr3runner** installer, but it
was old, crashed often and didn't install on Kindle 4 2012 (black) model. I changed it a bit to provide better user
experience (no menu when there is only one command, **Cool Reader** name in the documents list, updated installer that runs
on all K4 models).

So, here you have the latest **Qt drivers providing perfect contrast** on K4, most recent **Cool Reader 3 Qt port** for
Kindle based on the git **crengine** sources, updated **cr3runner** to easily run it on your device and packaged into
Kindle update with the help of **KindleTool** for easier installation. No need to research and google, everything is in
one place!

Feel free to fork and send me pull requests/patches.

Kindle Paperwhite and Touch Support
=========================================
-   [Installation instructions](http://www.mobileread.com/forums/showpost.php?p=2278589&postcount=147) (eng)
-   [Installation instructions](http://www.the-ebook.org/forum/viewtopic.php?p=973382#973382) (rus)


Quick Installation Guide for K3/K4NT
=========================================
-   [Russian guide](http://kindle.copiny.com/question/details/id/66374)

1.  [Jailbreak](http://wiki.mobileread.com/wiki/Kindle4NTHacking) your Kindle
2.  Copy **update_cr3runner_n.n_install-k4.bin** from
    [cr3runner.zip](https://github.com/downloads/CrazyCoder/coolreader-kindle-qt/cr3runner.zip) to Kindle flash root
3.  Copy all directories from **cr3-kindle-nnnn-nn-nn.zip** available in
    [Downloads](https://github.com/CrazyCoder/coolreader-kindle-qt/downloads) to Kindle flash root
4.  (optional, but recommended) Download the latest test version as the 
    [repository snapshot zip](https://github.com/CrazyCoder/coolreader-kindle-qt/zipball/master) and copy all the files
    from the **dist** folder replacing all existing file on the connected Kindle (this way you update to the most recent
    version with all the latest changes)
5.  From the Kindle **Settings** dialog open menu, run **Update Your Kindle**
6.  After reboot you should have **Cool Reader** in the documents list, open it.

Kindle 3/DX owners can also use [launchpad](http://www.mobileread.com/forums/showthread.php?t=97636).

Links
=========================================
-   Russian discussion thread on [the-ebook.org forum](http://www.the-ebook.org/forum/viewtopic.php?t=20811)
-   English discussion thread on [mobileread.com forum](http://www.mobileread.com/forums/showthread.php?t=148850)
