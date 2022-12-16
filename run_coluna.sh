if [ "$#" -ne 1 ] || ! [ -d "$1" ]; then
  echo "One argument required: path_to_instances"
  exit
fi

BUILD_DIRECTORY=./cmake-build-debug
INSTANCES=$1

echo "Solving using coluna"
julia GAP/coluna/main.jl glpk $INSTANCES
