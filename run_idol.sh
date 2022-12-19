BUILD_DIRECTORY=./cmake-build-debug
INSTANCES=$1
shift

echo "Solving using idol"
for FILE in $INSTANCES/*
do
  echo "$(date) ${FILE} ${@}"
  echo "$(date) ${FILE} ${@}" >> reports/history_idol_GAP.log
  $BUILD_DIRECTORY/GAP/idol/benchmark_idol_gap $FILE $@ >> reports/history_idol_GAP.log 2>&1
done
