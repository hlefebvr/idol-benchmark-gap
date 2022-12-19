if [ "$#" -ne 5 ] || ! [ -d "$1" ]; then
  echo "One argument required: path_to_instances"
  exit
fi

BUILD_DIRECTORY=./cmake-build-debug
INSTANCES=$1

echo "Solving using idol"
for FILE in $INSTANCES/*
do
  echo "$(date) ${FILE} ${2} ${3} ${4} ${5}"
  echo "$(date) ${FILE} ${2} ${3} ${4} ${5}" >> reports/history_idol_GAP.log
  $BUILD_DIRECTORY/GAP/idol/benchmark_idol_gap $FILE $2 $3 $4 $5 >> reports/history_idol_GAP.log 2>&1
done
