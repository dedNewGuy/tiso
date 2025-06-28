# tiso - terminal prayer time for muslim

## Build

Requirements
- clang or others if you know how to modify the build.sh (you probably do)

```bash
./build.sh
```

## Screenshot

![image](https://github.com/user-attachments/assets/8b4c4b0e-4693-4f3d-bb2e-b452161f6f06)


## Usage

```
Usage: tiso [OPTIONS]

put config file in ~/.config/tiso/config.txt

The config file should strictly follow this format

fajr=5:45
zuhur=13:17
asr=17:40
maghrib=19:23
isha=20:48

running *tiso* without any option will display time left before the next prayer
time 

OPTIONS:
-t <h:m:s>  Set timer to <h:m:s> and in descending
```

## Contribution

You can contribute if you want. I haven't modularize and abstract away a lot of things in the code so keep that in mind.
