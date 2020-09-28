@echo off

cd /d %~dp0

if not exist dev\ja\nvram mkdir dev\ja\nvram
if not exist dev\ja\nvram\coin.xml copy prop\coin.xml dev\ja\nvram\coin.xml
if not exist dev\ja\nvram\eacoin.xml copy prop\eacoin.xml dev\ja\nvram\eacoin.xml
if not exist dev\ja\nvram\dm_pad.xml copy prop\dm_pad.xml dev\ja\nvram\dm_pad.xml
if not exist dev\ja\nvram\network.xml copy prop\network.xml dev\ja\nvram\network.xml
if not exist dev\ja\nvram\ea3-config.xml copy prop\eamuse-config2.xml dev\ja\nvram\ea3-config.xml
if not exist dev\ja\raw mkdir dev\ja\raw
if not exist dev\tmp mkdir dev\tmp

launcher -K gdhook.dll -V prop/avs-config2.xml -E prop/ea3-config2.xml gdxg.dll --config gdhook.conf %*
