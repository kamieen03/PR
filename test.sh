#!/usr/bin/env bash
hosp=0
miecz=0
karabin=0
alive=$1
mpirun -np $1 a.out > temp.txt

while read  line; do
    [ "$line" == "MIECZ+" ] && ((miecz++)) && echo "MIECZ: $miecz"
    [ "$line" == "MIECZ-" ] && ((miecz--)) && echo "MIECZ: $miecz"
    [ "$line" == "KARABIN+" ] && ((karabin++)) && echo "KARABIN: $karabin"
    [ "$line" == "KARABIN-" ] && ((karabin--)) && echo "KARABIN: $karabin"
    [ "$line" == "HOSPITALIZED" ] && ((hosp++)) && echo "HOSPITALIZED: $hosp"
    [ "$line" == "UNHOSPITALIZED" ] && ((hosp--)) && echo "HOSPITALIZED: $hosp"
    [ "$line" == "DIE" ] && ((alive--)) && echo "Alive: $alive"
    [[ $hosp > 4 ]] || [[ $miecz > 2 ]] || [[ $karabin > 2 ]] && echo -e "\033[1;31mERROR"
done < temp.txt

