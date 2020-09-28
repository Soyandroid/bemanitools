# Game list

The following games are compatible with this version of gdhook:
* GITADORA GuitarFreaks & DrumMania
* GITADORA OverDrive
* GITADORA Tri-Boost
* GITADORA Tri-Boost Re:EVOLVE
* GITADORA Matixx
* GITADORA EXCHAIN

# Data setup and running the game

We assume that you are using a clean/vanilla data dump. Ensure your ("contents")
folder with your unpacked data looks like this:
- data
- modules
- prop

* Copy/Move all files from the *modules* directory to the root folder, so they
are located next to the *data* and *prop* folders.
* Create a new file *app-config.xml* in the *prop* folder with the following
content:
```
<?xml version="1.0"?>
<param></param>
```
* Copy *avs-config-jb.xml* to *avs-config.xml* and *avs-config-ja.xml* to
*avs-config2.xml* in *prop* folder.
* Copy *eamuse-config.xml* to *ea3-config.xml* and *eamuse-config2.xml* to
*ea3-config2.xml* in *prop* folder.
* Setup proper paths for *dev/nvram* and *dev/raw* in *prop/avs-config.xml* 
and *prop/avs-config2.xml* by replacing the *<fs>*-block in that file
with the following block, where "XX" should be "ja" for *avs-config2.xml*
or "jb" for *avs-config.xml*:
```
  <fs>
    <root>
      <device __type="str">..\contents</device>
    </root>
    <nvram>
      <device __type="str">dev/XX/nvram</device>
      <fstype __type="str">fs</fstype>
      <option __type="str">posix=1</option>
    </nvram>
    <raw>
      <device __type="str">dev/XX/raw</device>
    </raw>
    <nr_mountpoint __type="u16">128</nr_mountpoint>
    <nr_mounttable __type="u16">128</nr_mounttable>
    <nr_filedesc __type="u16">128</nr_filedesc>
  </fs>
```
* Add proper *<id>* and *<soft>* block as below in *prop/ea3-config.xml* 
and *prop/ea3-config2.xml* if they're not exists, change */ea3/soft/spec*
to "A" for *ea3-config2.xml* or "B" for *ea3-config.xml*:
```
  <id>
     <pcbid  __type="str">00010203040506070809</pcbid>
     <softid __type="str">00010203040506070809</softid>
  </id>
  <soft>
    <model __type="str">M32</model>
    <dest  __type="str">J</dest>
    <spec  __type="str">A</spec>
    <rev   __type="str">A</rev>
    <ext   __type="str">2015032701</ext>
  </soft>
```
* Unpack the package containing gdhook into the root folder so gdhook.dll
and all other files are located in the same folder as *data*, *prop*, 
*gdxg.dll*, etc.
* Run the gamestart-XX.bat file as admin. Where XX matches the game you 
want to run.

# Eamuse network setup

* Open the prop/ea3-config.xml or prop/ea3-config2.xml
* Replace the *ea3/network/services* URL with network service URL of your
choice (for example http://my.eamuse.com)
* Edit the *ea3/id/pcbid*

# Real hardware support

### GuitarFreaks GUITAR UNIT

Remove the "#" before *emu.disable_guitar1* or *emu.disable_guitar2*
and set value of the one you need to disable to true.
The game expect GUITAR UNIT 1 on COM2 and GUITAR UNIT 2 on COM3.

### DrumMania DRUM UNIT

Remove the "#" before *emu.disable_drum* and set value to true.
The game expect DRUM UNIT on COM2.

### Slotted/Wave pass card readers

Remove the "#" before *emu.disable_eamio* and set value to true.
Notice the game expect card readers on COM0 of P4IO, but gdhook p4io
emulation re-assigned that to COM4, so if you want to use a real card reader
you should set them to COM4 in device manager.
For GuitarFreaks, XG or SD mode expect 2 readers on the same acio bus,
while GuitarFreaks GD mode and DrumMania only need 1 reader.

### Disable all I/O emulation

Run the launcher without the hook dll: *launcher gdxg.dll*

# Troubleshooting and FAQ

## Game crashes with "W:prop: read error '/dev/nvram/eacoin.xml'"

* Manually copy *prop/eacoin.xml* into nvram folder (eg. *dev/ja/nvram* for GF),
open the copied file with text editor, then delete *<tax>* section and save it.