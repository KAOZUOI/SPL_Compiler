for file in `ls test/phase2/test/`
do
    rm test/phase2/test/${file%.*}.out
done