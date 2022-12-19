if [ "$#" -ne 6 ] || ! [ -d "$1" ]; then
  echo "5 arguments required, given ${#}, i.e., ${@}."
  exit
fi

BUILD_DIRECTORY=./cmake-build-debug
INSTANCES=$1

echo "Solving using idol"
for FILE in $INSTANCES/*
do
  echo "$(date) ${FILE} ${2} ${3} ${4} ${5} ${6}"
  echo "$(date) ${FILE} ${2} ${3} ${4} ${5} ${6}" >> reports/history_idol_GAP.log
  $BUILD_DIRECTORY/GAP/idol/benchmark_idol_gap $FILE $2 $3 $4 $5 $6 >> reports/history_idol_GAP.log 2>&1
done
