echo WZ verifica comm_size nao pot 2 com root=3 
echo --------------------------------------------- comm_size=8
mpirun -np 8 -oversubscribe myBroadcast 1 8 8 -r 3 2>&1 | sort | grep Send
echo --------------------------------------------- comm_size=7
mpirun -np 7 -oversubscribe myBroadcast 1 8 7 -r 3 2>&1 | sort | grep Send
echo --------------------------------------------- comm_size=6
mpirun -np 6 -oversubscribe myBroadcast 1 8 6 -r 3 2>&1 | sort | grep Send
