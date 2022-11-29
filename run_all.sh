BUILD_DIRECTORY=cmake-build-debug

for INSTANCE in GAP/data/**/*
do
  echo $INSTANCE
  $BUILD_DIRECTORY/GAP/idol/benchmark_idol_gap $INSTANCE
done
