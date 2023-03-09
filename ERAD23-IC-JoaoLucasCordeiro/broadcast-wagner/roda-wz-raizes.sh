echo ------------- RAIZ = 0 ---------------------
./roda-wz-root.sh 0 2>&1 | grep NODE | sort -k 4
echo ------------- RAIZ = 1 ---------------------
./roda-wz-root.sh 1 2>&1 | grep NODE | sort -k 4
echo ------------- RAIZ = 3 ---------------------
./roda-wz-root.sh 3 2>&1 | grep NODE | sort -k 4
