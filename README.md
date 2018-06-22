# ParticleBot2

## Prerequisite

This bot requires installing Guosh, ParticleDI, ParticleINI (all are written by me), SQLite3 and CURL.

Additionally you can install libxml++ 2.6 (required to build titler) and/or jsoncpp (required to build urban dictionary plugin).

## Building

```bash
$ git clone https://github.com/handicraftsman/particlebot2
$ cd particlebot2
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/ # in case of using Ninja instead of Make
$ make -j4 # where 4 is the number of CPU cores you have
# Optional:
# sudo make install
```

## Configuring

Bot config is written in a simple .ini file. Format can be seen in /particlebot2.sample.ini

## Starting

```bash
$ /path/to/particlebot2 -l loglevel -c /path/to/config.ini
# Show help message:
$ /path/to/particlebot2 --help
```
