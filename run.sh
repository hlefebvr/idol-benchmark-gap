BUILD_DIRECTORY=cmake-build-debug
INSTANCES=GAP/data/dummies

for FILE in $INSTANCES/*
do
  echo "$(date) ${FILE}" >> history.log
  $BUILD_DIRECTORY/GAP/idol/benchmark_idol_gap $FILE >> history.log 2>&1
done
