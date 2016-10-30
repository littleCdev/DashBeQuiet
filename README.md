# BeQuietDash!

BeQuietDash tries to catch the bootp/dhcp packet of an configured (it's better to not have an product selected) DASH-Button and sending it a configuration so it can't connect home to amazon.
The DNS and Gateway will be set to its own IP.

Keep in mind that the Button can't connect to the internet. The timeout seems to be around 40 seconds before you can use the button again!

### Compiling
```sh
$ gcc main.c -Wall
```
or if you want to see the raw bootp packages with debug
```sh
$ gcc main.c -Wall -DDEBUG
```


### requirements
Nothing but gcc

### using
```sh
$ sudo ./a.out
```

### Output
Without debug
```sh
$ sudo ./a.out 
2016-10-30 20:28:34
yay this is a dash
2016-10-30 20:29:09
yay this is a dash
2016-10-30 20:29:49
yay this is a dash
```


With debug

```sh
$ sudo ./a.out 
main.c:216 got:         261 bytes
2016-10-30 20:26:29
yay this is a dash
main.c:253 Requested IP: 10.0.1.153

BOOTP Packet
   |-Operation Code:    1
   |-Hardware Type:     1
   |-Hardware Addr Length:      6
   |-Hops               0
   |-Transaction ID:    11114a96
   |-Seconds:           0s
   |-Flags:             0
   |-Client IP Address: 0 0 0 0 
   |-Server IP Address: 0 0 0 0 
   |-Your IP Address:   0 0 0 0 
   |-Gateway IP Address:        0 0 0 0 
   |-Client Hardware Address:
                   44 65 0D E0 92 18 00 00 00 00 00 00 00 00 00 00 
   |-Vendor:
                   63 82 53 63 35 01 03 36      099 130 083 099 053 001 003 054         c.Sc5..6
                   04 0A 00 00 14 32 04 0A      004 010 000 000 020 050 004 010         .....2..
                   00 01 99 37 03 01 03 06      000 001 153 055 003 001 003 006         ...7....
                   FF 7F 00 00 8E 15 27 29      255 127 000 000 142 021 039 041         .....')
                   84 7F 00 00 00 00 00 00      132 127 000 000 000 000 000 000         .......
                   00 00 00 00 E0 E2 A5 F5      000 000 000 000 224 226 165 245         ........
                   FE 7F 00 00 28 BD EB 28      254 127 000 000 040 189 235 040         ...(..(
                   84 7F 00 00 A8 81 EC 28      132 127 000 000 168 129 236 040         ......(

BOOTP Packet
   |-Operation Code:    2
   |-Hardware Type:     1
   |-Hardware Addr Length:      6
   |-Hops               0
   |-Transaction ID:    11114a96
   |-Seconds:           0s
   |-Flags:             0
   |-Client IP Address: 0 0 0 0 
   |-Server IP Address: 0 0 0 0 
   |-Your IP Address:   10 0 1 -103 
   |-Gateway IP Address:        0 0 0 0 
   |-Client Hardware Address:
                   44 65 0D E0 92 18 00 00 00 00 00 00 00 00 00 00 
   |-Vendor:
                   63 82 53 63 35 01 05 36      099 130 083 099 053 001 005 054         c.Sc5..6
                   04 0A 00 00 14 33 04 00      004 010 000 000 020 051 004 000         .....3..
                   58 02 00 00 01 04 FF 00      088 002 000 000 001 004 255 000         X.......
                   00 00 03 04 0A 00 01 99      000 000 003 004 010 000 001 153         ........
                   06 04 0A 00 01 99 FF 00      006 004 010 000 001 153 255 000         ........
                   00 00 00 00 00 00 00 00      000 000 000 000 000 000 000 000         ........
                   00 00 00 00 00 00 00 00      000 000 000 000 000 000 000 000         ........
                   00 00 00 00 00 00 00 00      000 000 000 000 000 000 000 000         ........


```
