@echo off

cd /d %~dp0

if not exist dev\jb\nvram mkdir dev\jb\nvram
if not exist dev\jb\nvram\coin.xml copy prop\coin.xml dev\jb\nvram\coin.xml
if not exist dev\jb\nvram\eacoin.xml copy prop\eacoin.xml dev\jb\nvram\eacoin.xml
if not exist dev\jb\nvram\dm_pad.xml copy prop\dm_pad.xml dev\jb\nvram\dm_pad.xml
if not exist dev\jb\nvram\network.xml copy prop\network.xml dev\jb\nvram\network.xml
if not exist dev\jb\nvram\ea3-config.xml copy prop\eamuse-config.xml dev\jb\nvram\ea3-config.xml
if not exist dev\jb\raw mkdir dev\jb\raw
if not exist dev\tmp mkdir dev\tmp

launcher -K -V prop/avs-config.xml -E prop/ea3-config.xml gdhook.dll gdxg.dll --config gdhook.conf %*
