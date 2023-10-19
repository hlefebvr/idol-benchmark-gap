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
  timeout 300s $BUILD_DIRECTORY/GAP/idol/benchmark_idol_gap $FILE $@ || echo "${FILE},coluna,1,0,0,500,1,TIME_LIMIT,,,,," >> "results_GAP_coluna.csv"
done
