# ViGEm IIDXIO
This application allows you to use any iidxio backend, e.g. `iidxio-ezusb.dll`, `iidxio-ezusb2.dll`
or `iidxio-bio2.dll`, to be exposed as two XBOX 360 game controller on Windows.

Thus, it allows you to use a real cabinet with all its IO with *any* game that supports xinput.

## Setup
* Install [ViGEmBus](https://github.com/ViGEm/ViGEmBus/releases)
* Place the following in the same folder as `vigem-iidxio.exe`:
  * Get a copy of [ViGEmClient.dll](https://bin.jvnv.net/file/ZgMJK/ViGEmClient.zip) (or from bemanitools-supplements)
  * Rename your corresponding `iidxio-XXX.dll`, e.g. `iidxio-bio2.dll`, to `iidxio.dll`.
* Run `vigem-iidxio.exe` so that the config file gets created (on first start)
* Edit `vigem-iidxio.conf` if needed. Available options are explained in the file

## Usage
* Run `vigem-iidxio.exe` and keep it open/running
* To quit the program, hit the TEST + SERVICE button at the same time

## Input mappings
| Cabinet    | XBOX Gamepad 1     | XBOX Gamepad 2     |
|------------|--------------------|--------------------|
| P1 Key 1   | A                  |                    |
| P1 Key 2   | B                  |                    |
| P1 Key 3   | X                  |                    |
| P1 Key 4   | Y                  |                    |
| P1 Key 5   | Left shoulder      |                    |
| P1 Key 6   | Rights houlder     |                    |
| P1 Key 7   | Dpad right         |                    |
| P2 Key 1   |                    | A                  |
| P2 Key 2   |                    | B                  |
| P2 Key 3   |                    | X                  |
| P2 Key 4   |                    | Y                  |
| P2 Key 5   |                    | Left shoulder      |
| P2 Key 6   |                    | Rights houlder     |
| P2 Key 7   |                    | Dpad right         |
| Start P1   | Start              |                    |
| Start P2   |                    | Start              |
| VEFX       | Back               |                    |
| Effect     |                    | Back               |
| Test       | Left thumbstick    |                    |
| Service    | Right thumbstick   |                    |
| Coin       |                    | Left thumbstick    |
| TT P1 Up   | Thumbstick X axis+ |                    |
| TT P1 Down | Thumbstick X axis- |                    |
| TT P2 Up   |                    | Thumbstick X axis+ |
| TT P2 Down |                    | Thumbstick X axis- |

Turntables are either in absolute or relative mode depending on how it's defined in the
`vigem-iidxio.conf`.

## Additional Notes For Cabinets (Running on embedded Windows 7):
* Make sure that you follow the instructions exactly from the release page
([Prerequisites for Windows 7](https://github.com/ViGEm/ViGEmBus/wiki/Prerequisites-for-Windows-7))
* If you get an error while trying to install KB3033929, re-enable windows update
* Make sure to ewfmgr C: -commit and reboot after installing the drivers (this only needs to be
done once)
