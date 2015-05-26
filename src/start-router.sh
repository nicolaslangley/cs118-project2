# Script for initializing and starting routers in random order

echo "Running setup script..."
echo "Building sources..."
make
echo "Starting two simple test routers..."
./router-exec -r &
./router-exec -s &

