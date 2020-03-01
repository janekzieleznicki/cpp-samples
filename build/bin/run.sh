cd $(dirname $0)
TIME=$((90*60)) #1hr:30min
nice -n-20 ./executor-atomic --timer $TIME > atomic-1hr.txt

nice -n-20 ./executor-mutex --timer $TIME > mutex-1hr.txt
