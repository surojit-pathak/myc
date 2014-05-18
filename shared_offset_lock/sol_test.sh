function test_start()
{
    testi=`expr $testi + 1`
}

function chk_sanity()
{
    if [ $? -ne 0 ]
    then
        echo -e "\n\nError!!! in test#$testi"
        echo -e "\nnthr - $nthr, start - $start, end - $end, count - $count, ratio - $ratio\n"
        exit 1
    fi
}

testi=0

# run for basic sanity
test_start
./libsol/test_sol -t 2 -s 100 -e 200 -c 500
chk_sanity
echo -e "\nFinished nthr - $nthr, start - $start, end - $end, count - $count, ratio - $ratio\n" 

start=109
count=1000

nthr=2
THRMAX=16 # Maximum Number of threads
while [ $nthr -le $THRMAX ]
do
    end=2103; ENDMAX=21783
    while [ $end -le $ENDMAX ]
    do
            ratio=4; MAX_RATIO=10
            while [ $ratio -le $MAX_RATIO ]
            do
               ./libsol/test_sol -t $nthr  -s $start -e $end -c $count -r $ratio 
               chk_sanity
               echo -e "\nFinished nthr - $nthr, start - $start, end - $end, count - $count, ratio - $ratio\n" 
            ratio=`expr $ratio + 2`
            done
    end=`expr $end + 1000`
    done
nthr=`expr $nthr + 2`
done

