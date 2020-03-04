cd $(dirname $0)
TIME=$((60*5)) #1hr:30min
nice -n-20 ./executor-atomic --timer $TIME > atomic-1hr.txt

nice -n-20 ./thread-pool-atomic --timer $TIME > thread-pool-atomic.txt

