rm -rf test/phase2/test_result_v && mkdir test/phase2/test_result_v
rm -rf test/phase2/test_result && mkdir test/phase2/test_result
# VERBOSE(DEBUG)
# for file in `ls test/phase2/test/`
# do
#     echo "test/phase2/test/$file"
#     ./bin/splc test/phase2/test/$file  > test/phase2/test_result_v/${file%.*}.out
# done

# NO VERBOSE
for file in `ls test/phase2/test/`
do
    echo "test/phase2/test/$file"
    ./bin/splc test/phase2/test/$file  > test/phase2/test_result/${file%.*}.out
done
# for file in `ls test/phase2/test_result/`
# do
#     echo "test/phase2/test_result/$file"
#     sed -i '1d' test/phase2/test_result/$file
#     diff test/phase2/test_result/$file test/phase2/test_ans/$file > test/phase2/test_result/${file%.*}.diff
# done