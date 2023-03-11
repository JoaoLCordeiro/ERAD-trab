make -B 2> /dev/null

#para re-fazer os experimentos, excluímos os arquivos antigos
#caso não queira excluir, guarde os arquivos antigos em outro diretório
rm resultados-prof.csv resultados-mpi.csv 2> /dev/null

#numero de elementos
NMSG=20000

# para bloqueantes
for ARGTAM in 8 1024 4096 16384
do
	echo "$NMSG mensagens de tamanho $ARGTAM com 8 processos" >> resultados-prof.csv
	echo "$NMSG mensagens de tamanho $ARGTAM com 8 processos" >> resultados-mpi.csv
	for vez in $(seq 1 20)
	do
		mpirun -np 8 --hostfile hostfile8.txt ./meuBcast "$NMSG" "$ARGTAM" -r 0 | grep 'Tempo:' | awk '{printf "%s\n",$2}' >> resultados-prof.csv
		mpirun -np 8 --hostfile hostfile8.txt ./Bcast "$NMSG" "$ARGTAM" -r 0 | grep 'Tempo:' | awk '{printf "%s\n",$2}' >> resultados-mpi.csv
	done

	echo "$NMSG mensagens de tamanho $ARGTAM com 16 processos" >> resultados-prof.csv
	echo "$NMSG mensagens de tamanho $ARGTAM com 16 processos" >> resultados-mpi.csv
	for vez in $(seq 1 20)
	do
		mpirun -np 16 --hostfile hostfile16.txt ./meuBcast "$NMSG" "$ARGTAM" -r 0 | grep 'Tempo:' | awk '{printf "%s\n",$2}' >> resultados-prof.csv
		mpirun -np 16 --hostfile hostfile16.txt ./Bcast "$NMSG" "$ARGTAM" -r 0 | grep 'Tempo:' | awk '{printf "%s\n",$2}' >> resultados-mpi.csv
	done

	echo "$NMSG mensagens de tamanho $ARGTAM com 32 processos" >> resultados-prof.csv
	echo "$NMSG mensagens de tamanho $ARGTAM com 32 processos" >> resultados-mpi.csv
	for vez in $(seq 1 20)
	do
		mpirun -np 32 --hostfile hostfile32.txt ./meuBcast "$NMSG" "$ARGTAM" -r 0 | grep 'Tempo:' | awk '{printf "%s\n",$2}' >> resultados-prof.csv
		mpirun -np 32 --hostfile hostfile32.txt ./Bcast "$NMSG" "$ARGTAM" -r 0 | grep 'Tempo:' | awk '{printf "%s\n",$2}' >> resultados-mpi.csv
	done																																	

	echo "$NMSG mensagens de tamanho $ARGTAM com 64 processos" >> resultados-prof.csv
	echo "$NMSG mensagens de tamanho $ARGTAM com 64 processos" >> resultados-mpi.csv
	for vez in $(seq 1 20)
	do
		mpirun -np 64 -oversubscribe --hostfile hostfile64.txt ./meuBcast "$NMSG" "$ARGTAM" -r 0 | grep 'Tempo:' | awk '{printf "%s\n",$2}' >> resultados-prof.csv
		mpirun -np 64 -oversubscribe --hostfile hostfile64.txt ./Bcast "$NMSG" "$ARGTAM" -r 0 | grep 'Tempo:' | awk '{printf "%s\n",$2}' >> resultados-mpi.csv
	done																																			
	NMSG=$(expr $NMSG / 2)
done
