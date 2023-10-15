if [ "$#" -ne 2 ] || ! [ -d "$1" ]; then
  echo "Argument required: path_to_instances UUID"
  exit 1
fi

BUILD_DIRECTORY=./cmake-build-debug
INSTANCES=$1
UUID=$2

echo "Solving using coluna"

for FILE in $INSTANCES/*
do
  timeout 300s julia GAP/coluna/main.jl glpk $FILE || echo "${FILE},coluna,1,0,0,500,1,TIME_LIMIT,,,,,," >> "results_GAP_coluna__${UUID}.csv"
done
