# SO_Praticas

Resolução dos exercícios das aulas práticas.

Para correr o device:

1. cd /home/host/echo
2. sudo sh load.sh echo
3. gcc -Wextra example.c -o example
4. ./example /dev/echo
5. sudo sh remv.sh echo

Isto dá load, executa o codigo exemplo que abre, escreve, le e fecha o device.
(Falta fazer alinea 6.1 - mais do que uma device driver.)

Para correr o LAB3:

0. Abrir PuTTY com configuraçoes corretas e abrir a VM
1. cd host/LAB3/serp
2. sudo ./load.sh serp (deve aparece o char 'a' no putty que indica que o device foi instalado corretamente)
3. gcc -Wextra test.c -o test
4. ./test (escrever chars no PuTTY e eles devem aparecer no output do test)