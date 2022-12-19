if [ $# -lt 2 ]; then
    echo "Usage: Expected arguments: path_to_instances method [with_heuristics] [smoothing_factor] [farkas_pricing] [branching_on_master]"
    exit 1
fi

BUILD_DIRECTORY=./cmake-build-debug
INSTANCES=$1
shift

echo "Solving using idol"
for FILE in $INSTANCES/*
do
  echo "$(date) ${FILE} ${@}"
  echo "$(date) ${FILE} ${@}" >> reports/history_idol_GAP.log
  echo $@
  $BUILD_DIRECTORY/GAP/idol/benchmark_idol_gap $FILE $@ #>> reports/history_idol_GAP.log 2>&1
done
