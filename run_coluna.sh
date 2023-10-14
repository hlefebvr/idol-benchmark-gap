if [ "$#" -ne 1 ] || ! [ -d "$1" ]; then
  echo "One argument required: path_to_instances"
  exit 1
fi

BUILD_DIRECTORY=./cmake-build-debug
INSTANCES=$1

echo "Solving using coluna"

for FILE in $INSTANCES/*
do
  timeout 320s julia GAP/coluna/main.jl glpk $FILE || echo "${FILE},coluna,1,0,0,500,1,TIME_LIMIT,,,,,," >> results_GAP_coluna.csv
done
