@echo off

cd /d %~dp0

if not exist dev\nvram mkdir dev\nvram
if not exist dev\nvram\coin.xml copy prop\coin.xml dev\nvram\coin.xml
if not exist dev\nvram\eacoin.xml copy prop\eacoin.xml dev\nvram\eacoin.xml
if not exist dev\nvram\dm_pad.xml copy prop\dm_pad.xml dev\nvram\dm_pad.xml
if not exist dev\nvram\network.xml copy prop\network.xml dev\nvram\network.xml
if not exist dev\nvram\ea3-config2.xml copy prop\eamuse-config2.xml dev\nvram\ea3-config2.xml
if not exist dev\raw mkdir dev\raw

launcher -K gdhook.dll gdxg.dll -V prop/avs-config2.xml -E prop/ea3-config2.xml --config gdhook.conf %*
