# Raspberry Pico W WLAN connection example

WLAN connection test using two boards:
- first board runs as WLAN access point, receives and echoes back UDP packet
- second board connects, repeatedly sends UDP packet and lights LED if echoed back in time

# Status
Proof-of-concept, under construction but functional

# Compile
- use Raspberry Pi "default" installation (SDK in /home/pi/pico/pico-sdk)
- alternatively, edit CMakeLists.txt which sets the SDK location against recommendations, a commonly found convenience hack
- from the top level folder: cmake -S . -B build; cmake --build build
- copy build/node1_ap.uf2 to the first board
- copy build/node2_ap.uf2 to the second board

# run
- power up the node1 "ap" board. It blinks at 0.5 Hz
- power up the node2 board
- on successful execution, the node 2 board LED lights up as long as packets are successfully echoed back
- other blink codes e.g. 2x on the node 2 board indicate failure (e.g. AP not detected), see source
- the node2 board may be power cycled and connect repeatedly, but the node 1 board blink state will not revert to its original slow speed
- after several seconds with broken link, the node 2 end connection will time out, indicated by 7 blinks.

# Note
For unknown reasons, regular "station" mode does not work using the "poll" WLAN driver architecture (the AP works in either poll or background mode, not on git)