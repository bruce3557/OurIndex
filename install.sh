if [ -d ./bin ]; then
  echo "bin exist"
else
  echo "NO bin directory, build it"
  mkdir ./bin
fi

cd ./index
make clean
make
cd ../query
make clean
make

