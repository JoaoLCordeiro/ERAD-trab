echo raiz é $1
mpirun -np 8 -oversubscribe myBroadcast 1 800 8 -r $1
