# 只跑*.spl文件，不跑*.out文件
rm -rf test/phase1/test_result && mkdir test/phase1/test_result
rm -rf test/phase1/extra-test_result && mkdir test/phase1/extra-test_result
# test
for file in `ls test/phase1/test/`
do
    echo "test/phase1/test/$file"
    ./splc test/phase1/test/$file  > test/phase1/test_result/${file%.*}.out
done

for file in `ls test/phase1/test_result/`
do
    echo "test/phase1/test_result/$file"
    sed -i '1d' test/phase1/test_result/$file
    diff test/phase1/test_result/$file test/phase1/test_ans/$file > test/phase1/test_result/${file%.*}.diff
done

# extra-test
for file in `ls test/phase1/extra-test/`
do
    echo "test/phase1/extra-test/$file"
    ./splc test/phase1/extra-test/$file  > test/phase1/extra-test_result/${file%.*}.out
done

for file in `ls test/phase1/extra-test_result/`
do
    echo "test/phase1/extra-test_result/$file"
    sed -i '1d' test/phase1/extra-test_result/$file
    diff test/phase1/extra-test_result/$file test/phase1/extra-test_ans/$file > test/phase1/extra-test_result/${file%.*}.diff
done
