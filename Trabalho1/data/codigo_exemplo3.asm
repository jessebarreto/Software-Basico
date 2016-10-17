SECTION TEXT
input n
load n
fat: sub one
jnpz fim
store aux
mult n
store n
load aux
jnp fat
fim: output n
stop
SECTION DATA
aux: space
n: space
one: const 1
