if [ "$#" -ne 1 ] || ! [ -d "$1" ]; then
  echo "Argument required: path_to_instances"
  exit 1
fi

BUILD_DIRECTORY=./cmake-build-debug
INSTANCES=$1

echo "Solving using coluna"

for FILE in $INSTANCES/*
do
  timeout 300s julia GAP/coluna/main.jl highs $FILE || echo "${FILE},Coluna.jl,1,0,0,500,1,TIME_LIMIT,," >> "results_GAP_coluna.csv"
done
