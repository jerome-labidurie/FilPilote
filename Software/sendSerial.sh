#/bin/bash
# send serial command to arduino for FilPilote/Chacon
# ie: DomotiqueSerial.ino
# See https://github.com/jerome-labidurie/FilPilote

#TODO:
# reponse

# def values
DEV=/dev/ttyUSB0
REPEAT=1

function usage
{
   echo "usage: $0  [-h] [-d <device>] [-r <repeat>] <cmd> <args>" >&2
   echo "   <device> : tty to use (def: $DEV)" >&2
   echo "   <repeat> : nb of send repeats (def: $REPEAT)" >&2
   echo "eg: " >&2
   echo "   $0 -d /dev/ttyUSB1 -r 3 N 1 1 # switch on device 1 1" >&2
   echo "   $0 -d /dev/ttyUSB1 A 0        # Stop heater 0" >&2
}

while getopts ":d:r:h" opt
do
  case $opt in
    d) # device
        DEV=$OPTARG
        ;;
    r) # retry
        REPEAT=$OPTARG
        ;;
    h)
        usage
        exit 1
        ;;
    \?)
        echo "Option $OPTARG unknown" >&2
        usage
        exit 1
        ;;
    :)
        echo "Option -$OPTARG requires an argument." >&2
        usage
        exit 1
        ;;
    esac
done

# remove parameters used by getopts
shift $(( $OPTIND - 1 ))

# check for command
if [ $# -lt 2 ]
then
    usage
    exit 1
fi

# save actual port config
# stty -F "$DEV" -g

# serial port: 9600 8N1
stty -F "$DEV" 9600 -parity cs8 -cstopb
# stty -F $DEV cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts

# connect file 3 to serial device
exec 3<>"$DEV"

# send command a few times
for c in $(seq $REPEAT)
do
   echo "Sending $* ..."
   echo "$*" >&3
   sleep 1
done

# restore file 3
exec 3>&-
