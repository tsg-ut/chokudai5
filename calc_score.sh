g++ -I/mnt/c/Users/denjj/Documents/GitHub/ac-library --std=c++17 -O2 chokudai5.cpp -o chokudai5

sum=0
for f in in/*; do
  echo "Running $f"
  ./chokudai5 < $f > out/$(basename $f)
  echo "Evaluating $f"
  output=$(./checker/output_checker $f out/$(basename $f) out/$(basename $f))
  echo $output
  score=$(echo $output | sed 's/[^0-9]*\([0-9]\+\)[^0-9]*/\1/')
  sum=$((sum+score))
done
echo "Total Score: $sum"
