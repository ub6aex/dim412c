# DMX512 dimmer (up to 12 channels)

## Build for 4-channel mode
```
make clean flash CH_NUM=4
```
or just:
```
make clean flash
```
There are outputs (PA5, PA6, PA7, PB1) for the first 4 channels, used for LED indicators.

## Build for any number of channels (up to 12)
```
make clean flash CH_NUM=12